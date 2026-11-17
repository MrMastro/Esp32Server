# Wireless joystick: collegamento telecomando ↔ dispositivi

> Stato (2026-11-17): **in implementazione**.
>
> | Fase | Stato |
> | --- | --- |
> | 0. Compatibilità ESP-NOW | ✅ fatta e provata |
> | 1. Accoppiamento | ✅ fatta e provata |
> | 2. Direzioni firmate | ✅ fatta e provata |
> | 3. Heartbeat, pallino pieno, riaccoppiamento | ✅ fatta e provata |
> | 4. Sessioni e risparmio energetico | ✅ fatta e provata (riposo, risveglio al primo movimento, timeout di sicurezza) |
> | 5. Pressione lunga di SW (sospensione / ripresa) | ⬜ **non ancora implementata**: oggi SW fa solo il toggle BUTTON |
> | 6. Misure di corrente e latenza | ⬜ da fare |
>
> Per controllare da soli se funziona: [Come verificare](#come-verificare). Riguarda **due progetti**:
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
| Accoppiamento | **HTTP** una volta (all'avvio / alla riscoperta / alla ripresa), con `deviceName` e password del dispositivo. È anche il **primo contatto**: `getInfoHello` non si usa |
| Password | **Una per dispositivo**, in `espDevices.json` (campo `devicePassword`); il `deviceName` di `espDevices.json` deve essere **uguale** a quello del dispositivo |
| Primo comando dopo una pausa | Inviato **subito** dentro `SESSION_START`: ogni dispositivo lo applica appena si sveglia (sfasamento max ~100 ms, solo sul primo) |
| Durata sessione | **15 s** senza movimenti, **configurabile** |
| Joystick addormentato del progetto generale | **Ignorato**: il telecomando ha la priorità (stato prevedibile su tutti i dispositivi) |
| Joystick fisico sui dispositivi | Nuovo `joystickSettings.remoteControlWireless`: se `true` i pin vengono ignorati e si ascolta solo il telecomando |
| Sospendere / riprendere | Tenere premuto **SW per più di 5 s** sospende il telecomando (per i dispositivi è **come spegnerlo**, ma vengono avvisati subito); un'altra pressione lunga lo riprende **come all'accensione** (vedi sotto) |

## Architettura

```
 TELECOMANDO (C6)                                   DISPOSITIVO (ESP32 Esp32Server) x N
 ─────────────────                                  ───────────────────────────────────
 avvio /   POST /api/v1/remote/pair ──────────────► verifica deviceName + password, salva MAC telecomando,
 riscoperta  {nonce_r, mac, ...}   ◄─────────────── {nonce_d, channel}  → chiave di sessione (RAM)

 joystick: DIRECTION (al cambio)    ──ESP-NOW─────► coda → task wireless → verifica mittente, firma, seq
           HEARTBEAT (dir. attuale) ──ESP-NOW─────►   → recvButtonByJoystick() solo se la direzione cambia
                                   ◄──ESP-NOW────── HEARTBEAT_ACK (firmato)    → pallino pieno
                                   ◄──ESP-NOW────── UNKNOWN_SESSION (non firm.) → riaccoppiamento immediato
           SESSION_START / _END     ──ESP-NOW─────► esci / torna in modem sleep (fase 4)
```

- **HTTP** solo per l'accoppiamento, che fa anche da scoperta (all'avvio, in background). `getInfoHello`
  resta nel progetto generale e nel comando seriale `hello` per la diagnosi, ma il telecomando non lo usa.
- **ESP-NOW** per tutto il resto: pacchetti diretti radio-radio, non passano dal router, conferma di
  ricezione automatica a livello radio.

### Moduli

| Progetto | File | Ruolo |
| --- | --- | --- |
| entrambi | `src/wireless/WirelessProtocol.h` | **identico** nei due progetti: costanti, MAC/hex, nonce, chiave di sessione, impronta, pacchetto da 15 byte, firma/verifica, codici direzione |
| telecomando | `src/wireless/WirelessPairing.*` | accoppiamento HTTP (`wirelessPair`) e sessioni per dispositivo (chiave, `seq`, stato heartbeat) |
| telecomando | `src/wireless/WirelessLink.*` | ESP-NOW: peer, invio `DIRECTION`/`HEARTBEAT` firmati, coda delle risposte, statistiche |
| telecomando | `src/wireless/WirelessManager.*` | task wireless (risposte, heartbeat adattivo, riscoperta) e task di accoppiamento (coda, fuori dal lock) |
| telecomando | `src/wireless/WirelessConfig.*` | lettura di `/config/wireless.json` |
| telecomando | `src/devices/EspDevices.*` | `espDevices.json`, stato dei pallini, client HTTP minimo (GET/POST) |
| dispositivo | `src/controllers/RemoteControllers.cpp` | endpoint `POST /api/v1/remote/pair` |
| dispositivo | `src/wireless/RemotePairing.*` | telecomando accoppiato (MAC, chiave, `generation`), protetto da sezione critica |
| dispositivo | `src/wireless/WirelessReceiver.*` | ricevitore ESP-NOW: verifica, applicazione della direzione, `HEARTBEAT_ACK` / `UNKNOWN_SESSION` |
| dispositivo | `src/Main.cpp` (`remoteControlWirelessTask`) | task wireless, creato dopo il WiFi solo con `remoteControlWireless = true` |

Comandi seriali del telecomando: `devices` (stato dei dispositivi, statistiche ESP-NOW, sessioni con impronta
della chiave, ultimo ACK, heartbeat persi), `pair` (riaccoppia tutti i dispositivi in background),
`hello,<ip>` (diagnosi con `getInfoHello`).

## Risparmio energetico: sessioni a richiesta

Per ricevere, la radio di un dispositivo deve essere accesa. In modem sleep (default) si sveglia solo a
ogni beacon del router (~100 ms) e **i pacchetti ESP-NOW arrivati mentre dorme vanno persi** (non sono
bufferizzati dal router come il traffico WiFi normale). Quindi:

| Fase | Dispositivi | Telecomando |
| --- | --- | --- |
| **Riposo** | modem sleep (`WiFi.setSleep(true)`) | modem sleep; `HEARTBEAT` ogni `heartbeatIdleS`, **ritrasmesso** finché la radio del dispositivo conferma (finestra 600 ms); radio accesa ~150 ms in più per ricevere l'ACK |
| **Primo movimento** | ricevono `SESSION_START` (con la direzione) al primo risveglio → radio sempre accesa, direzione applicata | apre la sessione: `SESSION_START` **ritrasmesso** a ogni dispositivo finché la radio conferma (finestra 600 ms); modem sleep spento |
| **Uso** | radio sempre accesa: direzioni in pochi ms | `DIRECTION` al cambio; ogni `heartbeatActiveMs` un `SESSION_START` (fa da heartbeat, risveglia un dispositivo riaddormentato e ne rinnova il timer) |
| **Fine** (`sessionTimeoutS` = 15 s senza movimenti) | `SESSION_END` → modem sleep | `SESSION_END` affidabile a tutti, modem sleep di nuovo attivo |

Sicurezza: se un dispositivo non riceve `SESSION_START` o `DIRECTION` per `wirelessSessionTimeoutS`
(30 s, più lungo di quello del telecomando) chiude **da solo** la sessione e torna in modem sleep, così non
resta acceso se il telecomando si spegne. Gli `HEARTBEAT` a riposo **non** rinnovano questo timer (altrimenti
un `SESSION_END` perso lo terrebbe sveglio per sempre).

**Invio affidabile** (telecomando): `SESSION_START`, `HEARTBEAT` e `SESSION_END` vengono ritrasmessi ogni
~40 ms finché arriva la conferma radio del dispositivo o scade la finestra; ogni ritrasmissione porta la
direzione **attuale** e un `seq` nuovo. Le mancate conferme durante le ritrasmissioni (dispositivo che dorme)
sono normali e compaiono in `devices` come "non confermati"; "invii affidabili scaduti" conta invece le
finestre finite senza nessuna conferma (dispositivo spento o fuori portata).

Il modem sleep del telecomando si può disattivare con `remoteModemSleep: false` in `wireless.json` (utile per
il debug o se il risveglio dà problemi).

Altro risparmio sul telecomando: schermo spento col tasto BOOT (già fatto), CPU a 80 MHz a schermo spento.

Note:

- con le strisce LED accese il consumo è dominato dai LED; il risparmio sulla radio conta soprattutto a LED spenti;
- l'accoppiamento HTTP richiede il modem sleep **spento** sul telecomando (con entrambi in sleep il primo
  contatto falliva: vedi README, sezione `espDevices.json`): il task di accoppiamento lo spegne per tutta la
  durata;
