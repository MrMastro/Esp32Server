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
> | 5. Pressione lunga di SW (disconnessione / connessione) | 🟡 provata; poi cambiato lo schermo (slider al posto di "in corso", conferma breve, pallini vuoti da disconnessi): **da riprovare** |
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
| Accoppiamento | **HTTP** una volta (all'avvio / alla riscoperta / alla riconnessione con SW), con `deviceName` e password del dispositivo. È anche il **primo contatto**: `getInfoHello` non si usa |
| Password | **Una per dispositivo**, in `espDevices.json` (campo `devicePassword`); il `deviceName` di `espDevices.json` deve essere **uguale** a quello del dispositivo |
| Primo comando dopo il riposo (fine sessione) | Inviato **subito** dentro `SESSION_START`: ogni dispositivo lo applica appena si sveglia (sfasamento max ~100 ms, solo sul primo) |
| Durata sessione | **15 s** senza movimenti, **configurabile** |
| Joystick addormentato del progetto generale | **Ignorato**: il telecomando ha la priorità (stato prevedibile su tutti i dispositivi) |
| Joystick fisico sui dispositivi | Nuovo `joystickSettings.remoteControlWireless`: se `true` i pin vengono ignorati e si ascolta solo il telecomando |
| Disconnettere / connettere | Tenere premuto **SW per più di 5 s** **disconnette** tutti i dispositivi (per loro è **come spegnere il telecomando**, ma vengono avvisati subito); un'altra pressione lunga **rifà la connessione** come all'accensione (vedi sotto). Non è una pausa: i dispositivi dimenticano il telecomando |

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

## Disconnessione e connessione (pressione lunga di SW)

Tenendo premuto il tasto SW del joystick per più di **5 s** (`longPressS`, configurabile) il telecomando
**disconnette** tutti i dispositivi; ripetendo la pressione lunga **rifà la connessione**.

Non è una pausa: i dispositivi vengono **disconnessi davvero**. Per loro equivale a **spegnere il
telecomando** (tornano a funzionare come senza telecomando e lo dimenticano); l'unica differenza è che
vengono **avvisati subito** invece di accorgersene allo scadere del timeout di sicurezza. La connessione
successiva equivale a **riaccendere il telecomando**: si rifà l'accoppiamento da capo, con nuove chiavi.

### Convivenza con il toggle BUTTON

SW cambia la modalità BUTTON **sul fronte di pressione** (subito, per non aggiungere latenza): anche
una pressione lunga inizia quindi con un cambio di modalità, e i dispositivi ricevono la variante
`*_BUTTON` (o viceversa). Regola:

1. alla pressione: toggle BUTTON immediato e invio della nuova direzione (comportamento normale, fluido);
2. se SW resta premuto fino a `longPressS`: il toggle viene **annullato** (si torna alla modalità di prima)
   e ai dispositivi si invia la direzione ripristinata, così restano nello stato precedente alla pressione;
3. poi la disconnessione (o la connessione).

### A schermo

- Pressione **sotto i 2 s**: nulla (pressione normale, solo toggle BUTTON).
- **Da 2 s a 5 s**: in fondo allo schermo la scritta "DISCONNESSIONE" (o "CONNESSIONE" se i dispositivi
  sono disconnessi) e una barra che si riempie; rilasciando prima resta solo il toggle BUTTON. Se
  `longPressS` è minore di 4 s, la barra parte a metà del tempo.
- **A 5 s**: nello stesso punto la barra diventa **subito uno slider animato** (cursore che va avanti e
  indietro) con la stessa scritta, finché il lavoro non è finito; poi una **conferma breve** a tutto schermo
  (~1 s):

| Azione | Slider fino a | Conferma |
| --- | --- | --- |
| Disconnessione | tutti i dispositivi hanno confermato (o 500 ms) | "DISCONNESSO"; poi pallini **vuoti** (i dispositivi per il telecomando sono offline) e la scritta "DISCONNESSO" sotto la direzione |
| Connessione | il **primo dispositivo risulta collegato** (al massimo 15 s) | "CONNESSO" (quali dispositivi lo sono si vede dai pallini); se nessuno accetta l'accoppiamento o si collega entro 15 s: "NESSUN DISPOSITIVO" |

Nei log ogni passaggio ha il tempo (`t=...` in ms).

### Disconnessione

Telecomando:

- invia `DISCONNECT` (firmato) a ogni dispositivo accoppiato e attende `DISCONNECT_ACK`, con
  ritrasmissione (finestra 400 ms: il dispositivo può essere in modem sleep); dopo al massimo 500 ms va
  avanti lo stesso (chi non ha confermato chiuderà la sessione allo scadere del suo timeout di sicurezza,
  come con il telecomando spento);
- **interrompe la comunicazione radio con i dispositivi**: niente più direzioni, heartbeat, riscoperta né
  accoppiamenti; cancella le chiavi di sessione;
- il **WiFi resta collegato al router**: OTA, API impostazioni e comandi seriali continuano a funzionare;
- il joystick continua a mostrare la direzione a schermo, ma non la invia a nessuno;
- in `devices` i dispositivi sono `OFFLINE` con il motivo "disconnesso (pressione lunga di SW)"; il comando
  `pair` è rifiutato finché non si riconnette con SW.

Dispositivi, alla ricezione di `DISCONNECT` (solo se la firma è valida):

- rispondono `DISCONNECT_ACK`;
- chiudono la sessione, tornano in modem sleep e **dimenticano l'accoppiamento** (MAC e chiave del
  telecomando): da qui in poi rifiutano i pacchetti di quel telecomando finché non si riaccoppia;
