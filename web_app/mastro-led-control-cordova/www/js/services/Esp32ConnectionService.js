import HttpUtils from '../utils/HttpUtils.js';
import ConstantApiList from '../constants/apiList.js';
import LocalStorageService from './LocalStorageService.js';
import IpV4StringUtils from '../utils/IpV4StringUtils.js';
import InfoEsp32Model from '../models/InfoEsp32Model.js';
import TextUtils from '../utils/TextUtils.js';
import { ConnectionInfo, Esp32Model } from '../models/Esp32Model.js';
import NoConnectException from '../exceptions/NoConnectException.js';

export default class Esp32ConnectionService {
    constructor(context) {
        this.referenceHost = "";
        this.context = context;
        this.linkedDeviceSearch = [];
        this.localStorageService = new LocalStorageService();
        this.init();
    }

    async init() {
    }

    getIP() {
        return this.localStorageService.getLocalIp();
    }

    async setFakeIP() {
        // this.referenceHost = "192.168.1.120";
        this.referenceHost = "192.168.64.24";
    }

    async setLocalIP() {
        await this.setLocalIpV2();
    }

    async getLocalIpOldMethod() {
        return new Promise((resolve) => {
            const RTCPeerConnection =
                window.RTCPeerConnection ||
                window.mozRTCPeerConnection ||
                window.webkitRTCPeerConnection;

            if (!RTCPeerConnection) {
                console.error("RTCPeerConnection non è supportato nel tuo browser.");
                this.referenceHost = "";
                return resolve(""); // Restituisci stringa vuota
            }

            let pc = new RTCPeerConnection({ iceServers: [] });
            pc.createDataChannel(""); // Creazione di un canale dati fittizio

            pc.createOffer()
                .then(offer => pc.setLocalDescription(offer))
                .catch(() => {
                    console.error("Errore durante la creazione dell'offerta WebRTC.");
                    this.referenceHost = null;
                    resolve(""); // Restituisci stringa vuota in caso di errore
                });

            pc.onicecandidate = (ice) => {
                if (!ice || !ice.candidate || !ice.candidate.candidate) return;

                const ipRegex = /([0-9]{1,3}(\.[0-9]{1,3}){3})/; // Regex per IP
                const ipMatch = ipRegex.exec(ice.candidate.candidate);

                if (ipMatch) {
                    let ip = ipMatch[1];
                    pc.onicecandidate = null; // Disabilita ulteriori eventi
                    pc = null;
                    if (ip.startsWith("192.168.")) {
                        this.referenceHost = ip;
                        this.localStorageService.setLocalIp(ip);
                        resolve(ip);
                    } else {
                        this.referenceHost = null;
                        this.localStorageService.setLocalIp(null);
                        resolve(null);
                    }
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

    async setLocalIpV2() {
        return new Promise(async (resolve, reject) => {
            if (this.context.networkState === Connection.WIFI) {
                networkinterface.getWiFiIPAddress(
                    (data) => {
                        if (data.ip.startsWith("192.168.")) {
                            this.referenceHost = data.ip;
                            this.localStorageService.setLocalIp(data.ip);
                            resolve(data.ip);
                        } else {
                            this.referenceHost = null;
                            this.localStorageService.setLocalIp(null);
                            resolve(null);
                        } // Risolvi la Promise con l'IP
                    },
                    (error) => {
                        this.referenceHost = null;
                        this.localStorageService.setLocalIp(null);
                        resolve(null);
                        console.error("Errore nel recupero dell'IP:", error);
                        reject(error); // Rifiuta la Promise in caso di errore
                    }
                );
            } else {
                await networkinterface.getCarrierIPAddress(
                    async (data) => {
                        // Verifica se l'IP inizia con "192.168."
                        if (data.ip.startsWith("192.168.")) {
                            this.referenceHost = data.ip;
                            this.localStorageService.setLocalIp(data.ip);
                            resolve(data.ip);
                        } else {
                            this.referenceHost = null;
                            this.localStorageService.setLocalIp(null);
                            resolve(null);
                        }
                    },
                    async (error) => {
                        console.error("Errore nel recupero IP:", error);
                        resolve(await this.getLocalIpOldMethod()());
                    }
                );
            }
        });
    }


    async setLinkedDeviceSearch(callBackWhenDeviceFound = null) {
        await this.setLocalIP();
        let adressIpV4 = this.referenceHost;
        if (!adressIpV4) { // Prova le reti conosciute
            let ipKnows = this.localStorageService.getIpListKnown() || [];
            for (let ip of ipKnows) {
                await this.searchLinkedDevice(ip, true, callBackWhenDeviceFound);
                if (this.linkedDeviceSearch != []) { //ho trovato almeno 1 dispositivo nella rete, interrompo
                    break; 
                }
            }
        } else {
            await this.searchLinkedDevice(adressIpV4, false, callBackWhenDeviceFound);
        }
    }

    async searchLinkedDevice(ipV4, addIpToListKnown, callBackWhenDeviceFound = null){
        this.linkedDeviceSearch = [];
        this.localStorageService.setEsp32InfoDeviceMem(this.linkedDeviceSearch);
        let noConnection = false;
        let initialIndex = 0;
        let requests = [];

        if (cordova.platformId == 'android') {
            cordova.plugin.http.setRequestTimeout(ConstantApiList.timeoutForSearchtMs);
        }

        // Aggiungiamo le promesse per ogni richiesta HTTP all'array
        for (let i = initialIndex; i < 255; i++) {
            let adressI = IpV4StringUtils.getAdressWithIndexHost(ipV4, i);

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
                                if(addIpToListKnown){
                                    this.updateIpListKnown(ipV4);
                                }
                                if (callBackWhenDeviceFound != null) {
                                    callBackWhenDeviceFound(resultApi);
                                }
                            } else {
                                console.log("Scartato in quanto json non valido: " + i);
                            }
                        } else {
                            console.log(`Scartato: ${i} - ${adressI}`);
                        }
                    } catch (error) {
                        console.log(`Errore nella richiesta per l'indirizzo ${adressI}:`, error);
                        if (error.status == -6) {
                            noConnection = true;
                        }
                    }
                })() // Eseguiamo subito la funzione asincrona
            );
        }

