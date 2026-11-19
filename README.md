# Esp32Server

Local Server for esp32 projects

To load settings rename .\data\settings\defaultSettings.json into .\data\settings\settings.json and modify

## Modalità file: memoria flash come unità di rete (WebDAV)

I file della memoria flash (LittleFS: `/settings/settings.json`, i preset, `/www`...) si possono aprire e
modificare da **Esplora risorse** via WiFi, senza rifare "Upload Filesystem Image":

1. dal monitor seriale scrivi `microsd` oppure `fileremote` (stesso comando del telecomando; qui la
   microSD non serve: viene esposta la flash);
2. la scheda si riavvia in modalità file: il **LED di stato resta acceso fisso** e la seriale stampa
   l'indirizzo, es. `http://mastrodevice.local/` (nome = `deviceName` in minuscolo, `_` → `-`) e l'IP;
3. in Esplora risorse: tasto destro su *Questo PC* → **Connetti unità di rete** → l'indirizzo stampato
   (se il nome non viene trovato al primo colpo riprova, oppure usa l'IP);
4. per uscire: `reboot` oppure stacca la corrente (la richiesta vale per un solo avvio).

In modalità file girano solo WiFi, server WebDAV (porta 80) e seriale (`reboot`, `info`, `info_ip`,
`help`): niente server web normale, effetti LED, OTA, joystick, wireless. Le modifiche ai JSON valgono
dal riavvio successivo.

Note:

- "Upload Filesystem Image" riscrive tutta la partizione con il contenuto di `data/`: se hai modificato
  file via WebDAV ricopiali prima in `data/`, altrimenti vanno persi;
- la partizione è piccola (128 KB) e la scrittura è lenta (pochi KB/s): va benissimo per i JSON;
- **non c'è password** (Windows rifiuta di default l'autenticazione semplice su HTTP non cifrato): finché
  la modalità è attiva i file, comprese le password in `settings.json`, sono leggibili da chi è sulla rete;
- serve il servizio Windows **WebClient** (vedi il README del telecomando, sezione "Modalità SD");
- il server WebDAV è `src/fileMode/FsWebDav.*`, stesso file di `addon_project/remote_control_esp32/src/storage/FsWebDav.*`
  (tenerli allineati).

RoadMap:

- 1.0.0:
  - [x] Base features
- 1.1.0:
  - [x] Settings model loaded by file (feature/1.1.0_setting_load_with_file)
  - [ ] Add settings for ledService into json file  (WIP)
  - [ ] Bluetooth access: Connection, Terminal and more  (WIP)
  - [ ] Set setting by SettingService to modify only or more value into Settings s
  - [ ] Settings modify with Serial usb, Serial Web and Serial bluetooth (To think)
  - [ ] Create a doc with protocol for communicate with serial.

Other features but not planning:

- Esp32 with more security protocol like https, ssl and certificate.