- il light sleep automatico (dorme anche la CPU) richiederebbe la gestione energetica di ESP-IDF, non attiva
  nel core 2.x precompilato del progetto generale: per ora si resta sul modem sleep a sessioni.

## Sospensione delle connessioni (pressione lunga di SW)

Tenendo premuto il tasto SW del joystick per più di **5 s** (`longPressS`, configurabile) il telecomando
**sospende** il joystick wireless; ripetendo la pressione lunga lo **riprende**.

Per i dispositivi la sospensione equivale a **spegnere il telecomando**: tornano a funzionare come senza
telecomando. L'unica differenza è che vengono **avvisati subito** invece di accorgersene allo scadere del
timeout di sicurezza. La ripresa equivale a **riaccendere il telecomando**: si rifanno tutti i passaggi
dell'avvio (contatto, accoppiamento, sessione).

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

Telecomando:

- invia `REMOTE_PAUSE` (firmato) a ogni dispositivo accoppiato e attende `REMOTE_PAUSE_ACK`, con qualche
  ritrasmissione (es. 3 tentativi in ~300 ms); se un dispositivo non conferma si va avanti lo stesso
  (scatterà il suo timeout di sicurezza, come con il telecomando spento);
- **interrompe la comunicazione radio con i dispositivi**: niente più direzioni, heartbeat né riscoperta;
  cancella le chiavi di sessione (alla ripresa se ne fanno di nuove);
