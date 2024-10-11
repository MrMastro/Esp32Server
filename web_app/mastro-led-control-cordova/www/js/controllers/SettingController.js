import SettingService from '../services/SettingServices.js';
import LoginView from '../views/LoginView.js';
import SettingView from '../views/SettingView.js';
import WaitView from '../views/WaitView.js';
import AlertMessageView from '../views/AlertMessageView.js';

export default class SettingController {
    constructor(host) {
        this.settingService = new SettingService();
        this.settingView = new SettingView();
        this.loginView = new LoginView(document.getElementById('LoginViewContainer'));
        this.waitView = new WaitView(document.getElementById('WaitViewContainer'));
        this.alertMessageView = new AlertMessageView(document.getElementById('AlertMessageViewContainer'));
        this.referenceHost = host;
        this.init();
    }

    async init() {

        this.loginView.render();
        this.waitView.render();
        // Event Listener per il salvataggio delle impostazioni
        //? si mettono nelle view o nel controller?
        //? se la view non è completamente HTML si dichiara l'ascoltatore nella view e si usa il metodo della view


        requestAnimationFrame(() => {
            document.querySelector('.saveSettingsBtn').addEventListener('click', () => {
                this.saveSettings();
            });

            //! in questo caso non ho la view quindi metto tutto qui
            this.loginView.bindBtnLogin(this.login.bind(this));

            //!qui invece avendoci la loginView faccio la bind, come gia scritto
            //document.getElementById('Login-Modal').on('click')
            $('.settingsBtn').on('click', () => this.showModal());
        });
    }

    async saveSettings() {
        
        //Gestione colori
        const colorHex = document.querySelector('#initialRgbInput').value;

        // Rimuovi il carattere "#" se presente
        let hex = colorHex.replace(/^#/, '');

        // Converte l'esadecimale in valori RGB
        const bigint = parseInt(hex, 16);
        const r = (bigint >> 16) & 255; // Estrae il valore rosso
        const g = (bigint >> 8) & 255;  // Estrae il valore verde
        const b = bigint & 255;         // Estrae il valore blu

        const settings = {
            deviceName: document.querySelector('#deviceNameInput').value,
            devicePassword: document.querySelector('#devicePasswordInput').value,
            communicationMode: document.querySelector('#communicationModeInput').value,
            debug: document.querySelector('#debugInput').checked,
            ssidAP: document.querySelector('#ssidAPInput').value,
            passwordAP: document.querySelector('#passwordAPInput').value,
            ssidWIFI: document.querySelector('#ssidWIFIInput').value,
            passwordWIFI: document.querySelector('#passwordWIFIInput').value,
            initialEffect: document.querySelector('#initialEffectInput').value,
            initialDeltaT: document.querySelector('#initialDeltaTInput').innerText,
            initialR: r,
            initialG: g,
            initialB: b,
            ledSettings: {
                enableStripRgb: document.querySelector('#enableStripRgbInput').checked,
                pinLedCinRgb: document.querySelector('#pinLedCinRgbInput').value,
                pinLedDinRgb: document.querySelector('#pinLedDinRgbInput').value,
                enableStripWs2811: document.querySelector('#enableStripWs2811Input').checked,
                numLedWs2811: document.querySelector('#numLedWs2811Input').value,
                pinLedWs2811: document.querySelector('#pinLedWs2811Input').value
            }
        };

        const success = await this.settingService.saveDeviceSettings(this.referenceHost, settings);
        if (success) {
            console.log("Impostazioni salvate con successo");
            this.settingView.hide();
            this.alertMessageView.alert("Successo", "Impostazioni salvate, il dispositivo sarà riavviato");
        } else {
            console.error("Errore nel salvataggio delle impostazioni");
            this.alertMessageView.alert("Errore", "Si è verificato un problema durante il salvataggio delle impostazioni");
        }
    }


    async showModal() {
        if (this.settingService.isLogged()) {
            this.waitView.show();
            let settings = await this.settingService.getSettings(this.referenceHost);
            this.waitView.hide();
            this.settingView.render(settings);
            this.settingView.show();
        } else {
            this.loginView.show();
        }
    }

    async login() {
        this.loginView.hide();
        this.waitView.show();
        let result = await this.settingService.login("name", "password");
        this.waitView.hide();
        if (result) {
            this.waitView.show();
            let settings = await this.settingService.getSettings(this.referenceHost);
            this.waitView.hide();
            this.settingView.render(settings);
            this.settingView.show();
        } else {
            alert("Credenziali non valide");
        }

    }

    async setReferenceHost(newHost){
        this.referenceHost = newHost;
    }

}