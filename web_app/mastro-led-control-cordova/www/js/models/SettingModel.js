export default class SettingsModel {
    constructor(
        deviceName = "", 
        devicePassword = "", 
        communicationMode = "AP_MODE", 
        debug = false, 
        initialEffect = "NO_EFFECT", 
        initialDeltaT = 0, 
        initialColors = [{r: 0, g: 0, b: 0}],
        ssidAP = "",
        passwordAP = "",
        ssidWIFI = "",
        passwordWIFI = "",
        enableStripRgb = false,
        pinLedDinRgb = 19,
        pinLedCinRgb = 18,
        enableStripWs2811 = false,
        numLedWs2811 = 32,
        pinLedWs2811 = 5
    ) {
        // Inizializza i valori del dispositivo con quelli passati o di default
        this.deviceName = deviceName;
        this.devicePassword = devicePassword;
        this.communicationMode = communicationMode;
        this.debug = debug;
        this.initialEffect = initialEffect;
        this.initialDeltaT = initialDeltaT;
        this.initialColors = initialColors;
        this.ssidAP = ssidAP;
        this.passwordAP = passwordAP;
        this.ssidWIFI = ssidWIFI;
        this.passwordWIFI = passwordWIFI;
        
        // Impostazioni LED
        this.ledSettings = {
            enableStripWs2811: enableStripWs2811,
            numLedWs2811: numLedWs2811,
            pinLedWs2811: pinLedWs2811,
            enableStripRgb: enableStripRgb,
            pinLedDinRgb: pinLedDinRgb,
            pinLedCinRgb: pinLedCinRgb
        };
    }

    // Metodo per aggiornare le impostazioni del dispositivo
    updateSettings(newSettings) {
        Object.assign(this, newSettings);
    }

    // Metodo per ottenere la configurazione corrente
    getSettings() {
        return this;
    }
}