- il **WiFi resta collegato al router**: OTA, API impostazioni e comandi seriali continuano a funzionare;
- a schermo: messaggio "CONNESSIONI SOSPESE" per ~2 s, poi i pallini restano visibili ma in **grigio**
  (stato "sospeso") con una scritta "PAUSA";
- il joystick continua a mostrare la direzione a schermo, ma non la invia a nessuno.

Dispositivi, alla ricezione di `REMOTE_PAUSE` (solo se la firma è valida):

- rispondono `REMOTE_PAUSE_ACK`;
- chiudono la sessione, tornano in modem sleep e **dimenticano l'accoppiamento** (MAC e chiave del
  telecomando): da qui in poi rifiutano i pacchetti di quel telecomando finché non si riaccoppia;
- tornano all'**esecuzione senza joystick**: l'effetto LED in corso **continua com'è**, e il dispositivo
  risponde solo ad API, seriale e comandi ritardati. Il joystick fisico resta disattivato
  (`remoteControlWireless = true`).

### Ripresa

Nuova pressione lunga: messaggio "CONNESSIONI RIPRESE", poi **gli stessi passaggi dell'accensione**:

1. accoppiamento HTTP (`/api/v1/remote/pair`) direttamente con ogni dispositivo di `espDevices.json`
   (niente `getInfoHello`): nuovi nonce, **nuova chiave di sessione**;
2. heartbeat e sessioni come al solito; riparte la riscoperta periodica.

I pallini ripartono dagli stati dell'avvio (vuoto → mezzo pieno → pieno) man mano che i dispositivi
rispondono. Servono alcuni secondi prima di poter comandare i dispositivi.

