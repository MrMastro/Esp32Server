# remote_control_esp32

Telecomando basato su **Waveshare ESP32-C6-LCD-1.3** (ESP32-C6, display 1.3" 240x240 ST7789V2, LED RGB WS2812B, slot microSD, 4MB flash).

Progetto PlatformIO (piattaforma pioarduino, Arduino core 3.x).

## Build e upload

- Compilare da **PowerShell** o dai pulsanti PlatformIO di VS Code, **non da Git Bash**: lo script `idf_tools.py` di pioarduino rifiuta gli ambienti MSYS/MinGW e la toolchain non viene installata.
- Tabella partizioni `min_spiffs.csv`: due slot app da 1,9MB per l'OTA, filesystem in flash minimo (le configurazioni sono sulla SD).
  Il firmware deve restare sotto 1,9MB (controllare la riga `Flash:` del build).
- Upload via USB-C. Se la porta non viene vista: tenere premuto BOOT, premere RST, rilasciare BOOT.

## OTA (upload via WiFi)

1. In `main_settings.json` impostare `"ota_upload": true` (oppure via API, vedi sotto) e riavviare.
   Lo schermo mostra `OTA ON` accanto all'IP.
2. Copiare `ota_local.ini.example` in `ota_local.ini` (non versionato): una sezione `[ota_<nome>]` per ogni
   dispositivo, con `ip` e `password` (= `devicePassword` del suo `main_settings.json`).
3. Scegliere il dispositivo con `target` nella sezione `[ota]`:

   ```ini
   [ota]
   target = esp32_2          ; usa la sezione [ota_esp32_2]

   [ota_esp32_1]
   ip = 172.16.250.100
   password = ...

   [ota_esp32_2]
   ip = 0.0.0.0              ; 0.0.0.0 = non ancora noto: l'upload si ferma con un errore
   password = ...
   ```

   oppure al volo senza toccare il file (PowerShell): `$env:OTA_TARGET = "esp32_2"`.
4. Caricare con l'ambiente OTA: `pio run -e esp32-c6-lcd-1_3-ota -t upload`
   (o dalla barra di PlatformIO scegliendo l'ambiente `esp32-c6-lcd-1_3-ota`).
   Il log mostra `[OTA] destinazione: <nome> (<ip>)`. La scelta la fa `scripts/ota_select.py`
   (PlatformIO non supporta variabili annidate tipo `${ota_${ota.target}.ip}`).

Un dispositivo nuovo va caricato la prima volta via USB (`pio run -e esp32-c6-lcd-1_3 -t upload`): l'OTA
funziona solo da quando il firmware è sulla scheda e `ota_upload` è `true` nel suo `main_settings.json`.

Il PC deve essere sulla stessa rete della scheda (in AP_MODE: collegarsi al WiFi `ssidAP`).
Ogni firmware caricato deve contenere l'OTA, altrimenti si torna all'upload via USB.

## API impostazioni

`POST http://<ip>/api/settings` con corpo JSON: `deviceName` e `devicePassword` (autenticazione)
più **una sola** chiave di `main_settings.json`. Se la richiesta è valida la chiave viene salvata
sulla SD, lo schermo mostra la chiave modificata e la scheda si riavvia dopo 5 secondi.

```json
{ "deviceName": "ESP32_Remote", "devicePassword": "esp32password", "ota_upload": true }
```

Esempio da PowerShell:

```powershell
Invoke-RestMethod -Method Post -Uri http://192.168.4.1/api/settings -ContentType "application/json" `
  -Body '{"deviceName":"ESP32_Remote","devicePassword":"esp32password","communicationMode":"WIFI_MODE"}'
```

Chiavi modificabili e validazione:

| Chiave | Valori ammessi |
| --- | --- |
| `ota_upload`, `debug` | `true` / `false` |
| `mode` | `remote_control` |
| `communicationMode` | `AP_MODE`, `WIFI_MODE` |
| `ssidAP`, `ssidWIFI` | stringa da 1 a 32 caratteri |
| `passwordAP`, `passwordWIFI` | vuota (rete aperta) o da 8 a 63 caratteri |

`deviceName` e `devicePassword` non sono modificabili via API (servono per l'autenticazione).

Risposte: `200` salvato · `400` richiesta non valida · `401` credenziali errate · `409` riavvio già in corso ·
`503` SD assente · `500` scrittura fallita. Corpo: `{"status": "ok"|"error", "message": "..."}`.

Nota: la comunicazione è in HTTP non cifrato, la password viaggia in chiaro sulla rete.

## Pin della scheda

### Occupati dall'hardware di bordo (non usare)

| GPIO | Uso |
| --- | --- |
| 4 | microSD CS |
| 5 | SPI MISO (microSD) |
| 6 | SPI MOSI (display + microSD) |
| 7 | SPI SCLK (display + microSD) |
| 8 | LED RGB WS2812B |
| 9 | Pulsante BOOT |
| 12, 13 | USB D-/D+ (seriale e upload) |
| 14 | LCD CS |
| 15 | LCD DC |
| 21 | LCD RST |
| 22 | LCD retroilluminazione |

Il display e la microSD condividono lo stesso bus SPI (GPIO 5/6/7) con CS separati.

### Connettore esterno a 9 pin

GPIO esposti (letti sulla serigrafia della scheda): **1, 2, 3, 12, 13, 16, 17, 19 (?), 23**

- 12, 13: USB, usarli disabilita la seriale USB e l'upload.
- 16, 17: UART0 TX/RX.
- Il pin letto come "29" è da verificare (sull'ESP32-C6 i GPIO 24-30 sono riservati alla flash: probabilmente è 19 o 20).
- ADC disponibile solo su GPIO 0-6 → sul connettore: 1, 2, 3.

## Joystick analogico

| Joystick | Scheda |
| --- | --- |
| SW | **GPIO 1** (digitale, `INPUT_PULLUP`) |
| VRy | **GPIO 2** (ADC) |
| VRx | **GPIO 3** (ADC) |
| VCC | **3V3** (non 5V: l'ADC accetta al massimo ~3.3V) |
| GND | GND |

ADC a 12 bit (0-4095), centro tipico ~1930/1960, zona morta 400.

Mappatura assi ereditata dal progetto principale (`Esp32Server/src/Main.cpp`):

- asse X → UP / DOWN
- asse Y → RIGHT / LEFT
- il pulsante SW fa da **toggle**: quando è attivo, al comando si aggiunge il suffisso `_BUTTON`.

## microSD

Formattare in **FAT32** da Windows (preferibilmente schede ≤ 32GB). exFAT non è garantito.

Le configurazioni JSON stanno nella cartella **`/config`** della microSD (es. `/config/joystick.json`).
Non si usa la cartella `data/` di PlatformIO né il filesystem in flash (LittleFS/SPIFFS): per cambiare
una configurazione basta modificare il file sulla SD dal PC, senza ricaricare nulla sulla scheda.

Il contenuto da copiare nella root della SD è in [`sd_card/`](sd_card/) (quindi `sd_card/config/joystick.json` → `/config/joystick.json`).

In git sono versionati solo `default_main_settings.json` (modello senza dati reali), `joystick.json`,
`wireless.json` e `espDevices.json.example` (modello senza dati reali).
Tutti gli altri file in `sd_card/config/` sono ignorati perché contengono dati sensibili e cambiano da PC a PC.
Il firmware legge sempre **`/config/main_settings.json`**: copiare `default_main_settings.json` in
`main_settings.json`, inserire le proprie credenziali e mettere quello sulla SD.
Se la SD o il file mancano, il firmware usa i default (pin 1/2/3, calibrazione automatica, zona morta 400).

### /config/main_settings.json

Impostazioni generali (stessi nomi di `SettingsModel` del progetto principale, solo i campi utili).
Se la SD manca, il file non esiste o il JSON non è valido si usano i default di `SettingsModel::getDefault()`;
i singoli campi mancanti nel file prendono il valore di default.

| Campo | Default | Significato |
| --- | --- | --- |
| `mode` | `remote_control` | Funzione del dispositivo (per ora solo `remote_control`) |
| `deviceName` | `ESP32_Device` | Hostname in rete (e nome per l'OTA) |
| `devicePassword` | `esp32password` | Password per l'OTA e per l'API delle impostazioni |
| `ota_upload` | `false` | `true` = OTA via WiFi attivo (upload da PlatformIO) |
| `communicationMode` | `AP_MODE` | `AP_MODE` o `WIFI_MODE` |
| `debug` | `false` | `true`: log informativi sulla seriale (avvio, WiFi, joystick wireless, tempi `t=...`). `false`: escono solo warning, errori, le risposte ai comandi seriali e la configurazione all'avvio, e il telecomando è più reattivo (come `debug` nel progetto principale) |
| `ssidAP` / `passwordAP` | `ESP32_AP` / `ap_password` | Access point creato dalla scheda (password ≥ 8 caratteri) |
| `ssidWIFI` / `passwordWIFI` | `Home_Network` / `wifi_password` | Rete WiFi a cui collegarsi (solo 2,4GHz) |

Comportamento della rete (come `MastroServer` del progetto principale):

- `AP_MODE`: la scheda crea l'access point `ssidAP` (IP `192.168.4.1`).
- `WIFI_MODE`: si collega a `ssidWIFI`; se entro 10 secondi non ci riesce, passa ad AP.
- valore sconosciuto: AP.

`BLUETOOTH_MODE` non è supportato: l'ESP32-C6 ha solo Bluetooth LE, non il Bluetooth classico.

### /config/joystick.json

| Campo | Default | Significato |
| --- | --- | --- |
| `enableJoystick` | `true` | Abilita il joystick |
| `pinSwitch` / `pinAnalogX` / `pinAnalogY` | `1` / `3` / `2` | GPIO di SW, VRx, VRy |
| `autoCalibrate` | `true` | Misura il centro all'avvio (non toccare la levetta durante il boot) |
| `centerX` / `centerY` | `1930` / `1960` | Centro usato se `autoCalibrate` è `false` |
| `deadZone` | `400` | Scostamento dal centro sotto il quale l'asse è centrato |

### /config/espDevices.json

Lista dei dispositivi conosciuti (ESP32 del progetto principale), compilata a mano: al massimo **10** (gli altri vengono ignorati). Può essere vuota (`[]`); se il file manca la lista è vuota. Non è versionato (contiene IP, MAC e password della rete di casa): il modello è `sd_card/config/espDevices.json.example`, da copiare in `espDevices.json` e personalizzare.

```json
[
  { "deviceName": "MastroDevice", "ip": "172.17.252.192", "macAdress": "14:33:5C:02:7B:A0", "color": "#52A3BA",
    "devicePassword": "password_del_dispositivo" }
]
```

| Campo | Significato |
| --- | --- |
| `deviceName` | `deviceName` del `settings.json` del dispositivo: **deve coincidere** (serve per l'accoppiamento) |
| `ip` | Indirizzo del dispositivo (obbligatorio) |
| `macAdress` | MAC atteso nella risposta: identifica il dispositivo (obbligatorio, maiuscole/minuscole indifferenti) |
| `color` | Colore del pallino a schermo, `#RRGGBB` (se manca o non è valido: bianco) |
| `devicePassword` | `devicePassword` del `settings.json` del dispositivo (se manca il dispositivo non viene contattato) |

I dispositivi vengono contattati **all'avvio, come ultima cosa**, in un task in background (lo schermo resta attivo), direttamente con l'accoppiamento del joystick wireless (`POST http://<ip>/api/v1/remote/pair`, vedi [docs/WIRELESS_JOYSTICK.md](docs/WIRELESS_JOYSTICK.md)): un dispositivo è **online** se risponde, **accoppiato** se accetta `deviceName` e `devicePassword` e il MAC coincide. Chi non risponde viene ritentato una volta (timeout 7 s per tentativo), perché il primo contatto dopo l'avvio spesso fallisce. L'esito è nel log seriale (`[Devices] ...`, `[Wireless] ...`) e nel comando `devices`.

A schermo, sotto l'intestazione, c'è un pallino per dispositivo nel suo `color`: **vuoto** = offline, **mezzo pieno** = online, **pieno** = collegato (da fare).

Nota: il modem sleep (risparmio energetico WiFi) del telecomando viene spento durante l'accoppiamento HTTP: con il modem sleep attivo su entrambi gli ESP32 il primo contatto diretto tra loro falliva. A riposo è attivo (vedi `remoteModemSleep` in `wireless.json`).

### /config/wireless.json

Parametri del joystick wireless (heartbeat, sessioni, riscoperta, modem sleep): tabella completa in
[docs/WIRELESS_JOYSTICK.md](docs/WIRELESS_JOYSTICK.md#configurazioni). Se il file manca si usano i default.

## Modalità SD: microSD come unità di rete (WebDAV)

L'ESP32-C6 non ha USB OTG, quindi non può comparire come chiavetta USB. In alternativa la microSD si
può aprire da **Esplora risorse via WiFi** (WebDAV):

1. dal monitor seriale scrivi `microsd` o `fileremote` (se la microSD non c'è il comando viene ignorato);
2. la scheda si riavvia in modalità SD e lo schermo mostra l'indirizzo con il nome del dispositivo,
   es. `http://esp32-remote.local/` (mDNS), più l'IP come riserva e la scritta "Connetti unita' di rete".
   Il nome deriva da `deviceName` di `main_settings.json`: minuscolo, con `_`, spazi e punti sostituiti
   da `-` (negli indirizzi di rete il `_` non è valido), es. `ESP32_Remote` → `esp32-remote.local`;
3. in Esplora risorse: tasto destro su *Questo PC* → **Connetti unità di rete** → cartella
   `http://esp32-remote.local/` (se il nome non viene trovato usa l'IP, es. `http://172.17.252.107/`) (oppure scrivi `\\172.17.252.107\DavWWWRoot` nella barra degli indirizzi);
4. per uscire: `reboot` dal monitor seriale, oppure stacca la corrente. La richiesta vale per un solo
   avvio (flag in NVS cancellato all'avvio), quindi ogni riavvio successivo torna alla modalità normale.

In modalità SD girano solo rete, server WebDAV (porta 80, radice = radice della microSD) e seriale
(`reboot`, `info`, `info_ip`, `help`); niente joystick, wireless, OTA, API impostazioni. Il tasto BOOT
spegne/accende lo schermo come di consueto. Velocità misurata: ~280 KB/s in scrittura, ~235 KB/s in lettura.

Note per Windows:

- serve il servizio **WebClient** (Client Web): se "Connetti unità di rete" dice che la cartella non è
  valida, avvialo da *Servizi* (`services.msc` → WebClient → Avvia; tipo di avvio *Automatico* per non
  doverlo rifare). Da PowerShell come amministratore: `Start-Service WebClient`;
- **non c'è password**: Windows rifiuta di default l'autenticazione semplice su HTTP non cifrato. Finché
  la modalità SD è attiva tutta la microSD (anche i file con le password) è leggibile da chi è sulla
  stessa rete: per questo dura solo fino al riavvio.

## Collegamento con i dispositivi (joystick wireless)

Il collegamento continuo telecomando → dispositivi (ESP-NOW, accoppiamento, heartbeat, sessioni per il
risparmio energetico, `remoteControlWireless` nel progetto generale) è descritto in
[docs/WIRELESS_JOYSTICK.md](docs/WIRELESS_JOYSTICK.md), con lo stato di ogni fase e come verificare che funzioni.

## Comandi seriali

I caratteri si scrivono in sequenza (monitor in modalità raw, come il progetto principale): il comando parte dopo 1 secondo senza nuovi caratteri, non serve Invio. Parametri separati da virgola.

| Comando | Effetto |
| --- | --- |
| `reboot[,ms]` (alias `reset`) | Riavvia subito, o dopo `ms` millisecondi |
| `change_communication_mode,<AP_MODE\|WIFI_MODE>` | Salva la modalità su SD e riavvia |
| `info` / `info_ip` | Stato del dispositivo / indirizzo IP |
| `settings` | Ristampa la configurazione caricata |
| `hello,<ip>` | Diagnosi: `GET http://<ip>/api/v1/getInfoHello` verso un IP qualsiasi |
| `devices` | Stato dei dispositivi (online, accoppiati, motivo), statistiche ESP-NOW e sessioni wireless con l'impronta della chiave |
| `pair` | Riaccoppia tutti i dispositivi (in background) |
| `prod` | `debug = false` in `main_settings.json` e riavvio: escono solo warning, errori, le risposte ai comandi e la configurazione all'avvio |
| `debug` | `debug = true` in `main_settings.json` e riavvio: tutti i log |
| `microsd` (alias `fileremote`) | Riavvia in **modalità SD**: microSD come unità di rete (WebDAV). Ignorato se la microSD non c'è |
| `help` | Lista dei comandi |
| `t` | `test()`: funzione di prova libera |

## Struttura del codice

| File | Contenuto |
| --- | --- |
| `src/main.cpp` | Avvio (display → SD → config → joystick) e `loop()` |
| `src/BoardPins.h` | Pin della scheda e default del joystick |
| `src/display/` | Display ST7789 (Arduino_GFX) sul bus SPI condiviso |
| `src/storage/` | Montaggio microSD e percorsi in `/config`; `FsWebDav`: server WebDAV della modalità SD (stesso file del progetto generale) |
| `src/settings/` | `SettingsModel` (main_settings.json), `COMMUNICATION_MODE`, `DEVICE_MODE` |
| `src/network/` | Avvio della rete in AP_MODE / WIFI_MODE, OTA (ArduinoOTA) |
| `src/settings/SettingsValidator.*` | Validazione delle richieste all'API impostazioni |
| `src/joystick/` | Algoritmo del joystick, configurazione JSON, enum `J_DIRECTION` (copia di quello del progetto principale) |
| `src/devices/` | Dispositivi conosciuti (`espDevices.json`), stato online/offline, client HTTP minimo |
| `src/wireless/` | Joystick wireless: protocollo condiviso (`WirelessProtocol.h`), accoppiamento, ESP-NOW, heartbeat (`WirelessManager`), `wireless.json` |
| `src/test/` | Prove: schermata joystick |
| `src/system/BootMode.*` | Richiesta "una tantum" della modalità SD (NVS) |
| `src/system/` | `take/freeExclusiveExecution()`: mutex tra i task FreeRTOS (display e SD condividono il bus SPI) |
| `src/test/` | `testJoystickOnDisplay()`: mostra a schermo la stringa `J_DIRECTION` e i valori grezzi |

Il test si attiva/disattiva commentando la riga `testJoystickOnDisplay(joystick);` in `loop()`.
