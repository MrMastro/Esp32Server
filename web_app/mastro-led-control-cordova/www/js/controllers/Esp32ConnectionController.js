import SettingService from '../services/SettingServices.js';
import LoginView from '../views/LoginView.js';
import SettingView from '../views/SettingView.js';
import WaitView from '../views/WaitView.js';
import AlertMessageView from '../views/AlertMessageView.js';
import TimeUtils from '../utils/TimeUtils.js'
import FrontEndMessage from '../constants/FrontEndMessageItalian.js'
import LocalStorageService from '../services/LocalStorageService.js';


export default class SettingController {
    constructor(host, headerView) {
        this.localStorageService = new LocalStorageService();
        this.init();
    }

    async init() {
        // this.loginView.render();
        // this.waitView.render();
        this.bindEvents();
    }

    async bindEvents() {
        requestAnimationFrame(() => {
            console.log("WIP");
            // this.settingView.bindBtnSaveSettings(this.saveSettings.bind(this));
            // this.loginView.bindBtnLogin(this.login.bind(this));
            // this.headerView.bindBtnSettings(() => this.showModal());
        });
    }

    async setLinkedDeviceSearch(){
        console.log("WIP");

    }


    // async login() {
    //     this.loginView.hide();
    //     this.waitView.show();
    //     let deviceName = this.loginView.getDeviceName();
    //     let password = this.loginView.getDevicePassword();
    //     let result = await this.settingService.login(this.referenceHost, deviceName, password);
    //     await TimeUtils.wait(500);
    //     this.waitView.hide();

    //     //validate result
    //     switch (result.status) {
    //         case -4:
    //             this.alertMessageView.alert(FrontEndMessage.titleError, FrontEndMessage.noConnect);
    //             break;
    //         case 401:
    //             this.alertMessageView.alert(FrontEndMessage.titleError, FrontEndMessage.unauthorizedWrongCredential);
    //             break;
    //         case 200:
    //             let settings = await this.settingService.getSettings(this.referenceHost);
    //             this.settingView.render(settings);
    //             this.waitView.hide();
    //             this.settingView.show();
    //             break;
    //         default:
    //             this.waitView.hide();
    //             this.alertMessageView.alert(FrontEndMessage.titleError, FrontEndMessage.genericError);
    //             break;
    //     }
    // }

}