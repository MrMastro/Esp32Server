export default class SettingsModel {
    constructor() {
        // Inizializza i valori del dispositivo
        this.deviceName = "xxx (effettua login)";
        this.devicePassword = "esp32passwordGeneric";
        this.communicationMode = "AP_MODE";
        this.debug = true;
        this.initialEffect = "NO_EFFECT";
        this.initialDeltaT = 0;
        this.initialR = 0;
        this.initialG = 0;
        this.initialB = 255;
        this.ssidAP = "ESP32_AP";
        this.passwordAP = "ap_passwordGeneric";
        this.ssidWIFI = "Home_Network";
        this.passwordWIFI = "wifi_passwordGeneric";
        this.ledSettings = {
            enableStripWs2811: true,
            numLedWs2811: 32,
            pinLedWs2811: 5,
            enableStripRgb: true,
            pinLedDinRgb: 19,
            pinLedCinRgb: 18
        };
    }

    // Metodo per aggiornare le impostazioni del dispositivo
    updateSettings(newSettings) {
        Object.assign(this, newSettings);
    }

    // Metodo per ottenere la configurazione corrente
    getSettings() {
        return {
            deviceName: this.deviceName,
            devicePassword: this.devicePassword,
            communicationMode: this.communicationMode,
            debug: this.debug,
            initialEffect: this.initialEffect,
            initialDeltaT: this.initialDeltaT,
            initialR: this.initialR,
            initialG: this.initialG,
            initialB: this.initialB,
            ssidAP: this.ssidAP,
            passwordAP: this.passwordAP,
            ssidWIFI: this.ssidWIFI,
            passwordWIFI: this.passwordWIFI,
            ledSettings: this.ledSettings
        };
    }
}