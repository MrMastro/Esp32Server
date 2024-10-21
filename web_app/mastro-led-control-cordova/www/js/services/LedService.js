import HttpUtils from '../utils/HttpUtils.js';
import ConstantApiList from '../constants/apiList.js';
import NoConnectException from '../exceptions/NoConnectException.js';
import UnauthorizedErrorException from '../exceptions/UnauthorizedErrorException.js';
import GenericErrorExceptions from '../exceptions/GenericErrorException.js';
import FrontEndMessage from '../constants/FrontEndMessageItalian.js';

export default class LedService {
    constructor() {
        this.logged = false;
    }

    async saveInitialEffect(host, initialSettingSaveModel){
        let result = await HttpUtils.postCustom(host,ConstantApiList.saveInitialEffectApi,{},initialSettingSaveModel);
        if(typeof result.data == 'string'){
            result.data = JSON.parse(result.data);
        }else{
            result.data = result.data;
        }
        return result;
    }

    async postStartEffect(host, requestBody){
        let result = await HttpUtils.postCustom(host,ConstantApiList.sendEffectLedApi,{},requestBody);
        if(typeof result.data == 'string'){
            result.data = JSON.parse(result.data);
        }else{
            result.data = result.data;
        }
        return result; //todo sobtitute with model (genericResponse?)
    }

    async postStoptEffect(host, requestBody){
        let result = await HttpUtils.postCustom(host,ConstantApiList.sendStopEffectApi,{},requestBody);
        if(typeof result.data == 'string'){
            result.data = JSON.parse(result.data);
        }else{
            result.data = result.data;
        }
        return result; //todo sobtitute with model (genericResponse?)
    }

    async getAvaibleEffects(host){
        let result = await HttpUtils.getCustom(host, ConstantApiList.getAvaibleEffects, {}, {});
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
                if(typeof result.data === 'string'){
                    result.data = JSON.parse(result.data);
                    return result.data.effects;
                }
                break;
            default:
                throw new GenericErrorExceptions(FrontEndMessage.genericError);
        }
        return result;
    }

}