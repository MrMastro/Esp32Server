// src/controllers/Esp32ConnectionController.js

import ConstantApiList from '../constants/apiList';
import DefaultConstants from '../constants/DefaultConstants';
import FrontEndMessage from '../constants/FrontEndMessageItalian';
import GenericErrorExceptions from '../exceptions/GenericErrorException';
import NoConnectException from '../exceptions/NoConnectException';
import UnauthorizedErrorException from '../exceptions/UnauthorizedErrorException';
import { ConnectionInfo } from '../models/Esp32Model';
import SettingsModel from '../models/SettingModel.js';
import Esp32ConnectionService from '../services/Esp32ConnectionService.js';
import LocalStorageService from '../services/LocalStorageService';
import TimeUtils from '../utils/TimeUtils';
import AlertMessageView from '../views/AlertMessageView.vue';
import WaitView from '../views/WaitView.vue';

interface AlertMessageView {
    alert(title: string, message: string): void;
}

interface WaitView {
    show(): void;
    hide(): void;
    render(data?: any): void;
}

export default class Esp32ConnectionController {
    
    private context: any; // Adjust the type based on your actual context
    private esp32ConnectionView: any; // Adjust the type based on your actual view structure
    private loginView: any; // Adjust the type based on your actual view structure
    private settingView: any; // Adjust the type based on your actual view structure
    private waitView: WaitView;
    private alertMessageView: AlertMessageView;
    private localStorageService: LocalStorageService;
    private esp32ConnectionService: Esp32ConnectionService;
    private settingService: any; // Adjust the type based on your actual service structure
    
    constructor(context: { espConnectionView: any; loginView: any; settingView: any; espConnectionService: Esp32ConnectionService; settingService: any; }) {
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
        await this.firstUpdateStatusDevices();
        this.bindEvents();
    }

    async firstUpdateStatusDevices() {
        await this.esp32ConnectionService.updateStatusDevices();
        setTimeout(() => {
            this.esp32ConnectionView.render(this.localStorageService.getEsp32InfoDeviceMem());
        }, 1000); // 1000 millisecondi = 1 secondo
    }

    async bindEvents() {
        this.esp32ConnectionView.bindButtonSearchEsp32(this.runLinkedDeviceSearch.bind(this));
        this.esp32ConnectionView.bindButtonUpdateStatusEsp32(this.updateStatusEsp32.bind(this));
        this.esp32ConnectionView.bindButtonInfoEsp32(this.showInfo.bind(this));
        this.esp32ConnectionView.bindButtonAdvancedSearchEsp32(this.runLinkedDeviceAdvancedSearch.bind(this));
        this.loginView.bindBtnLogin(this.showSettings.bind(this));
        this.settingView.bindBtnSaveSettings(this.saveSettings.bind(this))
    }

    async runLinkedDeviceSearch() {
        this.waitView.show();
        try {
            await this.esp32ConnectionService.setLinkedDeviceSearch();
            await TimeUtils.processAndWait(this.esp32ConnectionView.render.bind(this.esp32ConnectionView), this.localStorageService.getEsp32InfoDeviceMem());
        } catch (error) {
            console.log("err: " + error);
            if(DefaultConstants.debugApp){
                this.esp32ConnectionView.render(this.localStorageService.getEsp32InfoDeviceMem());
            } else {
                this.esp32ConnectionView.render(this.localStorageService.getEsp32InfoDeviceMem());
            }
        }
        TimeUtils.wait(100);
        this.waitView.hide();
    }

    async runLinkedDeviceAdvancedSearch() {
        this.waitView.show();
        let valueIp = this.esp32ConnectionView.getIpForAdvancedSearch();
        await this.esp32ConnectionService.searchLinkedDevice(valueIp, true, null);
        this.waitView.hide();
        this.esp32ConnectionView.render(this.localStorageService.getEsp32InfoDeviceMem());
    }

    async updateStatusEsp32() {
        this.waitView.show();
        await this.esp32ConnectionService.updateStatusDevices();
        this.esp32ConnectionView.render(this.localStorageService.getEsp32InfoDeviceMem());
        this.waitView.hide();
    }

    showInfo(esp32Model: any) {
        this.loginView.show(esp32Model);
    }

    async showSettings(esp32Model: { espConnection: { infoConnection: any; connectionState?: any; }; }) {
        try {
            let i = esp32Model.espConnection.infoConnection;
            this.waitView.show();
            let login = await this.settingService.login(i.ip, i.deviceName, this.loginView.getDevicePassword());
            this.loginView.hide();
            await TimeUtils.wait(500);
            let settings = new SettingsModel();
            settings = await this.settingService.getSettings(esp32Model.espConnection.infoConnection.ip);
            this.waitView.hide();
            this.settingView.render(settings, esp32Model);
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