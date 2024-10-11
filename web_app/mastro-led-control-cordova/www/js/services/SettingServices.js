import SettingModel from '../models/SettingModel.js';
import HttpUtils from '../utils/HttpUtils.js';
import ConstantApiList from '../constants/apiList.js';

export default class SettingService {
    constructor() {
        this.settingModel = new SettingModel();
        this.logged = false;
    }

    // Metodo per ottenere le impostazioni del dispositivo
    async getSettings(host) {
        // Simulazione di una chiamata API
        if (!this.logged) {
            return this.settingModel;
        } else {
            let result = await HttpUtils.getCustom(host,ConstantApiList.getSettingsApi,{},{});
            let content = JSON.parse(result.data);
            //let data = content.data;
            this.settingModel.updateSettings(content.data);
            return this.settingModel;
        }
    }

    // Metodo per salvare le impostazioni del dispositivo
    async saveDeviceSettings(host, settings) {
        let result = await HttpUtils.postCustom(host,ConstantApiList.saveSettigsApi,{},settings);
        if(result == "ERROR"){
            return false;
        }
        let content = JSON.parse(result.data);
        this.settingModel.updateSettings(content.data);
        return this.settingModel;
    }

    isLogged() {
        return this.logged;
    }

    // Metodo per salvare le impostazioni del dispositivo
    async login(deviceName, devicePassword) {
        // Simulazione di una chiamata API
        return new Promise((resolve) => {
            setTimeout(() => {
                //todo call espserver
                this.logged = true;
                resolve(true);
            }, 1000);
        });
    }

}