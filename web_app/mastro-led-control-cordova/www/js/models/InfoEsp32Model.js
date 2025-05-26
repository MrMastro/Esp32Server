export default class InfoEsp32Model {
    constructor(deviceName = "",ip ="", macAdress="") {
        // Inizializza i valori del dispositivo
        this.deviceName = deviceName;
        this.ip = ip;
        this.macAdress = macAdress;
    }

    // Metodo per aggiornare le impostazioni del dispositivo
    updateInfoEsp32(newInfoEsp32) {
        Object.assign(this, newInfoEsp32);
    }

    static validate(object){
        return (object.hasOwnProperty("deviceName") && object.hasOwnProperty("ip") && object.hasOwnProperty("macAdress"))
    }

    static createModel(object){
        if(this.validate(object)){
            return new InfoEsp32Model(object.deviceName, object.ip, object.macAdress);
        }else{
            console.error("Cannot create, the object passed isn't InfoEsp32Model");
            return null;
        }
    }
    
}