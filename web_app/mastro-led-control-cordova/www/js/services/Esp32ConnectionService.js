import HttpUtils from '../utils/HttpUtils.js';
import ConstantApiList from '../constants/apiList.js';
import LocalStorageService from './LocalStorageService.js';
import IpV4StringUtils from '../utils/IpV4StringUtils.js';
import InfoEsp32Model from '../models/InfoEsp32Model.js';
import TextUtils from '../utils/TextUtils.js';
import { ConnectionInfo, Esp32Model } from '../models/Esp32Model.js';

export default class Esp32ConnectionService {
    constructor() {
        this.referenceHost = "";
        this.linkedDeviceSearch = [];
        this.localStorageService = new LocalStorageService();
        this.init();
    }

    async init() {
        await this.setLocalIP();
    }

    getIP() {
        return this.localStorageService.getLocalIp();
    }

    async setFakeIP() {
        // this.referenceHost = "192.168.1.120";
        this.referenceHost = "192.168.64.24";
    }

    async setLocalIP() {
        return new Promise((resolve) => {
            const RTCPeerConnection =
                window.RTCPeerConnection ||
                window.mozRTCPeerConnection ||
                window.webkitRTCPeerConnection;

            if (!RTCPeerConnection) {
                console.error("RTCPeerConnection non è supportato nel tuo browser.");
                this.referenceHost = "";
                return resolve(); // Restituisci stringa vuota
            }

            let pc = new RTCPeerConnection({ iceServers: [] });
            pc.createDataChannel(""); // Creazione di un canale dati fittizio

            pc.createOffer()
                .then(offer => pc.setLocalDescription(offer))
                .catch(() => {
                    console.error("Errore durante la creazione dell'offerta WebRTC.");
                    this.referenceHost = null;
                    resolve(); // Restituisci stringa vuota in caso di errore
                });

            pc.onicecandidate = (ice) => {
                if (!ice || !ice.candidate || !ice.candidate.candidate) return;

                const ipRegex = /([0-9]{1,3}(\.[0-9]{1,3}){3})/; // Regex per IP
                const ipMatch = ipRegex.exec(ice.candidate.candidate);

                if (ipMatch) {
                    this.referenceHost = ipMatch[1];
                    this.localStorageService.setLocalIp(ipMatch[1]);
                    pc.onicecandidate = null; // Disabilita ulteriori eventi
                    pc = null;
                    resolve();
                }
            };

            // Timeout nel caso in cui non venga trovato un candidato
            // setTimeout(() => {
            //     console.log("Errore: timeout nel recupero dell'IP locale.");
            //     this.referenceHost = null;
            //     resolve(); // Restituisci stringa vuota
            // }, 5000);
        });
    }

    async setLinkedDeviceSearch(callBackWhenDeviceFound = null) {
        this.linkedDeviceSearch = [];
        this.localStorageService.setEsp32InfoDeviceMem(this.linkedDeviceSearch);

        let adressIpV4 = this.referenceHost;

        if(adressIpV4 == null){
            this.localStorageService.setEsp32InfoDeviceMem(this.linkedDeviceSearch);
            return;
        }
        
        // console.log("My ip: " + adressIpV4 + " - Search device");

        if (cordova.platformId == 'android') {
            cordova.plugin.http.setRequestTimeout(ConstantApiList.timeoutForSearchtMs);
        }
    
        let initialIndex = 0;
    
        // Crea un array vuoto per raccogliere tutte le promesse
        let requests = [];
    
        // Aggiungiamo le promesse per ogni richiesta HTTP all'array
        for (let i = initialIndex; i < 255; i++) {
            let index = i;
            let adressI = IpV4StringUtils.getAdressWithIndexHost(adressIpV4, index);
    
            // Aggiungiamo la promise all'array requests
            requests.push(
                (async () => {
                    try {
                        let resultApi = await HttpUtils.getCustom(adressI, ConstantApiList.getInfoEsp32Hello, {}, {});
                        if (resultApi.status == 200 && resultApi.hasOwnProperty('data')) {
                            let result = resultApi.data;
                            if (typeof result === "string" && TextUtils.isJSON(result)) {
                                result = JSON.parse(result);
                            }
    
                            let esp32InfoFound = InfoEsp32Model.createModel(result);
                            if (esp32InfoFound != null) {
                                let esp32Found = new Esp32Model(ConnectionInfo.ONLINE, esp32InfoFound);
                                this.linkedDeviceSearch.push(esp32Found);
                                if (callBackWhenDeviceFound != null) {
                                    callBackWhenDeviceFound(resultApi);
                                }
                            } else {
                                console.log("Scartato in quanto json non valido: " + index);
                            }
                        } else {
                            console.log(`Scartato: ${index} - ${adressI}`);
                        }
                    } catch (error) {
                        console.log(`Errore nella richiesta per l'indirizzo ${adressI}:`, error);
                    }
                })() // Eseguiamo subito la funzione asincrona
            );
        }
    
        // Aspettiamo che tutte le richieste siano completate
        await Promise.all(requests);
    
        if (cordova.platformId == 'android') {
            cordova.plugin.http.setRequestTimeout(ConstantApiList.timeoutMs);
        }
        this.localStorageService.setEsp32InfoDeviceMem(this.linkedDeviceSearch);
    }

