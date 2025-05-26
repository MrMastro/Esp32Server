import SettingService from '../services/SettingServices.js';
import LoginView from '../views/LoginView.js';
import SettingView from '../views/SettingView.js';
import WaitView from '../views/WaitView.js';
import AlertMessageView from '../views/AlertMessageView.js';
import TimeUtils from '../utils/TimeUtils.js'
import FrontEndMessage from '../constants/FrontEndMessageItalian.js'
import UnauthorizedErrorException from '../exceptions/UnauthorizedErrorException.js';
import NoConnectException from '../exceptions/NoConnectException.js';
import GenericErrorExceptions from '../exceptions/GenericErrorException.js';
import SettingModel from '../models/SettingModel.js'

export default class SettingController {
    constructor(context) {
        this.settingService = context.settingService;
        this.settingView = new SettingView(document.getElementById('SettingsViewContainer'));
        //this.headerView = headerView;
        this.loginView = new LoginView(document.getElementById('LoginViewContainer'));
        this.waitView = new WaitView(document.getElementById('WaitViewContainer'));
        this.alertMessageView = new AlertMessageView(document.getElementById('AlertMessageViewContainer'));
        this.init();
    }

    async init() {
        this.loginView.render();
        this.waitView.render();
        this.bindEvents();
    }

    async bindEvents() {
        requestAnimationFrame(() => {
            this.settingView.bindBtnSaveSettings(this.saveSettings.bind(this));
            //this.loginView.bindBtnLogin(this.login.bind(this));
            //this.headerView.bindBtnSettings(() => this.showModal());
        });
    }

    async saveSettings() {
        this.settingView.hide();
        this.waitView.show();
        try {
            let data = await this.settingView.getSettings();
            //TODO - fix with context
            await this.settingService.saveDeviceSettings(this.context.espConnectionView.getActiveConnections(), data);
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

    async showSettings(esp32Model) {
        try {
            this.waitView.show();
            let settings = new SettingModel();
            settings = await this.settingService.getSettings(this.referenceHost);
            await TimeUtils.wait(500);
            this.waitView.hide();
            this.settingView.render(settings);
            this.settingView.show();
        } catch (error) {
            // Gestione dell'UnauthorizedError
            if (error instanceof UnauthorizedErrorException) {
                this.waitView.hide();
                this.alertMessageView.alert(FrontEndMessage.titleError, FrontEndMessage.unauthorizedRelogin);
                this.loginView.show();
            }
            // Gestione del NoConnectException
            else if (error instanceof NoConnectException) {
                this.waitView.hide();
                this.alertMessageView.alert(FrontEndMessage.titleError, FrontEndMessage.noConnect);
            }
            // Gestione del GenericError
            else if (error instanceof GenericErrorExceptions) {
                this.waitView.hide();
                this.alertMessageView.alert(FrontEndMessage.titleError, FrontEndMessage.genericError);
            }
            // Altri errori inattesi
            else {
                this.waitView.hide();
                this.alertMessageView.alert(FrontEndMessage.titleError, FrontEndMessage.genericError);
            }
        }
    }


    async showModal() {
        if (this.settingService.isLogged()) {
            this.showSettings();
        } else {
            this.loginView.show();
        }
    }

    async login() {
        this.loginView.hide();
        this.waitView.show();
        let deviceName = this.loginView.getDeviceName();
        let password = this.loginView.getDevicePassword();
        let result = await this.settingService.login(this.referenceHost, deviceName, password);
        await TimeUtils.wait(500);
        this.waitView.hide();

        //validate result
        switch (result.status) {
            case -4:
                this.alertMessageView.alert(FrontEndMessage.titleError, FrontEndMessage.noConnect);
                break;
            case 401:
                this.alertMessageView.alert(FrontEndMessage.titleError, FrontEndMessage.unauthorizedWrongCredential);
                break;
            case 200:
                let settings = await this.settingService.getSettings(this.referenceHost);
                this.settingView.render(settings);
                this.waitView.hide();
                this.settingView.show();
                break;
            default:
                this.waitView.hide();
                this.alertMessageView.alert(FrontEndMessage.titleError, FrontEndMessage.genericError);
                break;
        }
    }

    async setReferenceHost(newHost) {
        this.referenceHost = newHost;
    }

}