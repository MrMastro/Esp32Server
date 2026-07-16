import InfoEsp32Model from "./InfoEsp32Model";

export const ConnectionInfo = {
    ONLINE: "ONLINE",
    OFFLINE: "OFFLINE",
    UNKNOWN: "UNKNOWN"
};

export class Esp32Model {
    infoConnection: InfoEsp32Model | null;
    connectionState: string;
    active: boolean;

    /**
     * @param {string} connectionState - Stato della connessione (obbligatorio dall'enumeratore ConnectionInfo)
     * @param {InfoEsp32Model} infoEsp32 - Informazioni del dispositivo
     */
    constructor(connectionState: string, infoEsp32: InfoEsp32Model) {
        if (
            connectionState !== ConnectionInfo.ONLINE &&
            connectionState !== ConnectionInfo.OFFLINE &&
            connectionState !== ConnectionInfo.UNKNOWN
        ) {
            throw new Error("connectionState deve essere un valore dell'enumeratore ConnectionInfo");
        }
        this.infoConnection = infoEsp32;
        this.connectionState = connectionState;
        this.active = true;
    }

    // Metodo per aggiornare le impostazioni del dispositivo
    updateInfoEsp32(el: Partial<InfoEsp32Model>) {
        if (this.infoConnection) {
            Object.assign(this.infoConnection, el);
        }
    }

    /**
     * @param {string} connectionState - Stato della connessione (obbligatorio dall'enumeratore ConnectionInfo)
     */ 
    setConnection(connectionState: string) {
        if (connectionState !== ConnectionInfo.ONLINE && connectionState !== ConnectionInfo.OFFLINE && connectionState !== ConnectionInfo.UNKNOWN) {
            throw new Error("connectionState deve essere un valore dell'enumeratore ConnectionInfo");
        }
        this.connectionState = connectionState;
    }

    getInformation() {
        return this.infoConnection;
    }

    static validate(object: { infoConnection: InfoEsp32Model }) {
        return (object.hasOwnProperty("connectionState") && object.infoConnection.hasOwnProperty("deviceName") && object.infoConnection.hasOwnProperty("ip") && object.infoConnection.hasOwnProperty("macAddress"));
    }

    static createModel(object: any) {
        if (this.validate(object)) {
            return new Esp32Model(ConnectionInfo.UNKNOWN, object.infoConnection);
        } else {
            console.error("Cannot create, the object passed isn't valid for Esp32Model");
            return null;
        }
    }
}

