export default class LoginModel {
    constructor() {
        // Inizializza i valori del dispositivo
        this.deviceName = "";
        this.devicePassword = "";
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
        };
    }
}