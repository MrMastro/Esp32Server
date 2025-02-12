import ConstantApiList from '../constants/apiList.js';
import FrontEndMessage from '../constants/FrontEndMessageItalian.js';
import GenericErrorExceptions from '../exceptions/GenericErrorException.js';
import NoConnectException from '../exceptions/NoConnectException.js';
import UnauthorizedErrorException from '../exceptions/UnauthorizedErrorException.js';
import { ConnectionInfo } from '../models/Esp32Model.js';

import SettingsModel from '../models/SettingModel.js';
import Esp32ConnectionService from '../services/Esp32ConnectionService.js';
import LocalStorageService from '../services/LocalStorageService.js';
import TimeUtils from '../utils/TimeUtils.js';
import AlertMessageView from '../views/AlertMessageView.js';
import WaitView from '../views/WaitView.js';


export default class Esp32ConnectionController {
    constructor(context) {

        this.context = context;

        this.esp32ConnectionView = context.espConnectionView; 
        this.loginView = context.loginView;
        this.settingView = context.settingView;
        this.waitView = new WaitView(document.getElementById('WaitViewContainer'));

        this.alertMessageView = new AlertMessageView(document.getElementById('AlertMessageViewContainer'));

        this.localStorageService = new LocalStorageService();
        this.esp32ConnectionService = context.espConnectionService;
        this.settingService = context.settingService;
        this.esp32ConnectionView.render(this.localStorageService.getEsp32InfoDeviceMem());
        this.init();
    }

    async init() {
        this.waitView.render();
        this.bindEvents();
    }

    async bindEvents() {
        this.esp32ConnectionView.bindButtonSearchEsp32(this.runLinkedDeviceSearch.bind(this));
        this.esp32ConnectionView.bindButtonUpdateStatusEsp32(this.updateStatusEsp32.bind(this));
        this.esp32ConnectionView.bindButtonInfoEsp32(this.showInfo.bind(this));
        this.loginView.bindBtnLogin(this.showSettings.bind(this));
        this.settingView.bindBtnSaveSettings(this.saveSettings.bind(this))
    }

    async runLinkedDeviceSearch(){
        this.waitView.show();
        try {
            await this.esp32ConnectionService.setLinkedDeviceSearch();
            this.esp32ConnectionView.render(this.localStorageService.getEsp32InfoDeviceMem());
        } catch (error) {
            console.log("err: " + error);
        }
        this.waitView.hide();
    }

    async updateStatusEsp32(){
        this.waitView.show();
        await this.esp32ConnectionService.updateStatusDevices();
        this.esp32ConnectionView.render(this.localStorageService.getEsp32InfoDeviceMem());
        this.waitView.hide();
    }

    showInfo(esp32Model){
        this.loginView.show(esp32Model);
    }

    async showSettings(esp32Model) {
        try {
            let i = esp32Model.espConnection.infoConnection;
            this.waitView.show();
            let login = await this.settingService.login(i.ip, i.deviceName, this.loginView.getDevicePassword());
            this.loginView.hide();
            await TimeUtils.wait(500);
            let settings = new SettingsModel();
            settings = await this.settingService.getSettings(esp32Model.espConnection.infoConnection.ip);
            this.waitView.hide();
            this.settingView.render(settings,esp32Model);
            this.settingView.show(esp32Model);
        } catch (error) {
            // Gestione dell'UnauthorizedError
            if (error instanceof UnauthorizedErrorException) {
                this.waitView.hide();
                this.alertMessageView.alert(FrontEndMessage.titleError, FrontEndMessage.unauthorizedWrongCredential);
                this.loginView.show(esp32Model);
            }
            // Gestione del NoConnectException
            else if (error instanceof NoConnectException) {
                this.waitView.hide();
                this.alertMessageView.alert(FrontEndMessage.titleError, FrontEndMessage.noConnect);
                this.esp32ConnectionService.setSingleDeviceOffline(esp32Model.espConnection);
                esp32Model.espConnection.connectionState = ConnectionInfo.OFFLINE;
                this.loginView.show(esp32Model);
            }
            // Gestione del GenericError
            else if (error instanceof GenericErrorExceptions) {
                this.waitView.hide();
                this.alertMessageView.alert(FrontEndMessage.titleError, FrontEndMessage.genericError);
                this.esp32ConnectionService.setSingleDeviceOffline(esp32Model.espConnection);
                esp32Model.espConnection.connectionState = ConnectionInfo.OFFLINE;
                this.loginView.show(esp32Model);
            }
            // Altri errori inattesi
            else {
                this.waitView.hide();
                this.alertMessageView.alert(FrontEndMessage.titleError, FrontEndMessage.genericError);
                this.esp32ConnectionService.setSingleDeviceOffline(esp32Model.espConnection);
                esp32Model.espConnection.connectionState = ConnectionInfo.OFFLINE;
                this.loginView.show(esp32Model);
            }
        }
    }

    async saveSettings() {
        let getEsp32Selected = this.settingView.getEsp32Selected();
        this.settingView.hide();
        this.waitView.show();
        try {
            let data = await this.settingView.getSettings();
            await this.settingService.saveDeviceSettings(getEsp32Selected.espConnection.infoConnection.ip, data);
            await TimeUtils.wait(ConstantApiList.waitWhileEsp32Rebooting);
            await this.updateStatusEsp32();
            this.alertMessageView.alert(FrontEndMessage.titleSuccess, FrontEndMessage.saveSettingsSuccess);
            this.waitView.hide();
        } catch (error) {
            if (error instanceof UnauthorizedErrorException) {
                this.waitView.hide();
                this.alertMessageView.alert(FrontEndMessage.titleError, FrontEndMessage.unauthorizedRelogin);
                this.loginView.show();
            }
            else if (error instanceof NoConnectException) {
                this.waitView.hide();
                this.alertMessageView.alert(FrontEndMessage.titleError, FrontEndMessage.noConnect);
            }
            else if (error instanceof GenericErrorExceptions) {
                this.waitView.hide();
                this.alertMessageView.alert(FrontEndMessage.titleError, FrontEndMessage.genericError);
            }
            else {
                this.waitView.hide();
                this.alertMessageView.alert(FrontEndMessage.titleError, FrontEndMessage.genericError);
            }
        }
    }

}