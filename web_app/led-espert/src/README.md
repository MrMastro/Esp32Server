# Conversione Vue 3 di WebServerEsp32

Questo archivio contiene la conversione in componenti Vue 3 (Composition API,
`<script setup>`) dei file originali:

| Originale (vanilla JS) | Convertito in |
|---|---|
| `index.html` | `EspControlPage.vue` |
| `HeaderView.js` | `views/HeaderView.vue` |
| `FooterView.js` | `views/FooterView.vue` |
| `Esp32ConnectionView.js` | `views/Esp32ConnectionView.vue` |
| `MainView.js` | `views/MainView.vue` |
| `GroupColorView.js` | `views/GroupColorView.vue` |
| `SettingView.js` | `views/SettingsView.vue` |
| `LoginView.js` | `views/LoginView.vue` |
| `WaitView.js` | `views/WaitView.vue` |
| `AlertMessageView.js` | `views/AlertMessageView.vue` |

## Cosa manca (non era tra i file caricati)

Questi file venivano importati dagli originali ma non sono mai stati
caricati, quindi **non esistono ancora** in questo progetto. I componenti
Vue li importano comunque con lo stesso percorso relativo originale,
assumendo che tu li aggiunga così com'erano (sono moduli JS puri, non
serve convertirli in Vue):

- `models/LedMainModel.js`, `models/SettingModel.js`, `models/Esp32Model.js`,
  `models/request/LedColorRequest.js`
- `utils/ColorUtils.js`, `utils/TextUtils.js`, `utils/TimeUtils.js`
- `constants/DefaultConstants.js`
- Il **controller** applicativo (nell'originale coordinava le chiamate
  HTTP verso l'ESP32, il login, il salvataggio impostazioni, ecc.): questa
  logica non era in nessuno dei file forniti. In `EspControlPage.vue` ho
  lasciato dei `// TODO` esattamente nei punti dove andrebbe agganciata.

## Cambiamenti di comportamento rispetto all'originale

- **jQuery rimosso**: `LoginView.js` e `SettingView.js` usavano
  `$(this.modal).modal('show'/'hide')`, mentre `WaitView.js` e
  `AlertMessageView.js` usavano già l'API nativa `new bootstrap.Modal(...)`.
  Ho uniformato tutto all'API nativa di Bootstrap: se il progetto Vue
  include ancora jQuery altrove, questi due componenti non ne hanno più
  bisogno.
- **Modali pilotate via `ref` + metodi esposti** (`show()`, `hide()`,
  `alert()`) invece che tramite `v-if`/`v-model`, per restare fedeli al
  fatto che l'originale le apriva/chiudeva imperativamente da un
  controller esterno. `EspControlPage.vue` mostra come richiamarle.
- **`HeaderView.js`**: il link "Seriale" e "Aggiorna Esp32" avevano
  l'attributo non standard `hrefcommented` invece di `href` (quindi di
  fatto non erano cliccabili nell'originale). Li ho resi funzionanti con
  un vero `href`: verifica che gli URL (`/webConsole`, `updateLogin`)
  siano ancora corretti nel tuo routing.
- **`SettingView.js`**: le sezioni "Impostazioni Connessione AP" e
  "Impostazioni Connessione WIFI" erano *entrambe sempre visibili* nel
  markup originale, indipendentemente dal valore selezionato in
  "Tipologia di comunicazione". Le ho mantenute così per fedeltà, ma è
  un candidato naturale per un `v-if` su `form.communicationMode`.
- **`MainView.js`**: conteneva riferimenti a un campo IP e a una checkbox
  "Connessione AP" (`getFieldIp()`, `isAPconnection()`) il cui markup però
  non compare nel `render()` di quella stessa classe — esiste solo,
  commentato, in fondo al file. Sembra codice morto o un residuo di
  refactor in cui quella responsabilità è passata a
  `Esp32ConnectionView`. Non l'ho riportato per evitare di duplicare la
  gestione dell'IP in due componenti diversi.
- **`GroupColorView.js`**: il numero massimo di input colore era
  hard-coded a 5 (`createColorsIntoPanel(root, 5)`); mantenuto identico
  come costante `MAX_SLOTS`.

## Cosa va sistemato a livello di progetto (non nel singolo componente)

- I `<meta>`, la favicon, `manifest.json` e i fogli di stile globali
  (Bootstrap, Montserrat, `character.css`, `myCss.css`, ecc.) vanno
  nell'`index.html` root del progetto Vue (Vite) o importati come CSS
  globale in `main.js`.
- `cordova.js` resta nell'involucro Cordova, se il progetto è ancora
  ibrido; `jquery.min.js` non serve più ai componenti convertiti qui.
- `bootstrap.min.js` va sostituito con `import { Modal } from 'bootstrap'`
  (già usato nei componenti) — assicurati che il pacchetto `bootstrap`
  sia tra le dipendenze npm.
