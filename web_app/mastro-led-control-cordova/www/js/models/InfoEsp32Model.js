export default class InfoEsp32Model {
    constructor(deviceName = "",ip ="") {
        // Inizializza i valori del dispositivo
        this.deviceName = deviceName;
        this.ip = ip;
    }

    // Metodo per aggiornare le impostazioni del dispositivo
    updateInfoEsp32(newInfoEsp32) {
        Object.assign(this, newInfoEsp32);
    }
    
}