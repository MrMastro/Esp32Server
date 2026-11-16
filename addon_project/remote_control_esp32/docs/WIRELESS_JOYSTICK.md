# Wireless joystick: collegamento telecomando ↔ dispositivi

> Stato: **progetto, non ancora implementato.** Questo documento raccoglie le decisioni prese in analisi
> ed è il punto di partenza per l'implementazione. Riguarda **due progetti**:
>
> - **telecomando**: `addon_project/remote_control_esp32` (ESP32-C6, core Arduino 3.x / pioarduino)
> - **dispositivi**: il progetto generale `Esp32Server` (ESP32 classico, core Arduino 2.x, LED)

## Obiettivo

Il telecomando mantiene un collegamento continuo con **più dispositivi** (fino a 10, quelli di
`/config/espDevices.json`). Ogni direzione del joystick viene inviata a **tutti i dispositivi collegati
contemporaneamente**; ognuno reagisce con i propri preset LED (`ledPresets.getByTrigger(...)`).

Requisiti:

- **cambio di direzione fluido** (latenza di pochi ms durante l'uso);
- **consumo di corrente limitato su entrambi i lati**: il telecomando va a power bank o piccola batteria,
  i dispositivi hanno batterie più grandi ma il consumo va comunque contenuto;
- **autenticazione** con la password di ogni dispositivo.

## Decisioni prese

| Tema | Decisione |
| --- | --- |
| Destinatari | Ogni direzione va a **tutti** i dispositivi collegati |
| Rete | Tutti (telecomando compreso) in **WIFI_MODE** sullo stesso router → stesso canale. Niente AP (consuma di più) |
| Trasporto comandi | **ESP-NOW unicast** verso ciascun dispositivo (non broadcast: niente conferma, niente firma per dispositivo) |
| Accoppiamento | **HTTP** una volta (all'avvio / alla riscoperta), con la password del dispositivo |
| Password | **Una per dispositivo**, in `espDevices.json` (campo `devicePassword`) |
| Primo comando dopo una pausa | Inviato **subito** dentro `SESSION_START`: ogni dispositivo lo applica appena si sveglia (sfasamento max ~100 ms, solo sul primo) |
| Durata sessione | **15 s** senza movimenti, **configurabile** |
| Joystick addormentato del progetto generale | **Ignorato**: il telecomando ha la priorità (stato prevedibile su tutti i dispositivi) |
| Joystick fisico sui dispositivi | Nuovo `joystickSettings.wirelessJoystick`: se `true` i pin vengono ignorati e si ascolta solo il telecomando |
| Sospendere / riprendere | Tenere premuto **SW per più di 5 s** sospende tutte le connessioni; altri 5 s le riprendono (vedi sotto) |

## Architettura

```
 TELECOMANDO (C6)                                   DISPOSITIVO (ESP32 Esp32Server) x N
 ─────────────────                                  ───────────────────────────────────
 avvio: GET  /api/v1/getInfoHello  ───────────────► (già esistente)
        POST /api/v1/remote/pair   ───────────────► verifica password, salva MAC telecomando,
             {nonce_r, mac, ...}   ◄─────────────── {nonce_d, channel}  → chiave di sessione

 joystick: SESSION_START/DIRECTION ──ESP-NOW──────► coda → task → recvButtonByJoystick()
           HEARTBEAT               ──ESP-NOW──────►        (solo se la direzione cambia)
                                   ◄──ESP-NOW────── HEARTBEAT_ACK / UNKNOWN_SESSION
           SESSION_END             ──ESP-NOW──────► torna in modem sleep
```

- **HTTP** solo per scoperta e accoppiamento: riusa quanto già fatto (`getInfoHello` all'avvio in background).
- **ESP-NOW** per tutto il resto: pacchetti diretti radio-radio, non passano dal router, conferma di
  ricezione automatica a livello radio.

## Risparmio energetico: sessioni a richiesta

Per ricevere, la radio di un dispositivo deve essere accesa. In modem sleep (default) si sveglia solo a
ogni beacon del router (~100 ms) e **i pacchetti ESP-NOW arrivati mentre dorme vanno persi** (non sono
bufferizzati dal router come il traffico WiFi normale). Quindi:

| Fase | Dispositivi | Telecomando |
| --- | --- | --- |
| **Riposo** | modem sleep attivo | modem sleep attivo; heartbeat raro, ritrasmesso finché confermato |
| **Primo movimento** | ricevono `SESSION_START` al primo risveglio (≤ ~100 ms) → `esp_wifi_set_ps(WIFI_PS_NONE)` | modem sleep off (massima reattività) |
| **Uso** | radio sempre accesa: direzioni in pochi ms | invio al cambio di direzione |
| **Fine** (15 s senza movimenti) | `SESSION_END` → `WIFI_PS_MIN_MODEM` | modem sleep di nuovo attivo |

Sicurezza: se un dispositivo non riceve nulla per `wirelessSessionTimeoutS` (più lungo di quello del
telecomando) torna **da solo** in modem sleep, così non resta acceso se il telecomando si spegne.

Altro risparmio sul telecomando: schermo spento col tasto BOOT (già fatto), CPU a 80 MHz a schermo spento.

Note:

- con le strisce LED accese il consumo è dominato dai LED; il risparmio sulla radio conta soprattutto a LED spenti;
- l'accoppiamento HTTP all'avvio richiede il modem sleep **spento** sul telecomando (con entrambi in sleep
  il primo contatto falliva: vedi README, sezione `espDevices.json`);
- il light sleep automatico (dorme anche la CPU) richiederebbe la gestione energetica di ESP-IDF, non attiva
  nel core 2.x precompilato del progetto generale: per ora si resta sul modem sleep a sessioni.

## Sospensione delle connessioni (pressione lunga di SW)

Tenendo premuto il tasto SW del joystick per più di **5 s** (`longPressS`, configurabile) il telecomando
**sospende** tutte le connessioni; ripetendo la pressione lunga le **riprende**.

### Convivenza con il toggle BUTTON

Oggi SW cambia la modalità BUTTON **sul fronte di pressione** (subito, per non aggiungere latenza): anche
una pressione lunga inizia quindi con un cambio di modalità, e i dispositivi ricevono la variante
`*_BUTTON` (o viceversa). Regola:

1. alla pressione: toggle BUTTON immediato e invio della nuova direzione (comportamento attuale, fluido);
2. se SW resta premuto fino a `longPressS`: il toggle viene **annullato** (si torna alla modalità di prima)
   e ai dispositivi si invia la direzione ripristinata, così restano nello stato precedente alla pressione;
3. poi la sospensione (o la ripresa).

Durante la pressione, dopo ~1 s, lo schermo mostra una barra di avanzamento verso i 5 s, così si capisce
che sta per scattare la sospensione e si può rilasciare prima per avere solo il toggle BUTTON.

### Sospensione

- il telecomando invia `REMOTE_PAUSE` a tutti i dispositivi collegati (con ritrasmissione finché confermato);
- i dispositivi chiudono la sessione, tornano in modem sleep e non si aspettano più heartbeat
  (nessun timeout di sicurezza da attendere);
- il telecomando smette di inviare direzioni e heartbeat e ferma la riscoperta periodica;
- il WiFi del telecomando resta collegato al router (OTA, API impostazioni e comandi seriali continuano a funzionare);
- a schermo: messaggio "CONNESSIONI SOSPESE" per ~2 s, poi i pallini restano visibili ma in **grigio**
  (stato "sospeso") con una scritta "PAUSA";
- il joystick continua a mostrare la direzione a schermo, ma non la invia a nessuno.

### Ripresa

- nuova pressione lunga: messaggio "CONNESSIONI RIPRESE";
- il telecomando invia un `HEARTBEAT` a ogni dispositivo: la chiave di sessione è ancora valida, quindi con
  `HEARTBEAT_ACK` il dispositivo torna subito collegato; con `UNKNOWN_SESSION` (es. dispositivo riavviato nel
  frattempo) o nessuna risposta si rifà l'accoppiamento HTTP;
- riparte la riscoperta periodica.

La sospensione **non** viene salvata: dopo un riavvio il telecomando riparte con le connessioni attive.

## Accoppiamento (HTTP)

### espDevices.json (telecomando)

Nuovo campo `devicePassword` (il `devicePassword` del `main_settings.json` di quel dispositivo):

```json
[
  {
    "deviceName": "Esp32Server",
    "ip": "172.17.252.192",
    "macAdress": "14:33:5C:02:7B:A0",
    "color": "#52A3BA",
    "devicePassword": "password_del_dispositivo"
  }
]
```

Il file resta non versionato (contiene password).

### POST /api/v1/remote/pair (progetto generale, nuovo)

Richiesta:

```json
{
  "deviceName": "Esp32Server",
  "devicePassword": "password_del_dispositivo",
  "remoteMac": "58:E6:C5:15:C3:90",
  "remoteNonce": "<16 byte casuali, hex>",
  "protocolVersion": 1
}
```

Risposta `200`:

```json
{
  "deviceNonce": "<16 byte casuali, hex>",
  "channel": 6,
  "macAdress": "14:33:5C:02:7B:A0",
  "protocolVersion": 1
}
```

Errori: `401` password errata, `400` richiesta non valida, `409` versione di protocollo non supportata.

Entrambi calcolano la **chiave di sessione**:

```
sessionKey = HMAC-SHA256(devicePassword, remoteNonce || deviceNonce || remoteMac || deviceMac)
```

La password non viaggia più dopo l'accoppiamento. Il dispositivo registra `remoteMac` come telecomando
autorizzato (peer ESP-NOW) con la sua `sessionKey`. Un nuovo accoppiamento sostituisce il precedente.

Non si usa la cifratura nativa di ESP-NOW: di default gestisce ~7 peer cifrati (ne servono 10) e cambiarlo
richiede di ricompilare il framework. Si usa invece una firma HMAC applicativa.

## Protocollo ESP-NOW

### Formato del pacchetto (15 byte)

| Campo | Byte | Note |
| --- | --- | --- |
| `version` | 1 | versione del protocollo (1) |
| `type` | 1 | vedi tabella tipi |
| `seq` | 4 | contatore crescente per sessione di accoppiamento; il dispositivo accetta solo `seq > ultimo` (anti-replay) |
| `direction` | 1 | codice direzione (tabella sotto) |
| `hmac` | 8 | primi 8 byte di `HMAC-SHA256(sessionKey, version..direction)` |

### Tipi

| Codice | Tipo | Verso | Significato |
| --- | --- | --- | --- |
| 1 | `SESSION_START` | tel → disp | esci dal modem sleep + applica `direction` |
| 2 | `DIRECTION` | tel → disp | nuova direzione |
| 3 | `HEARTBEAT` | tel → disp | sono vivo; contiene la direzione attuale (corregge pacchetti persi) |
| 4 | `SESSION_END` | tel → disp | torna in modem sleep |
| 5 | `HEARTBEAT_ACK` | disp → tel | heartbeat accettato |
| 6 | `UNKNOWN_SESSION` | disp → tel | firma non valida / sessione sconosciuta (es. dispositivo riavviato) → riaccoppiare |
| 7 | `REMOTE_PAUSE` | tel → disp | connessioni sospese dal telecomando (pressione lunga SW): chiudi la sessione, torna in modem sleep, non attendere heartbeat |

### Codici direzione (tabella esplicita, condivisa)

I codici sono **fissi** e vanno definiti in un header condiviso copiato in entrambi i progetti
(es. `WirelessProtocol.h`): non affidarsi all'ordine degli enum `J_DIRECTION`, che sono copie separate.

| Codice | Direzione | Codice | Direzione |
| --- | --- | --- | --- |
| 0 | `CENTER` | 9 | `CENTER_BUTTON` |
| 1 | `UP` | 10 | `UP_BUTTON` |
| 2 | `DOWN` | 11 | `DOWN_BUTTON` |
| 3 | `LEFT` | 12 | `LEFT_BUTTON` |
| 4 | `RIGHT` | 13 | `RIGHT_BUTTON` |
| 5 | `UP_LEFT` | 14 | `UP_LEFT_BUTTON` |
| 6 | `UP_RIGHT` | 15 | `UP_RIGHT_BUTTON` |
| 7 | `DOWN_LEFT` | 16 | `DOWN_LEFT_BUTTON` |
| 8 | `DOWN_RIGHT` | 17 | `DOWN_RIGHT_BUTTON` |
| 255 | `NONE` (nessuna direzione) | | |

Il dispositivo converte il codice nella stringa (`"UP_BUTTON"`) e la passa a `recvButtonByJoystick()`.

### Regole

- **Invio al cambio**: il telecomando invia `DIRECTION` appena la direzione cambia (lettura joystick ogni 10 ms),
  in unicast a ogni dispositivo collegato (10 dispositivi ≈ 10–20 ms).
- **Stato, non eventi**: ogni pacchetto porta la direzione *attuale*; un pacchetto perso viene corretto dal successivo.
- **Applica solo se cambia**: il dispositivo chiama `recvButtonByJoystick()` solo se `direction` è diversa
  dall'ultima applicata (altrimenti ogni heartbeat farebbe ripartire l'effetto).
- **Conferma radio ≠ accettato**: `ESP_NOW_SEND_SUCCESS` arriva anche se la firma è sbagliata. Per questo
  l'heartbeat richiede `HEARTBEAT_ACK`; a ogni heartbeat il telecomando tiene la radio accesa ~50–100 ms per
  riceverlo, poi torna in modem sleep.
- **Collegato** (pallino pieno): ultimo heartbeat confermato da `HEARTBEAT_ACK`. Dopo 3 heartbeat senza ACK →
  `ONLINE` (o `OFFLINE` se non risponde neanche via HTTP). `UNKNOWN_SESSION` → riaccoppiamento immediato.
- **Riscoperta**: ogni `rediscoveryS` il telecomando ritenta `getInfoHello` + accoppiamento solo verso i
  dispositivi non collegati (es. accesi dopo il telecomando).
- **Ricezione sul dispositivo**: la callback ESP-NOW gira nel task WiFi e deve solo mettere il pacchetto in
  una coda; un task dedicato verifica firma e `seq` e chiama `recvButtonByJoystick()`.
- **Cambio canale del router**: il collegamento si perde (niente ACK) → riaccoppiamento via HTTP, che
  riporta il nuovo `channel`.

## Configurazioni

### Telecomando: `/config/wireless.json` (nuovo, versionabile: nessun dato sensibile)

| Campo | Default | Significato |
| --- | --- | --- |
| `sessionTimeoutS` | `15` | secondi senza movimenti prima di `SESSION_END` |
| `heartbeatActiveMs` | `1000` | intervallo heartbeat durante la sessione |
| `heartbeatIdleS` | `15` | intervallo heartbeat a riposo |
| `missedHeartbeats` | `3` | heartbeat senza ACK prima di considerare il dispositivo non collegato |
| `rediscoveryS` | `60` | intervallo di riscoperta dei dispositivi non collegati |
| `pairAttempts` | `2` | tentativi HTTP per dispositivo (il primo contatto dopo l'avvio spesso fallisce) |
| `longPressS` | `5` | secondi di pressione di SW per sospendere / riprendere le connessioni |

Se il file manca si usano i default (come `joystick.json`).

### Dispositivo: `joystickSettings` in `main_settings.json` (progetto generale)

| Campo | Default | Significato |
| --- | --- | --- |
| `wirelessJoystick` | `false` | `true`: ignora i pin e il joystick fisico, ascolta solo il telecomando |
| `wirelessSessionTimeoutS` | `30` | se non riceve nulla per questo tempo torna da solo in modem sleep (deve essere > `sessionTimeoutS` del telecomando) |

Con `wirelessJoystick = true`: non si chiamano `pinMode`/`analogRead` sui pin del joystick, `serialCableTask`
non legge più il joystick fisico, e lo stato "joystick addormentato" (`sleepyJoystick` / `wakeUpJoystick`)
non blocca i comandi del telecomando.

## Modifiche previste

### Telecomando (`remote_control_esp32`)

- `espDevices.json`: campo `devicePassword`.
- Accoppiamento HTTP dopo `getInfoHello` nel task di scansione all'avvio; riscoperta periodica.
- Nuovo modulo ESP-NOW: peer, invio unicast a tutti, firme HMAC, `seq`, heartbeat adattivo, sessioni.
- Stato `CONNECTED` (pallino pieno) da `HEARTBEAT_ACK`.
- Pressione lunga di SW: rilevamento in `Joystick` (durata della pressione), annullamento del toggle BUTTON,
  sospensione/ripresa (`REMOTE_PAUSE`), barra di avanzamento e pallini grigi con "PAUSA" a schermo.
- Modem sleep: spento durante accoppiamento e sessione, attivo a riposo.
- `/config/wireless.json` con i parametri sopra.

### Progetto generale (`Esp32Server`)

- `JoystickSettingsModel`: `wirelessJoystick`, `wirelessSessionTimeoutS` (to/fromJson, default).
- Endpoint `POST /api/v1/remote/pair` (verifica password come `login`, nonce, chiave di sessione).
- Ricevitore ESP-NOW: callback → coda → task (verifica firma e `seq`) → `recvButtonByJoystick()` solo al cambio.
- Gestione sessioni: `esp_wifi_set_ps` a runtime, timeout di sicurezza.
- `REMOTE_PAUSE`: chiude la sessione e smette di attendere heartbeat da quel telecomando.
- `serialCableTask`: con `wirelessJoystick = true` non legge il joystick fisico.
- Attenzione: `takeExclusiveExecution()` qui è un flag, non un mutex (vedi `ServicesCollector`).

## Piano di lavoro

0. **Test di compatibilità ESP-NOW** C6 (ESP-NOW v2) ↔ ESP32 classico (v1), nei due versi, con pacchetti
   piccoli. Se fallisce, va rivista l'architettura: farlo per primo.
1. **Accoppiamento**: `devicePassword`, endpoint `/api/v1/remote/pair`, chiave di sessione su entrambi i lati.
2. **Direzioni**: pacchetti firmati a tutti, coda e task sul dispositivo, `wirelessJoystick`.
3. **Heartbeat con ACK**: pallino pieno, `UNKNOWN_SESSION` → riaccoppiamento, riscoperta periodica.
4. **Sessioni e risparmio energetico**: `SESSION_START`/`SESSION_END`, modem sleep a sessioni, timeout di sicurezza.
5. **Sospensione/ripresa** con pressione lunga di SW (`REMOTE_PAUSE`, annullamento del toggle BUTTON, schermo).
6. **Misure**: corrente a riposo / in sessione su entrambi (es. con un tester USB), latenza percepita.

## Da verificare

- Compatibilità ESP-NOW v1/v2 in pratica (fase 0).
- Che il core 2.x del progetto generale (ESP-IDF 4.4) supporti tutto il necessario; la modalità di risparmio
  energetico dedicata a ESP-NOW (*connectionless power save*) potrebbe richiedere ESP-IDF 5.x, cioè la
  migrazione del progetto generale al core 3.x (lavoro a sé: `ESPAsyncWebServer`, `NeoPixelBus`, ...).
- Peso del firmware del telecomando (oggi ~65% dello slot OTA): ESP-NOW e HMAC (mbedtls) sono già nel
  framework, l'aggiunta dovrebbe essere piccola, ma va controllata per mantenere le due partizioni OTA.
