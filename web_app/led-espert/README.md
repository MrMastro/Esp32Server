# Capacitor + Vue + TypeScript Template

Questo progetto è un template di base per una app Capacitor con Vue 3, Vite e TypeScript.
Usalo come punto di partenza e sostituisci i contenuti di esempio con la logica della tua app.

## Struttura del progetto

- `src/`
  - `App.vue` - componente principale con un bottone "Avvia".
  - `main.ts` - entrypoint dell'app Vue.
  - `shims-vue.d.ts` - dichiarazioni per i file `.vue` in TypeScript.
- `index.html` - pagina HTML principale.
- `vite.config.ts` - configurazione Vite per Vue.
- `tsconfig.json` - configurazione TypeScript.
- `package.json` - script e dipendenze.

## Installazione

1. Apri un terminale nella cartella del progetto:
   ```bash
   cd /percorso/del/progetto
   ```
2. Installa le dipendenze:
   ```bash
   npm install
   ```

## Esecuzione in sviluppo (locale)

Per avviare l'app in locale:

```bash
npm run dev
```

Apri il browser sull'URL mostrato da Vite (di solito `http://localhost:5173`).

Se vuoi vedere la versione buildata in locale dopo la compilazione, esegui:

```bash
npm run preview
```

## Build di produzione

Crea la build ottimizzata per la distribuzione con:

```bash
npm run build
```

Questo comando esegue la compilazione TypeScript e l'ottimizzazione dei file web.

Dopo la build, i file generati si trovano nella cartella `dist/`.

## Debug

### Debug su PC (browser)

- Avvia l'app in locale:
  ```bash
  npm run dev
  ```
- Apri Chrome su `http://localhost:5173`.
- Apri Chrome DevTools con `F12` o `Ctrl+Shift+I`.
- Usa la scheda `Console` per errori JavaScript e warning Vue.
- Usa la scheda `Sources` per impostare breakpoint, ispezionare variabili e navigare nel codice TypeScript/Vue.
- Usa la modalità dispositivo (`Ctrl+Shift+M`) per simulare un device mobile Android e cambiare risoluzione, DPR, orientamento e tocco.

### Debug con dispositivo Android collegato (ADB)

- Assicurati che il dispositivo sia in modalità sviluppatore e con il debug USB attivo.
- Collega il dispositivo al PC via USB.
- Verifica che ADB lo riconosca:
  ```bash
  adb devices
  ```
- Copia i file web nel progetto Android:
  ```bash
  npm run cap:copy
  ```
- Apri il progetto Android:
  ```bash
  npm run cap:open:android
  ```
- Avvia l'app dal dispositivo o da Android Studio.
- Apri Chrome su PC e vai a `chrome://inspect`.
- Nella sezione `Remote Target`, trova il dispositivo Android e la WebView corrispondente all'app Capacitor.
- Clicca `Inspect` per aprire DevTools remoti sulla WebView e fare debugging della UI nativa.
- Se non vedi la WebView, assicurati che l'app sia avviata sul dispositivo e che il dispositivo sia autorizzato per il debug USB.

- Puoi usare anche `chrome://inspect/#devices` per gestire la connessione e aggiornare la lista dei target.

### Debug con VS Code

- Collega il dispositivo Android e verifica che ADB lo riconosca:
  ```bash
  adb devices
  ```
- Inoltra il debug remoto della WebView su una porta locale:
  ```bash
  adb forward tcp:9222 localabstract:chrome_devtools_remote
  ```
- Apri VS Code nella cartella del progetto.
- Crea o aggiorna `.vscode/launch.json` con questa configurazione:
  ```json
  {
    "version": "0.2.0",
    "configurations": [
      {
        "name": "Attach to Android WebView",
        "type": "pwa-chrome",
        "request": "attach",
        "port": 9222,
        "webRoot": "${workspaceFolder}"
      }
    ]
  }
  ```
- Avvia l'app sul dispositivo e poi avvia il debug in VS Code usando la configurazione `Attach to Android WebView`.
- Usa il pannello `Debugger` in VS Code per impostare breakpoint, esplorare le variabili e navigare il codice Vue/TypeScript.
- Se non funziona, prova prima con `chrome://inspect` per verificare la connessione ADB e la presenza della WebView.

## Uso di Capacitor

Il progetto è già predisposto per Capacitor. I comandi utili sono:

- Aggiungi Android:
  ```bash
  npm run cap:add:android
  ```
- Aggiungi iOS:
  ```bash
  npm run cap:add:ios
  ```
- Copia i file web nella piattaforma:
  ```bash
  npm run cap:copy
  ```
- Apri il progetto Android:
  ```bash
  npm run cap:open:android
  ```
- Apri il progetto iOS:
  ```bash
  npm run cap:open:ios
  ```

> Nota: per iOS è necessario usare un Mac con Xcode installato.

## Come funziona il template

Il template mostra una schermata di esempio con:

- un titolo generico
- un paragrafo descrittivo
- un bottone di esempio

Quando si preme il bottone, viene mostrato un messaggio di test.

Tutta la logica di base si trova in `src/App.vue` e usa Vue 3 con la Composition API (`ref`).

## Personalizzare per la tua app

Per trasformare questo template in un’app vera e propria, sostituisci i contenuti di esempio con quelli specifici della tua app:

- `src/App.vue`: qui puoi sostituire il layout e la logica di esempio con le tue schermate principali, componenti di presentazione e callback reali.
- `src/main.ts`: importa e registra eventuali plugin, router, store o provider della tua applicazione.
- `index.html`: modifica titolo, descrizione, logo e meta tag con il nome e le informazioni della tua app.
- `package.json`: aggiorna `name`, `version`, `description` e aggiungi dipendenze specifiche (ad esempio router, store, plugin Capacitor o librerie UI).
- `vite.config.ts`: personalizza la configurazione Vite solo se hai bisogno di alias, plugin aggiuntivi o build speciali.
- `tsconfig.json`: estendi le impostazioni TypeScript se la tua app richiede tipizzazione aggiuntiva o moduli specifici.

### Esempio di trasformazione

- Se la tua app ha una home e una pagina impostazioni, crea in `src/` una cartella `views/` o `views/` e sposta lì i componenti.
- Se usi Vue Router, installa `vue-router`, crea `src/router.ts` e registra il router in `src/main.ts`.
- Se hai bisogno di stato globale, aggiungi Pinia o Vuex e configura lo store in `src/main.ts`.
- Se devi usare API native di Capacitor, importa i plugin Capacitor in `src/App.vue` o nei componenti specifici.

### Cosa lasciare come template

- La struttura base del progetto (`src/`, `index.html`, `package.json`, `tsconfig.json`, `vite.config.ts`) va bene come punto di partenza.
- La logica `button -> message` è solo un esempio: eliminane o sostituiscila con le azioni reali della tua applicazione.

Aggiungi componenti, pagine, router, store o plugin secondo le tue esigenze. Una volta sostituiti i contenuti di esempio, il progetto sarà pronto per diventare un’app precisa.

## Dettagli tecnici

- Vue 3 con `@vitejs/plugin-vue`
- TypeScript per l'entrypoint e le dichiarazioni di tipo
- Capacitor per creare app native ibride

Se vuoi posso anche aggiungere le configurazioni di `capacitor.config.ts` e i file delle piattaforme Android/iOS. 