        // Aspettiamo che tutte le richieste siano completate
        await Promise.all(requests);

        if (cordova.platformId == 'android') {
            cordova.plugin.http.setRequestTimeout(ConstantApiList.timeoutMs);
        }

        if (noConnection) {
            throw new NoConnectException();
        }

        this.localStorageService.setEsp32InfoDeviceMem(this.linkedDeviceSearch);
    }

    updateIpListKnown(ip){
        let list = this.localStorageService.getIpListKnown();
        let found = false;
        if(!Array.isArray(list)){
            list = [];
        }

        list.forEach(ipEl => {
            if(ip == ipEl){
                found = true;
                return;
            }            
        });

        if(!found){
            list.push(ip);
        }

        this.localStorageService.setIpListKnown(list);

    }

    async getLinkedDeviceSearch() {
        return this.linkedDeviceSearch;
    }

    setSingleDeviceOffline(espConnection) {
        let updateList = this.localStorageService.getEsp32InfoDeviceMem();
        updateList.forEach((esp32, i) => {
            if (esp32.infoConnection.macAdress == espConnection.infoConnection.macAdress) {//confronto su mac adress perchè è univoco
                esp32.connectionState = ConnectionInfo.OFFLINE;
                esp32.active = false;
            }
        });
        this.linkedDeviceSearch = updateList;
        this.localStorageService.setEsp32InfoDeviceMem(this.linkedDeviceSearch);
    }

    async updateStatusDevices() {
        
        let updateList = this.localStorageService.getEsp32InfoDeviceMem();
        
        if(!Array.isArray(updateList)){
            return;
        }

        if (cordova.platformId == 'android') {
            cordova.plugin.http.setRequestTimeout(ConstantApiList.timeoutForSearchtMs); //essendo pochi posso usare i timeout normali
        }


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

    async createFakeEsp32(name, ip, mac) {
        let json = {
            "deviceName": name,
            "ip": ip,
            "macAdress": mac
        };

        let esp32InfoFound = InfoEsp32Model.createModel(json);
        if (esp32InfoFound != null) {
            let esp32Found = new Esp32Model(ConnectionInfo.ONLINE, esp32InfoFound);
            this.linkedDeviceSearch.push(esp32Found);
        }
    }

    async createFakeVector() {
        this.createFakeEsp32("MastroDevice", "192.168.64.63", "08:D1:F9:E1:8C:00");
        this.createFakeEsp32("MastroDevice2", "192.168.64.64", "08:D1:F9:E1:8C:22");
        this.createFakeEsp32("MastroDevice3", "192.168.64.65", "08:D1:F9:E1:8C:33");
    }

}