import HttpUtils from '../utils/HttpUtils.js';
import ConstantApiList from '../constants/apiList.js';
import LocalStorageService from './LocalStorageService.js';
import IpV4StringUtils from '../utils/IpV4StringUtils.js';

export default class Esp32ConnectionService {
    constructor() {
        this.localStoraceService = new LocalStorageService();
    }

    //Return a list of InfoEsp32Model calling hello api (info of connection to esp32 devices)
    async setLinkedDeviceSearch(adressIpV4, callBackWhenDeviceFound){
        console.log("Search device with host: " + adressIpV4);
        let memDevices = this.localStoraceService.getEsp32InfoDeviceMem();

        if(memDevices == null){
            memDevices = [];
        }

        if(cordova.platformId == 'android'){
            cordova.plugin.http.setRequestTimeout(ConstantApiList.timeoutForSearchtMs);
        }

        //todo
        let initialIndex = .,. IpV4StringUtils.getAdressWithIndexHost(adressIpV4, 0);

        for (let index = initialIndex; index < 255; index++) {
            let adressI = IpV4StringUtils.getAdressWithIndexHost(adressIpV4, index);
            let resultApi = await HttpUtils.getCustom(adressI, "ApiAdress",{},{});
            callBackWhenDeviceFound(resultApi);
        }

        if(cordova.platformId == 'android'){
            cordova.plugin.http.setRequestTimeout(ConstantApiList.timeoutMs);
        }

    }

    async callHelloEsp32Api(host){
        console.log("WIP");
    }

}