La sospensione **non** viene salvata: dopo un riavvio il telecomando riparte con le connessioni attive.

## Accoppiamento (HTTP)

### espDevices.json (telecomando)

Nuovo campo `devicePassword` (il `devicePassword` del `settings.json` di quel dispositivo). `deviceName`
deve essere **uguale** al `deviceName` del `settings.json` del dispositivo: l'accoppiamento li verifica
entrambi, come il login.

```json
[
  {
    "deviceName": "MastroDevice",
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
  "deviceName": "MastroDevice",
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

Errori: `400` richiesta non valida, `401` credenziali errate, `403` `remoteControlWireless` disattivato
sul dispositivo, `409` versione di protocollo non supportata.

`deviceName` e `devicePassword` sono quelli di `espDevices.json` e devono coincidere con il `settings.json`
del dispositivo: la verifica è la stessa del login.

L'accoppiamento è anche il **primo contatto** con il dispositivo (`getInfoHello` non si usa). Esito sul
telecomando:

| Risposta | Stato (pallino) |
| --- | --- |
| `200` | accoppiato: `ONLINE` (mezzo pieno), poi `CONNECTED` (pieno) al primo `HEARTBEAT_ACK` |
| errore HTTP (`401`, `403`, ...) o MAC diverso | raggiungibile ma non accoppiato: `ONLINE`, motivo in `devices` |
| nessuna risposta | `OFFLINE` (si ritenta una volta: il primo contatto dopo l'avvio spesso fallisce) |
| `devicePassword` mancante in `espDevices.json` | `OFFLINE`, non contattato |

Entrambi calcolano la **chiave di sessione**:

```
sessionKey = HMAC-SHA256(devicePassword, remoteNonce || deviceNonce || remoteMac || deviceMac)
```

Nonce e MAC sono concatenati **in binario**: 16 + 16 + 6 + 6 = 44 byte (nel JSON i nonce viaggiano in hex,
i MAC come `AA:BB:CC:DD:EE:FF`). I nonce vengono dal generatore hardware (`esp_fill_random`).
Costanti e funzioni sono in `WirelessProtocol.h`, **identico** nei due progetti
(`src/wireless/` in entrambi).

La password non viaggia più dopo l'accoppiamento. Il dispositivo registra `remoteMac` come telecomando
autorizzato con la sua `sessionKey` (solo in RAM: dopo un suo riavvio il telecomando deve riaccoppiarsi).
Un nuovo accoppiamento sostituisce il precedente.

Per verificare che i due lati abbiano la stessa chiave senza stamparla, entrambi scrivono nel log
un'**impronta**: i primi 4 byte di `HMAC-SHA256(sessionKey, "fingerprint")`, in hex.

Sul telecomando l'accoppiamento avviene nel **task di accoppiamento** di `WirelessManager` (coda di
dispositivi, uno alla volta, in background): all'avvio, con il comando `pair`, alla riscoperta e quando un
dispositivo va riaccoppiato (vedi [Regole](#regole)).

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
| 7 | `REMOTE_PAUSE` | tel → disp | telecomando sospeso (pressione lunga SW): chiudi la sessione, torna in modem sleep, dimentica l'accoppiamento, torna all'esecuzione senza joystick |
| 8 | `REMOTE_PAUSE_ACK` | disp → tel | `REMOTE_PAUSE` accettato |

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
  l'heartbeat richiede `HEARTBEAT_ACK` (il dispositivo lo invia sia a `HEARTBEAT` sia a `SESSION_START`).
- **Collegato** (pallino pieno): ultimo heartbeat confermato da `HEARTBEAT_ACK` (firmato, `seq` crescente
  anche in questo verso). Dopo `missedHeartbeats` heartbeat senza ACK il dispositivo torna `ONLINE`
  (o `OFFLINE` se non risponde neanche via HTTP) e:
  - se da quell'accoppiamento aveva già risposto almeno una volta → **riaccoppiamento immediato**;
  - se non ha **mai** risposto via ESP-NOW (risponde in HTTP ma non in radio: firmware senza heartbeat,
    canale diverso, ...) → si riprova solo alla **riscoperta** successiva, per non fare un ciclo continuo
    di accoppiamenti HTTP (visto in prova con il firmware vecchio sul dispositivo: uno ogni ~3 s).
- `UNKNOWN_SESSION` → **riaccoppiamento immediato**. Non è firmato (il dispositivo non ha la chiave giusta):
  nel caso peggiore un estraneo provoca un riaccoppiamento. Il dispositivo lo invia al massimo una volta al
  secondo, in risposta ai soli pacchetti che manda il telecomando; a un `seq` vecchio/ripetuto non risponde.
- **Heartbeat adattivo**: in sessione ogni `heartbeatActiveMs` (come `SESSION_START`), a riposo ogni
  `heartbeatIdleS` (come `HEARTBEAT`); il primo parte subito dopo ogni accoppiamento. Porta la direzione
  attuale: dopo un (ri)accoppiamento il dispositivo si allinea subito alla direzione del telecomando.
- **Riscoperta**: ogni `rediscoveryS` il telecomando ritenta l'accoppiamento solo verso i
  dispositivi non accoppiati (es. accesi dopo il telecomando) e con `devicePassword` configurata.
- **Accoppiamento in un task a parte** (coda di dispositivi, uno alla volta): un dispositivo spento (fino a
  `pairAttempts` × 7 s di attesa HTTP) non blocca gli heartbeat verso gli altri.
- **Ricezione sul dispositivo**: la callback ESP-NOW gira nel task WiFi e mette solo il pacchetto in una
  coda; il task wireless verifica, nell'ordine, telecomando accoppiato, mittente, versione, firma e
  `seq > ultimo`, poi applica la direzione. A ogni nuovo accoppiamento (`generation` diversa) `seq` e ultima
  direzione ripartono da zero. Gli scarti finiscono nel log al massimo una volta al secondo.
- **Cambio canale del router**: il collegamento si perde (niente ACK) → riaccoppiamento via HTTP, che
  riporta il nuovo `channel`.

## Configurazioni

### Telecomando: `/config/wireless.json` (versionato in `sd_card/config/`: nessun dato sensibile)

| Campo | Default | Significato |
| --- | --- | --- |
| `sessionTimeoutS` | `15` | secondi senza movimenti prima di `SESSION_END` |
| `heartbeatActiveMs` | `1000` | intervallo heartbeat durante la sessione |
| `heartbeatIdleS` | `15` | intervallo heartbeat a riposo |
| `missedHeartbeats` | `3` | heartbeat senza ACK prima di considerare il dispositivo non collegato |
| `rediscoveryS` | `60` | intervallo di riscoperta dei dispositivi non collegati |
| `pairAttempts` | `2` | tentativi HTTP per dispositivo (il primo contatto dopo l'avvio spesso fallisce) |
| `longPressS` | `5` | secondi di pressione di SW per sospendere / riprendere le connessioni (fase 5) |
| `remoteModemSleep` | `true` | modem sleep del telecomando a riposo; `false` = radio sempre accesa |

Se il file manca si usano i default (come `joystick.json`); all'avvio il telecomando scrive nel log i valori
usati (`[WirelessConfig] ...`).

### Dispositivo: `joystickSettings` in `settings.json` (progetto generale, `data/settings/`)

| Campo | Default | Significato |
| --- | --- | --- |
| `remoteControlWireless` | `false` | `true`: ignora i pin e il joystick fisico, ascolta solo il telecomando (**fatto**) |
| `wirelessSessionTimeoutS` | `30` | senza `SESSION_START`/`DIRECTION` per questo tempo chiude la sessione e torna in modem sleep (deve essere > `sessionTimeoutS` del telecomando; minimo 5) (**fatto**) |

Con `remoteControlWireless = true`: non si chiamano `pinMode`/`analogRead` sui pin del joystick, `serialCableTask`
non legge più il joystick fisico, e lo stato "joystick addormentato" (`sleepyJoystick` / `wakeUpJoystick`)
non blocca i comandi del telecomando. Il task wireless parte solo se il WiFi è attivo (in modalità
Bluetooth no). Se manca nel file vale `false`.

Firmware del progetto generale: l'ambiente `Esp32Com` esclude il Bluetooth (`-DEXCLUDE_BLUETOOTH`) e usa
`min_spiffs.csv` (due slot OTA); `Esp32Server` è lo stesso firmware caricato via OTA; `Esp32ComBT` tiene il
Bluetooth (`huge_app.csv`, niente OTA).

## Stato dell'implementazione

### Telecomando (`remote_control_esp32`)

| Cosa | Stato |
| --- | --- |
| `espDevices.json`: campo `devicePassword` (+ `espDevices.json.example` versionato) | fatto |
| Accoppiamento HTTP diretto (senza `getInfoHello`) in un task a parte; riscoperta periodica | fatto |
| ESP-NOW: peer, invio unicast a tutti, firme HMAC, `seq`, heartbeat adattivo | fatto |
| Stato `CONNECTED` (pallino pieno) da `HEARTBEAT_ACK`; `UNKNOWN_SESSION` → riaccoppiamento | fatto |
| `/config/wireless.json` (`WirelessConfig`) | fatto (`longPressS` usato nella fase 5) |
| Sessioni `SESSION_START` / `SESSION_END`, invio affidabile con ritrasmissione | fatto e provato (riposo e risveglio) |
| Modem sleep: spento durante accoppiamento, sessione e attesa ACK, attivo a riposo (`remoteModemSleep`) | fatto e provato |
| Pressione lunga di SW: annullamento del toggle BUTTON, sospensione (`REMOTE_PAUSE` + attesa di `REMOTE_PAUSE_ACK`, chiavi cancellate, WiFi collegato), ripresa come all'avvio, barra di avanzamento e pallini grigi con "PAUSA" | fase 5 |

### Progetto generale (`Esp32Server`)

| Cosa | Stato |
| --- | --- |
| `JoystickSettingsModel.remoteControlWireless` (to/fromJson, default `false`); joystick fisico disattivato | fatto |
| Endpoint `POST /api/v1/remote/pair` (verifica come `login`, nonce, chiave di sessione) | fatto |
| Ricevitore ESP-NOW: callback → coda → task (mittente, firma, `seq`) → `recvButtonByJoystick()` solo al cambio | fatto |
| `HEARTBEAT_ACK` firmato, `UNKNOWN_SESSION` (max 1/s) | fatto |
| `wirelessSessionTimeoutS`, sessioni con modem sleep a riposo (`WiFi.setSleep`), timeout di sicurezza | fatto e provato |
| `REMOTE_PAUSE`: `REMOTE_PAUSE_ACK`, chiude la sessione, dimentica l'accoppiamento, esecuzione senza joystick | fase 5 |

Attenzione: nel progetto generale `takeExclusiveExecution()` è un flag, non un mutex (vedi `ServicesCollector`).

## Piano di lavoro

0. ~~**Test di compatibilità ESP-NOW** C6 (ESP-NOW v2) ↔ ESP32 classico (v1), nei due versi, con pacchetti
   piccoli.~~ **Fatto (2026-11-17): funziona**, vedi [Fase 0: risultati](#fase-0-risultati).
1. **Accoppiamento**: `devicePassword`, endpoint `/api/v1/remote/pair`, chiave di sessione su entrambi i lati.
   **Fatto (2026-11-17)**: endpoint verificato dal PC (400/401/409/200 e chiave identica a un calcolo
   indipendente); telecomando ↔ dispositivo accoppiati con la stessa impronta della chiave sui due lati
   (accoppiamento diretto, senza `getInfoHello`).
2. **Direzioni**: pacchetti firmati a tutti, coda e task sul dispositivo, `remoteControlWireless`.
   **Fatto (2026-11-17)**: `WirelessLink` (telecomando) invia `DIRECTION` firmato al cambio di direzione,
   `WirelessReceiver` (dispositivo) verifica mittente, firma e `seq` e chiama `recvButtonByJoystick()` solo al
   cambio. Prova di 30 s muovendo il joystick: 136 direzioni applicate, 0 scartate; dall'avvio 228 inviati,
   228 confermati dalla radio. Modem sleep ancora spento sul dispositivo (fase 4).
3. **Heartbeat con ACK**: pallino pieno, `UNKNOWN_SESSION` → riaccoppiamento, riscoperta periodica.
   **Fatto (2026-11-17)**: `WirelessManager` (telecomando: task heartbeat/risposte/riscoperta + task di
   accoppiamento), `WirelessConfig` (`wireless.json`), risposte firmate in `WirelessReceiver` (dispositivo).
   Provato: firmware vecchio sul dispositivo → niente ciclo di riaccoppiamenti, riprova alla riscoperta →
   collegato appena aggiornato; dispositivo riavviato → `UNKNOWN_SESSION` → riaccoppiato e collegato in pochi
   secondi senza intervento.
4. **Sessioni e risparmio energetico**: `SESSION_START`/`SESSION_END`, modem sleep a sessioni, timeout di sicurezza.
   **Implementato e caricato (2026-11-17)**. Provato il **riposo**: dopo 15 s senza movimenti
   `SESSION_END`, dispositivo e telecomando in modem sleep; gli `HEARTBEAT` ogni 15 s lo raggiungono (27 invii,
   9 ritrasmissioni verso il dispositivo addormentato, **0 finestre scadute**) e resta `CONNECTED`.
   Provato il **risveglio**: dopo `SESSION_END` il dispositivo va in modem sleep; al primo movimento
   `inizio sessione` e la direzione (`UP`) applicata subito, nessun movimento perso; su 2025 invii 44
   ritrasmissioni verso il dispositivo addormentato, 0 finestre scadute.
   Provato il **timeout di sicurezza**: telecomando staccato durante una sessione → 30 s dopo l'ultimo
   movimento il dispositivo scrive `fine sessione (nessun movimento per 30 s): modem sleep`.
   **Fase 4 fatta (2026-11-17).**
5. **Sospensione/ripresa** con pressione lunga di SW (`REMOTE_PAUSE`/`REMOTE_PAUSE_ACK`, annullamento del
   toggle BUTTON, ripresa come all'avvio, schermo). **Non ancora implementata.**
6. **Misure**: corrente a riposo / in sessione su entrambi (es. con un tester USB), latenza percepita.

## Come verificare

Servono i due monitor seriali: telecomando (USB del C6) e dispositivo (CH340). Sul telecomando il comando
`devices` riassume tutto: stato e pallino di ogni dispositivo, statistiche ESP-NOW, sessione attiva o a
riposo, modem sleep, ultimo ACK e heartbeat persi.

| Situazione | Telecomando: cosa deve comparire | Dispositivo: cosa deve comparire |
| --- | --- | --- |
| Avvio | `[WirelessConfig] ...`, `[Wireless] <nome> accoppiato: ... impronta chiave XXXXXXXX`, poi `collegato (HEARTBEAT_ACK)`; pallino **pieno** | `[Wireless] ricevitore ESP-NOW attivo ... modem sleep a riposo`, `telecomando ... accoppiato ... impronta chiave XXXXXXXX` (**stessa impronta**) |
| Movimento del joystick | — (in `devices`: "sessione joystick attiva") | `inizio sessione: radio sempre accesa`, poi `Joystick direction: ...` a ogni cambio, mai due uguali di fila |
| 15 s fermo | `fine sessione (15 s senza movimenti)` | `fine sessione (SESSION_END dal telecomando): modem sleep` |
| A riposo | `devices`: `CONNECTED`, "ultimo ACK" sotto i 15 s circa, "invii affidabili scaduti" che non cresce | nessun messaggio |
| Primo movimento dopo la pausa | — | `inizio sessione` e la direzione applicata; ritardo percepito al massimo di qualche centinaio di ms, solo su questo movimento |
| Dispositivo riavviato | `sessione sconosciuta al dispositivo (riavviato?), riaccoppiamento`, poi `accoppiato` e `collegato` | `pacchetto scartato ... nessun telecomando accoppiato`, poi `telecomando ... accoppiato` |
| Telecomando spento durante una sessione | — | dopo 30 s: `fine sessione (nessun movimento per 30 s): modem sleep` |
| Dispositivo spento | dopo 3 heartbeat persi: `... heartbeat senza risposta, riaccoppiamento`, poi `non accoppiato: ...`; pallino **vuoto** | — |

Se qualcosa non torna: `pair` riaccoppia tutto; `remoteModemSleep: false` in `wireless.json` esclude il
modem sleep del telecomando dalle cause.

## Fase 0: risultati

Test del 2026-11-17: telecomando (ESP32-C6, core 3.3 / ESP-IDF 5.5, **ESP-NOW v2**) ↔ Esp32Server
(ESP32 classico, core 2.0.17 / ESP-IDF 4.4, **ESP-NOW v1**), entrambi in WIFI_MODE sullo stesso router
(canale 1), pacchetti da 16 byte, modem sleep spento su entrambi, effetto LED attivo sul dispositivo.

| Prova | Conferme radio | PONG | Andata e ritorno (min / medio / max) |
| --- | --- | --- | --- |
| 20 PING, uno ogni 100 ms | 20/20 | 20/20 | 3,8 / 4,7 / 9,5 ms |
| 200 PING, uno ogni 100 ms | 200/200 | 200/200 | 3,6 / 4,5 / 10,8 ms |

Conclusioni: ESP-NOW funziona **nei due versi** tra v2 e v1 con pacchetti piccoli, senza perdite, con
latenza di pochi ms (sola andata ≈ metà). L'architettura resta quella descritta sopra.

Il codice del test (comando `espnow_test` sul telecomando, `EspNowTestResponder` sul dispositivo) è
stato rimosso nella fase 2, sostituito dal protocollo vero: resta nella storia di git.

## Da verificare

- ~~Compatibilità ESP-NOW v1/v2 in pratica (fase 0).~~ Verificata: vedi [Fase 0: risultati](#fase-0-risultati).
- Comportamento con il **modem sleep attivo** sul dispositivo (fase 4): funziona a riposo (heartbeat
  consegnati con poche ritrasmissioni) e al risveglio (primo movimento applicato, niente perso); il ritardo
  del primo movimento resta da misurare (fase 6).
- I LED a volte non si aggiornavano finché il dispositivo non è stato riavviato (2026-11-17): la radio e
  l'avvio degli effetti funzionavano (log). Da tenere d'occhio. Possibile causa da sistemare comunque: task
  radio e task LED toccano l'effetto in corso senza un vero lucchetto (`takeExclusiveExecution()` è un flag);
  era così anche con il joystick fisico.
- Che il core 2.x del progetto generale (ESP-IDF 4.4) supporti tutto il necessario; la modalità di risparmio
  energetico dedicata a ESP-NOW (*connectionless power save*) potrebbe richiedere ESP-IDF 5.x, cioè la
  migrazione del progetto generale al core 3.x (lavoro a sé: `ESPAsyncWebServer`, `NeoPixelBus`, ...).
- Peso dei firmware dopo la fase 4: telecomando **66,7%** dello slot OTA da 1,9 MB, progetto generale
  (`Esp32Com`, senza Bluetooth) **68,4%**: c'è margine per la fase 5.
- Dispositivo spento con il telecomando collegato: atteso → 3 heartbeat persi → riaccoppiamento → nessuna
  risposta HTTP → `OFFLINE` (pallino vuoto), poi riscoperta ogni 60 s. Non ancora provato.