    async getLinkedDeviceSearch() {
        return this.linkedDeviceSearch;
    }

    setSingleDeviceOffline(espConnection){
        let updateList = this.localStorageService.getEsp32InfoDeviceMem();
        updateList.forEach( (esp32, i) => {
            if(esp32.infoConnection.macAdress == espConnection.infoConnection.macAdress){//confronto su mac adress perchè è univoco
                esp32.connectionState = ConnectionInfo.OFFLINE;
                esp32.active = false;
            }
        });
        this.linkedDeviceSearch = updateList;
        this.localStorageService.setEsp32InfoDeviceMem(this.linkedDeviceSearch);
    }

    async updateStatusDevices() {
        if (cordova.platformId == 'android') {
            cordova.plugin.http.setRequestTimeout(ConstantApiList.timeoutMs); //essendo pochi posso usare i timeout normali
        }
    
        let updateList = this.localStorageService.getEsp32InfoDeviceMem();
    
        // Creiamo un array di Promise per tutte le chiamate HTTP
        let requests = updateList.map(async (esp32, i) => {
            try {
                let resultApi = await HttpUtils.getCustom(esp32.infoConnection.ip, ConstantApiList.getInfoEsp32Hello, {}, {});
                if (resultApi.status == 200 && resultApi.hasOwnProperty('data')) {
                    let result = resultApi.data;
                    if (typeof result === "string" && TextUtils.isJSON(result)) {
                        result = JSON.parse(result);
                    }
    
                    let esp32InfoFound = InfoEsp32Model.createModel(result);
                    if (esp32InfoFound != null) {
                        esp32.active = true;
                        esp32.connectionState = ConnectionInfo.ONLINE;
                        esp32.infoConnection = esp32InfoFound; // aggiorno l'esp 32 se ha cambiato le info di connessione
                
                    } else {
                        esp32.active = false;
                        esp32.connectionState = ConnectionInfo.OFFLINE;
                    }
                } else {
                    esp32.active = false;
                    esp32.connectionState = ConnectionInfo.OFFLINE;
                }
            } catch (error) {
                console.log(`Errore nella richiesta per il dispositivo ${i}:`, error);
                esp32.active = false;
                esp32.connectionState = ConnectionInfo.OFFLINE;
            }
        });
    
        // Aspettiamo che tutte le richieste siano completate
        await Promise.all(requests);

        this.linkedDeviceSearch = updateList;
        this.localStorageService.setEsp32InfoDeviceMem(this.linkedDeviceSearch);

        if (cordova.platformId == 'android') {
            cordova.plugin.http.setRequestTimeout(ConstantApiList.timeoutMs);
        }
    }

    async createFakeEsp32(name,ip,mac){
        let json = {
            "deviceName": name,
            "ip": ip,
            "macAdress": mac
        };

        let esp32InfoFound = InfoEsp32Model.createModel(json);
        if (esp32InfoFound != null) {
            let esp32Found = new Esp32Model(ConnectionInfo.ONLINE,esp32InfoFound);
            this.linkedDeviceSearch.push(esp32Found);
        }
    }

    async createFakeVector(){
        this.createFakeEsp32("MastroDevice","192.168.64.63","08:D1:F9:E1:8C:00");
        this.createFakeEsp32("MastroDevice2","192.168.64.64","08:D1:F9:E1:8C:22");
        this.createFakeEsp32("MastroDevice3","192.168.64.65","08:D1:F9:E1:8C:33");
    }

}