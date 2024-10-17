import HttpUtils from '../utils/HttpUtils.js';
import ConstantApiList from '../constants/apiList.js';

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

    async postStartEffect(host, queryParam){
        let result = await HttpUtils.postCustom(host,ConstantApiList.sendEffectLedApi,queryParam,{});
        if(typeof result.data == 'string'){
            result.data = JSON.parse(result.data);
        }else{
            result.data = result.data;
        }
        return result; //todo sobtitute with model (genericResponse?)
    }

    async postStoptEffect(host, queryParam){
        let result = await HttpUtils.postCustom(host,ConstantApiList.sendStopEffectApi,queryParam,{});
        if(typeof result.data == 'string'){
            result.data = JSON.parse(result.data);
        }else{
            result.data = result.data;
        }
        return result; //todo sobtitute with model (genericResponse?)
    }

}