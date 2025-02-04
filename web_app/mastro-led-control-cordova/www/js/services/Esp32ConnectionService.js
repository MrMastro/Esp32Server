import HttpUtils from '../utils/HttpUtils.js';
import ConstantApiList from '../constants/apiList.js';
import LocalStorageService from './LocalStorageService.js';
import IpV4StringUtils from '../utils/IpV4StringUtils.js';
import InfoEsp32Model from '../models/InfoEsp32Model.js';
import TextUtils from '../utils/TextUtils.js';

export default class Esp32ConnectionService {
    constructor() {
        this.referenceHost = "";
        this.linkedDeviceSearch = [];
        this.localStoraceService = new LocalStorageService();
        this.init();
    }

    async init() {
        await this.setFakeIP();
    }

    getIP() {
        return this.referenceHost;
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

            const pc = new RTCPeerConnection({ iceServers: [] });
            pc.createDataChannel(""); // Creazione di un canale dati fittizio

            pc.createOffer()
                .then(offer => pc.setLocalDescription(offer))
                .catch(() => {
                    console.error("Errore durante la creazione dell'offerta WebRTC.");
                    this.referenceHost = "";
                    resolve(); // Restituisci stringa vuota in caso di errore
                });

            pc.onicecandidate = (ice) => {
                if (!ice || !ice.candidate || !ice.candidate.candidate) return;

                const ipRegex = /([0-9]{1,3}(\.[0-9]{1,3}){3})/; // Regex per IP
                const ipMatch = ipRegex.exec(ice.candidate.candidate);

                if (ipMatch) {
                    this.referenceHost = ipMatch[1];
                    pc.onicecandidate = null; // Disabilita ulteriori eventi
                    resolve();
                }
            };

            // Timeout nel caso in cui non venga trovato un candidato
            setTimeout(() => {
                console.log("Errore: timeout nel recupero dell'IP locale.");
                this.referenceHost = "";
                resolve(); // Restituisci stringa vuota
            }, 5000);
        });
    }

    //set a list of InfoEsp32Model calling hello api (info of connection to esp32 devices)
    async setLinkedDeviceSearch(callBackWhenDeviceFound=null) {
        this.createFakeVector();
        this.localStoraceService.setEsp32InfoDeviceMem(this.linkedDeviceSearch);
        this.linkedDeviceSearch = [];
        let adressIpV4 = this.referenceHost;
        console.log("My ip: "+adressIpV4+" - Search device");
        let memDevices = this.localStoraceService.getEsp32InfoDeviceMem();

        if (memDevices == null) {
            memDevices = [];
        }

        if (cordova.platformId == 'android') {
            cordova.plugin.http.setRequestTimeout(ConstantApiList.timeoutForSearchtMs);
        }

        //todo
        let initialIndex = 0;

        for (let index = initialIndex; index < 255; index++) {
            let adressI = IpV4StringUtils.getAdressWithIndexHost(adressIpV4, index);
            let resultApi = await HttpUtils.getCustom(adressI, ConstantApiList.getInfoEsp32Hello, {}, {});
            if (resultApi.status == 200 && resultApi.hasOwnProperty('data')) {
                let result = resultApi;
                if (typeof resultApi.data == "string") {
                    if (TextUtils.isJSON(resultApi.data)) {
                        result = JSON.parse(resultApi.data);
                    }
                } else {
                    result = resultApi.data;
                }
                let esp32Found = InfoEsp32Model.createModel(result);
                if (esp32Found != null) {
                    this.linkedDeviceSearch.push(esp32Found);
                    if(callBackWhenDeviceFound != null){
                        callBackWhenDeviceFound(resultApi);
                    }
                } else {
                    console.log("Scartato in quanto json non valido: " + index);
                }
            } else {
                console.log("Scartato: " + index);
            }
        }

        if (cordova.platformId == 'android') {
            cordova.plugin.http.setRequestTimeout(ConstantApiList.timeoutMs);
        }
    }

    async getLinkedDeviceSearch() {
        return this.linkedDeviceSearch;
    }

    async callHelloEsp32Api(host) {
        console.log("WIP");
    }

    async createFakeEsp32(name,ip,mac){
        let json = {
            "deviceName": name,
            "ip": ip,
            "macAdress": mac
        };

        let esp32Found = InfoEsp32Model.createModel(json);
        if (esp32Found != null) {
            this.linkedDeviceSearch.push(esp32Found);
        }
    }

    async createFakeVector(){
        this.createFakeEsp32("MastroDevice","192.168.64.63","08:D1:F9:E1:8C:00");
        this.createFakeEsp32("MastroDevice2","192.168.64.64","08:D1:F9:E1:8C:22");
        this.createFakeEsp32("MastroDevice3","192.168.64.65","08:D1:F9:E1:8C:33");
    }

}