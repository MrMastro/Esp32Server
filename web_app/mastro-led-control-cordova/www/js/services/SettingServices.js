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
        if (!this.logged) {
            return {};
        } else {
            let result = await HttpUtils.getCustom(host,ConstantApiList.getSettingsApi,{},{});
            let content;
            if(typeof result.data == 'string'){
                content = JSON.parse(result.data);
            }else{
                content = result.data;
            }
            //let data = content.data;
            this.settingModel.updateSettings(content.data);
            return this.settingModel;
        }
    }

    // Metodo per salvare le impostazioni del dispositivo
    async saveDeviceSettings(host, settings) {
        let result = await HttpUtils.postCustom(host,ConstantApiList.saveSettigsApi,{},settings);
        if(result.status != 200){
            return false;
        }
        return true;
    }

    isLogged() {
        return this.logged;
    }

    // Metodo per salvare le impostazioni del dispositivo
    async login(host, deviceName, devicePassword) {
        let result = await HttpUtils.postCustom(host, ConstantApiList.loginApi, {}, {deviceName: deviceName,devicePassword: devicePassword});
        if(result.status != 200){
            if(result.code == 200){
                console.warn("result.status != 200 but result.code == 200, is your similation on cordova simulate?");
                this.logged = true;
                return true;
            }
            return false;
        }
        this.logged = true;
        return true;
    }

}