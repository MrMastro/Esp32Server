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
            throw new Error("L'input deve essere un array di stringhe.");
        }

        // Controlla se tutte le voci dell'array sono stringhe
        for (let i = 0; i < list.length; i++) {
            if (typeof list[i] !== 'string') {
                throw new Error("Tutti gli elementi dell'array devono essere stringhe.");
            }
        }

        localStorage.setItem('ledEffectList', JSON.stringify(arrayDiStringhe));
    }

    checkExistenceEffectList() {
        return localStorage.getItem('ledEffectList') !== null;
    }

    formatEffectList() {
        localStorage.setItem('ledEffectList' , JSON.stringify(DefaultConstants.defaultEffects));
    }


}