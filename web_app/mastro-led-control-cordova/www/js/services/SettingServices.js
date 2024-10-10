import SettingModel from '../models/SettingModel.js';

export default class SettingService {
    constructor() {
        this.settingModel = new SettingModel();
        this.logged = false;
    }

    // Metodo per ottenere le impostazioni del dispositivo
    async getSettings() {
        // Simulazione di una chiamata API
        if (!this.logged) {
            return this.settingModel;
        } else {
            let promise = new Promise((resolve) => {
                setTimeout(() => {
                    resolve(this.settingModel);
                }, 1000);
            }); //todo subtitute with api
            // Aspetta che la promessa venga risolta e restituisci il risultato
            const settings = await promise;
            return this.settingModel; // Restituisce le impostazioni
        }
    }

    // Metodo per salvare le impostazioni del dispositivo
    async saveDeviceSettings(settings) {
        // Simulazione di una chiamata API
        return new Promise((resolve) => {
            setTimeout(() => {
                this.settingModel.updateSettings(settings);
                resolve(true);
            }, 1000);
        });
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