- tornano all'**esecuzione senza joystick**: l'effetto LED in corso **continua com'è**, e il dispositivo
  risponde solo ad API, seriale e comandi ritardati. Il joystick fisico resta disattivato
  (`remoteControlWireless = true`).

### Connessione

Nuova pressione lunga: **gli stessi passaggi dell'accensione**:

1. accoppiamento HTTP (`/api/v1/remote/pair`) direttamente con ogni dispositivo di `espDevices.json`
   (niente `getInfoHello`): nuovi nonce, **nuova chiave di sessione**;
2. heartbeat e sessioni come al solito; riparte la riscoperta periodica.

I pallini ripartono dagli stati dell'avvio (vuoto → mezzo pieno → pieno) man mano che i dispositivi
rispondono; lo slider "CONNESSIONE" resta a schermo finché il primo è collegato.

Lo stato "disconnesso" **non** viene salvato: dopo un riavvio il telecomando si connette come sempre.

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
| 7 | `DISCONNECT` | tel → disp | disconnessione (pressione lunga SW): chiudi la sessione, torna in modem sleep, dimentica l'accoppiamento, torna all'esecuzione senza joystick |
| 8 | `DISCONNECT_ACK` | disp → tel | `DISCONNECT` accettato |

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
- **Riscoperta**: il telecomando ritenta l'accoppiamento solo verso i dispositivi non accoppiati (es. accesi
  dopo il telecomando o riavviati) e con `devicePassword` configurata, con un'attesa per dispositivo: dopo un
  accoppiamento **senza risposta** `fastRetryS`, poi il doppio a ogni fallimento consecutivo (10, 20, 40 s...)
  fino a `rediscoveryS`; dopo un rifiuto (401, 403, ...) o una sessione persa senza risposte ESP-NOW,
  `rediscoveryS`. Un accoppiamento riuscito azzera l'attesa. Il comando `devices` mostra quando si riprova.
- **Riaccoppiamento senza risposta con la sessione ancora valida** (es. comando `pair` su una rete lenta): la
  sessione resta com'è (il dispositivo non ha ricevuto nulla e usa ancora quella chiave). Se la richiesta fosse
  arrivata e solo la risposta persa, il dispositivo avrebbe la chiave nuova: gli heartbeat ricevono
  `UNKNOWN_SESSION` e il telecomando si riaccoppia da solo.
- **Accoppiamento in un task a parte** (coda di dispositivi, uno alla volta): un dispositivo spento (fino a
  `pairAttempts` × `pairTimeoutS` di attesa HTTP) non blocca gli heartbeat verso gli altri.
- **Voce ARP statica**: prima di ogni accoppiamento il telecomando registra in lwIP la coppia `ip` → `macAdress`
  di `espDevices.json` (`espDevicesPrepareArp`), cosi' non deve chiedere il MAC in broadcast. Su un hotspot
  del telefono la richiesta ARP in broadcast non arrivava ai dispositivi in modem sleep: l'accoppiamento HTTP
  falliva sempre (nessuna risposta in 7 o 15 s) mentre l'ESP-NOW funzionava; con la voce statica
  l'accoppiamento riesce in 0.2-0.5 s. Per questo `macAdress` deve essere quello giusto (MAC della STA).
