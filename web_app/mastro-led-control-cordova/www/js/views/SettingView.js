export default class SettingView {
    constructor() { //empty cause modify html yet written
        this.modal = $("#Settings-Modal");
        this.deviceNameInput = document.querySelector('#deviceNameInput');
        this.devicePasswordInput = document.querySelector('#devicePasswordInput');
        this.communicationModeInput = document.querySelector('#communicationModeInput');
        this.debugInput = document.querySelector('#debugInput');
        this.ssidAPInput = document.querySelector('#ssidAPInput');
        this.passwordAPInput = document.querySelector('#passwordAPInput');
        this.ssidWIFIInput = document.querySelector('#ssidWIFIInput');
        this.passwordWIFIInput = document.querySelector('#passwordWIFIInput');
        this.initialEffectInput = document.querySelector('#initialEffectInput');
    }

    render(settings) {
        // Aggiorna i campi della modale con i dati ricevuti dal Controller
        document.querySelector('#deviceNameInput').value = settings.deviceName;
        document.querySelector('#devicePasswordInput').value = settings.devicePassword;
        document.querySelector('#communicationModeInput').value = settings.communicationMode;
        document.querySelector('#debugInput').checked = settings.debug;
        document.querySelector('#ssidAPInput').value = settings.ssidAP;
        document.querySelector('#passwordAPInput').value = settings.passwordAP;
        document.querySelector('#ssidWIFIInput').value = settings.ssidWIFI;
        document.querySelector('#passwordWIFIInput').value = settings.passwordWIFI;
        document.querySelector('#initialEffectInput').value = settings.initialEffect;
        document.querySelector('#initialDeltaTInput').innerText = settings.initialDeltaT;
    
        // Imposta i valori RGB iniziali
        this.setRgbInput(settings.initialR, settings.initialG, settings.initialB);
        
        // Aggiorna le impostazioni delle luci LED
        document.querySelector('#enableStripRgbInput').checked = settings.ledSettings.enableStripRgb;
        document.querySelector('#pinLedCinRgbInput').value = settings.ledSettings.pinLedCinRgb;
        document.querySelector('#pinLedDinRgbInput').value = settings.ledSettings.pinLedDinRgb;
        document.querySelector('#enableStripWs2811Input').checked = settings.ledSettings.enableStripWs2811;
        document.querySelector('#numLedWs2811Input').value = settings.ledSettings.numLedWs2811;
        document.querySelector('#pinLedWs2811Input').value = settings.ledSettings.pinLedWs2811;
    }

    show() {
        $(this.modal).modal('show');
    }

    hide() {
        // Nascondi la modale
        $(this.modal).modal('hide');
    }

    // Metodo ausiliario per impostare i valori RGB nei campi di input
    setRgbInput(r, g, b) {
        const rgbHex = this.rgbToHex(r, g, b);
        document.querySelector('#initialRgbInput').value = rgbHex;
    }

    // Metodo ausiliario per convertire valori RGB in formato esadecimale
    rgbToHex(r, g, b) {
        const toHex = (c) => {
            const hex = c.toString(16).padStart(2, '0');
            return hex.length === 2 ? hex : '00';
        };
        return `#${toHex(r)}${toHex(g)}${toHex(b)}`;
    }

}