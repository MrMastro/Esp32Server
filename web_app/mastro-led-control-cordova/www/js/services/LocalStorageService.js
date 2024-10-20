import DefaultConstants from "../constants/DefaultConstants.js";

export default class LocalStorageService {
    constructor() {
    }

    getLedEffectList() {
        const value = localStorage.getItem('ledEffectList');
        return value ? JSON.parse(value) : null;
    }

    setEffectList(list) {

        if (!Array.isArray(list)) {
            throw new Error("L'input deve essere un array.");
        }
        localStorage.setItem('ledEffectList', JSON.stringify(list));
    }

    checkExistenceEffectList() {
        return localStorage.getItem('ledEffectList') !== null;
    }

    formatEffectList() {
        localStorage.setItem('ledEffectList' , JSON.stringify(DefaultConstants.defaultEffects));
    }


}