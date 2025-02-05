import SettingModel from '../models/SettingModel.js';
import HttpUtils from '../utils/HttpUtils.js';
import ConstantApiList from '../constants/apiList.js';
import FrontEndMessage from '../constants/FrontEndMessageItalian.js';
import GenericErrorExceptions from '../exceptions/GenericErrorException.js';
import UnauthorizedErrorException from '../exceptions/UnauthorizedErrorException.js';
import NoConnectException from '../exceptions/NoConnectException.js';

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
            let result = await HttpUtils.getCustom(host, ConstantApiList.getSettingsApi, {}, {});
            let content;
            if (typeof result.data == 'string') {
                content = JSON.parse(result.data);
            } else {
                content = result.data;
            }

            switch (result.status) {
                case -4:
                    throw new NoConnectException(FrontEndMessage.noConnect);
                case 401:
                    throw new UnauthorizedErrorException(FrontEndMessage.unauthorized);
                case 200:
                    this.settingModel.updateSettings(content.data);
                    break;
                default:
                    throw new GenericErrorExceptions(FrontEndMessage.genericError);
            }
            return this.settingModel;
        }
    }

    // Metodo per salvare le impostazioni del dispositivo
    async saveDeviceSettings(hosts, settings) {
        hosts.forEach(async (host) => {
            let result = await HttpUtils.postCustom(host, ConstantApiList.saveSettigsApi, {}, settings);
            let content;
            if (typeof result.data == 'string') {
                content = JSON.parse(result.data);
            } else {
                content = result.data;
            }
    
            switch (result.status) {
                case -4:
                    throw new NoConnectException(FrontEndMessage.noConnect);
                case 401:
                    throw new UnauthorizedErrorException(FrontEndMessage.unauthorized);
                case 200:
                    break;
                default:
                    throw new GenericErrorExceptions(FrontEndMessage.genericError);
            }
        });
        return true;
    }

    isLogged() {
        return this.logged;
    }

    // Metodo per salvare le impostazioni del dispositivo
    async login(host, deviceName, devicePassword) {
        let result = await HttpUtils.postCustom(host, ConstantApiList.loginApi, {}, { deviceName: deviceName, devicePassword: devicePassword });
        if (result.status != 200) {
            if (result.code == 200) {
                console.warn("result.status != 200 but result.code == 200, is your similation on cordova simulate?");
                this.logged = true;
                return true;
            }
            return result;
        }
        this.logged = true;
        return result;
    }

}