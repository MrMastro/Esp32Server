import HttpUtils from '../utils/HttpUtils.js';
import ConstantApiList from '../constants/apiList.js';
import LocalStorageService from './LocalStorageService.js';
import IpV4StringUtils from '../utils/IpV4StringUtils.js';

export default class Esp32ConnectionService {
    constructor() {
        this.referenceHost = "";
        this.linkedDeviceSearch = [];
        this.localStoraceService = new LocalStorageService();
        this.init();
    }

    async init(){
        await this.setFakeIP();
    }

    getIP(){
        return this.referenceHost;
    }

    async setFakeIP() {
        this.referenceHost = "192.168.1.120";
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
                this.referenceHost ="";
                resolve(); // Restituisci stringa vuota
            }, 5000);
        });
    }

    //set a list of InfoEsp32Model calling hello api (info of connection to esp32 devices)
    async setLinkedDeviceSearch(callBackWhenDeviceFound){
        this.linkedDeviceSearch = [];
        let adressIpV4 = this.referenceHost;
        console.log("Search device with host: " + adressIpV4);
        let memDevices = this.localStoraceService.getEsp32InfoDeviceMem();

        if(memDevices == null){
            memDevices = [];
        }

        if(cordova.platformId == 'android'){
            cordova.plugin.http.setRequestTimeout(ConstantApiList.timeoutForSearchtMs);
        }

        //todo
        let initialIndex = 0;

        for (let index = initialIndex; index < 255; index++) {
            let adressI = IpV4StringUtils.getAdressWithIndexHost(adressIpV4, index);
            let resultApi = await HttpUtils.getCustom(adressI, "ApiAdress",{},{});
            this.linkedDeviceSearch.push(resultApi);
            callBackWhenDeviceFound(resultApi);
        }

        if(cordova.platformId == 'android'){
            cordova.plugin.http.setRequestTimeout(ConstantApiList.timeoutMs);
        }

    }

    async getLinkedDeviceSearch(){
        return this.linkedDeviceSearch;
    }

    async callHelloEsp32Api(host){
        console.log("WIP");
    }

}