- **Tentativi brevi**: dopo il riavvio di un dispositivo il primo tentativo a volte resta bloccato anche se il
  dispositivo e' gia' raggiungibile, mentre una connessione nuova riesce subito: meglio piu' tentativi brevi
  (`pairAttempts` × `pairTimeoutS` = 4 × 5 s) che pochi lunghi.
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
| `rediscoveryS` | `60` | attesa massima tra due tentativi verso un dispositivo non collegato |
| `pairAttempts` | `4` | tentativi HTTP per dispositivo (il primo contatto dopo l'avvio spesso fallisce) |
| `pairTimeoutS` | `5` | attesa massima di ogni tentativo HTTP di accoppiamento (minimo 2) |
| `fastRetryS` | `10` | primo nuovo tentativo dopo un accoppiamento senza risposta, poi raddoppia fino a `rediscoveryS` |
| `longPressS` | `5` | secondi di pressione di SW per disconnettere / riconnettere i dispositivi (fase 5) |
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
| Pressione lunga di SW: annullamento del toggle BUTTON, disconnessione (`DISCONNECT` + attesa di `DISCONNECT_ACK`, chiavi cancellate, WiFi collegato), connessione come all'avvio, barra da 2 s, poi slider fino al risultato, conferma breve ("DISCONNESSO" / "CONNESSO" / "NESSUN DISPOSITIVO"), pallini vuoti e scritta "DISCONNESSO" | fatto; schermo cambiato il 2026-11-18, da riprovare |

### Progetto generale (`Esp32Server`)

| Cosa | Stato |
| --- | --- |
| `JoystickSettingsModel.remoteControlWireless` (to/fromJson, default `false`); joystick fisico disattivato | fatto |
| Endpoint `POST /api/v1/remote/pair` (verifica come `login`, nonce, chiave di sessione) | fatto |
| Ricevitore ESP-NOW: callback → coda → task (mittente, firma, `seq`) → `recvButtonByJoystick()` solo al cambio | fatto |
| `HEARTBEAT_ACK` firmato, `UNKNOWN_SESSION` (max 1/s) | fatto |
| `wirelessSessionTimeoutS`, sessioni con modem sleep a riposo (`WiFi.setSleep`), timeout di sicurezza | fatto e provato |
| `DISCONNECT`: `DISCONNECT_ACK`, chiude la sessione, dimentica l'accoppiamento, esecuzione senza joystick | fatto e provato |

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
5. **Disconnessione / connessione** con pressione lunga di SW (`DISCONNECT`/`DISCONNECT_ACK`, annullamento del
   toggle BUTTON, connessione come all'avvio, schermo). Provata il 2026-11-17 come "sospensione / ripresa":
   3 cicli; dalla pressione lunga: richiesta presa dal task wireless +36 ms, ACK del dispositivo +42 ms,
   messaggio a schermo **+45 ms**; poi riaccoppiamento con una nuova chiave e `collegato`; da disconnesso
   nessuna direzione arriva al dispositivo. Il 2026-11-18 rinominata in **disconnessione / connessione** (non è
   una pausa: i dispositivi dimenticano il telecomando) e aggiunto il riscontro "CONNESSIONE in corso..." →
   "CONNESSO" / "NESSUN DISPOSITIVO". Riprovata il 2026-11-18: 2 cicli; disconnessione → `DISCONNECT_ACK` +41 ms,
   "DISCONNESSI 1/1" a schermo **+47 ms**; connessione → accoppiato con nuova chiave, collegato, "CONNESSO
   MastroDevice" a schermo **+154–184 ms**; da disconnesso nessuna direzione arriva al dispositivo.
   Poi, su richiesta: slider animato al posto del messaggio "in corso" (stesso punto della barra, riscontro
   immediato), conferma breve (~1 s) senza nome del dispositivo, pallini **vuoti** da disconnessi (tolto lo
   stato `DISCONNECTED` / pallino grigio). **Da riprovare.**
   Telecomando: `Joystick::heldMs()` / `takeLongPress()`, `handleLongPress()` in `main.cpp`, stato del
   collegamento in `WirelessManager` (`LINK_STATE`: `CONNECTED` / `DISCONNECTING` / `DISCONNECTED`),
   schermo in `RemoteControlScreen` (barra, slider, conferma, scritta "DISCONNESSO").
   Dispositivo: `DISCONNECT` in `WirelessReceiver` (`DISCONNECT_ACK`, fine sessione, `remotePairingClear()`).

(La fase 6, misure di corrente e latenza, è stata tolta dal piano il 2026-11-18.)

## Come verificare

Servono i due monitor seriali: telecomando (USB del C6) e dispositivo (CH340), con **`"debug": true`** su
entrambi (`main_settings.json` del telecomando, `settings.json` del dispositivo): con `debug = false` i log
informativi qui sotto non escono (restano warning, errori e risposte ai comandi, come `devices`). Sul telecomando il comando
`devices` riassume tutto: stato e pallino di ogni dispositivo, statistiche ESP-NOW, sessione attiva o a
riposo, modem sleep, ultimo ACK e heartbeat persi.

| Situazione | Telecomando: cosa deve comparire | Dispositivo: cosa deve comparire |
| --- | --- | --- |
| Avvio | `[WirelessConfig] ...`, `[Wireless] <nome> accoppiato: ... impronta chiave XXXXXXXX`, poi `collegato (HEARTBEAT_ACK)`; pallino **pieno** | `[Wireless] ricevitore ESP-NOW attivo ... modem sleep a riposo`, `telecomando ... accoppiato ... impronta chiave XXXXXXXX` (**stessa impronta**) |
| Movimento del joystick | — (in `devices`: "sessione joystick attiva") | `inizio sessione: radio sempre accesa`, poi `Joystick direction: ...` a ogni cambio, mai due uguali di fila |
| 15 s fermo | `fine sessione (15 s senza movimenti)` | `fine sessione (SESSION_END dal telecomando): modem sleep` |
| A riposo | `devices`: `CONNECTED`, "ultimo ACK" sotto i 15 s circa, "invii affidabili scaduti" che non cresce | nessun messaggio |
| Primo movimento dopo il riposo | — | `inizio sessione` e la direzione applicata; ritardo percepito al massimo di qualche centinaio di ms, solo su questo movimento |
| Dispositivo riavviato | `sessione sconosciuta al dispositivo (riavviato?), riaccoppiamento`, poi `accoppiato` e `collegato` | `pacchetto scartato ... nessun telecomando accoppiato`, poi `telecomando ... accoppiato` |
| Telecomando spento durante una sessione | — | dopo 30 s: `fine sessione (nessun movimento per 30 s): modem sleep` |
| Dispositivo spento | dopo 3 heartbeat persi: `... heartbeat senza risposta, riaccoppiamento`, poi `non accoppiato: ...`; pallino **vuoto** | — |
| SW premuto da 2 a 5 s | in fondo allo schermo "DISCONNESSIONE" (o "CONNESSIONE") e la barra che si riempie; rilasciando prima resta solo il toggle BUTTON | — |
| SW premuto 5 s (disconnessione) | la barra diventa subito lo slider "DISCONNESSIONE"; `t=... pressione lunga di SW: disconnessione`, `DISCONNECT a N dispositivi`, `DISCONNECT_ACK da ...`, `dispositivi disconnessi: N/N hanno confermato`; conferma "DISCONNESSO" ~1 s, poi pallini **vuoti** e "DISCONNESSO" sotto la direzione; `devices`: "dispositivi DISCONNESSI", stato `OFFLINE` con motivo "disconnesso". Muovendo il joystick la direzione cambia a schermo ma non va ai dispositivi | eventuale direzione ripristinata (annullamento del toggle BUTTON), poi `fine sessione (DISCONNECT dal telecomando)` e `telecomando disconnesso: accoppiamento dimenticato`; i LED restano sull'effetto in corso |
| SW premuto 5 s di nuovo (connessione) | la barra diventa subito lo slider "CONNESSIONE" (resta finché il primo dispositivo è collegato); `pressione lunga di SW: connessione`, `connessione: accoppiamento di N dispositivi`, poi `accoppiato` (nuova impronta) e `collegato`; conferma "CONNESSO" ~1 s, pallini di nuovo pieni. Se nessuno risponde: "NESSUN DISPOSITIVO" | `telecomando ... accoppiato` con la nuova impronta |

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
  consegnati con poche ritrasmissioni) e al risveglio (primo movimento applicato, niente perso).
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
