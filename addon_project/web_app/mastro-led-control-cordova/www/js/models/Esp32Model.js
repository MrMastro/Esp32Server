import InfoEsp32Model from "./InfoEsp32Model.js";

export const ConnectionInfo = {
    ONLINE: "ONLINE",
    OFFLINE: "OFFLINE",
    UNKNOWN: "UNKNOWN"
};

export class Esp32Model {
    /**
     * @param {ConnectionInfo} connectionState - Stato della connessione (obbligatorio dall'enumeratore ConnectionInfo)
     * @param {InfoEsp32} infoEsp32 - Nome del dispositivo
     */
    constructor(connectionState, infoEsp32) {
        if (!Object.values(ConnectionInfo).includes(connectionState)) {
            throw new Error("connectionState deve essere un valore dell'enumeratore ConnectionInfo");
        }
        this.infoConnection = InfoEsp32Model.createModel(infoEsp32);
        this.connectionState = connectionState;
        this.active = true;
    }

    // Metodo per aggiornare le impostazioni del dispositivo
    updateInfoEsp32(el) {
        Object.assign(this.infoConnection, el);
    }

    /**
     * @param {ConnectionInfo} connectionState - Stato della connessione (obbligatorio dall'enumeratore ConnectionInfo)
     */ 
    setConnection(connectionState) {
        if (!Object.values(ConnectionInfo).includes(connectionState)) {
            throw new Error("connectionState deve essere un valore dell'enumeratore ConnectionInfo");
        }
        this.connectionState = connectionState;
    }

    getInformation() {
        return this.infoConnection;
    }

    static validate(object) {
        return (object.hasOwnProperty("connectionState") && object.hasOwnProperty("infoConnection") && object.infoConnection.hasOwnProperty("deviceName") && object.infoConnection.hasOwnProperty("ip") && object.infoConnection.hasOwnProperty("macAddress"));
    }

    static createModel(object) {
        if (this.validate(object)) {
            return new Esp32Model(ConnectionInfo.UNKNOWN, object.deviceName, object.ip, object.macAddress);
        } else {
            console.error("Cannot create, the object passed isn't valid for Esp32Model");
            return null;
        }
    }
}
