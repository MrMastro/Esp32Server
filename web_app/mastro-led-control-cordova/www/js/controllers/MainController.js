import WaitView from '../views/WaitView.js';
import AlertMessageView from '../views/AlertMessageView.js';
import LedService from '../services/LedService.js';
import FrontEndMessage from '../constants/FrontEndMessageItalian.js'
import InitialSettingSaveModel from '../models/InitialSettingSaveModel.js';
import HeaderView from '../views/HeaderView.js';
import FooterView from '../views/FooterView.js';
import SettingController from './SettingController.js';
import MainView from '../views/MainView.js';
import LocalStorageService from '../services/LocalStorageService.js';
import LedMainModel from '../models/LedMainModel.js';
import DefaultConstants from '../constants/DefaultConstants.js';
import UnauthorizedErrorException from '../exceptions/UnauthorizedErrorException.js';
import NoConnectException from '../exceptions/NoConnectException.js';
import GenericErrorExceptions from '../exceptions/GenericErrorException.js';
import TimeUtils from '../utils/TimeUtils.js';
import LedEffectRequest from '../models/request/LedEffectRequest.js';
import TextUtils from '../utils/TextUtils.js';
import Esp32ConnectionService from '../services/Esp32ConnectionService.js';
import Esp32ConnectionController from './Esp32ConnectionController.js';

export default class MainController {
    constructor(context = null) {
        this.context = context;
        this.esp32ConnectionService = context.espConnectionService;
        this.espConnectionView = this.context.espConnectionView;
        //status variable:
        // this.referenceHost = host;
        // this.apHost = DefaultConstants.defaultApHost;
        // this.debug = DefaultConstants.defaultDebug;
        //Component
        this.ledService = new LedService();
        this.localStorageService = new LocalStorageService();
        this.mainView = new MainView(document.getElementById('MainViewContainer'),[]);
        this.headerView = new HeaderView(document.getElementById('HeaderViewContainer'));
        this.footerView = new FooterView(document.getElementById('FooterViewContainer'));
        this.settingController = context.settingController;
        this.waitView = new WaitView(document.getElementById('WaitViewContainer'));
        this.alertMessageView = new AlertMessageView(document.getElementById('AlertMessageViewContainer'));
        this.init();
    }

    async init() {
        this.initilizeStorage();
        this.mainView.render(new LedMainModel(), this.localStorageService.getLedEffectList());
        this.waitView.render();
        this.bindEvents();

    }

    initilizeStorage(){
        if(!this.localStorageService.checkExistenceEffectList()){
            this.localStorageService.formatEffectList();
        }
    }

    async bindEvents() {
        this.mainView.bindBtnWs2811SetEffect(this.sendStartEffect.bind(this));
        this.mainView.bindBtnStopEffect(this.sendStopEffect.bind(this));
        this.mainView.bindBtnSaveInitialEffect(this.saveInitialEffect.bind(this));
        this.mainView.bindBtnUpdateEffect(this.updateEffectList.bind(this));

        this.mainView.bindBtnClearInitialEffect(this.clearInitialEffect.bind(this));
        this.mainView.bindRangeInputChange(this.rangeInputChange.bind(this));
        this.mainView.bindInputChange(this.inputChange.bind(this));
    }

    rangeInputChange(){
        let valueRange = this.mainView.getTimingRangeInput();
        this.mainView.setTimingInput(valueRange);
    }

    inputChange(alsoMinEdge = false){
        let valueTime = this.mainView.getTimingInput();
        valueTime = TextUtils.fixTextNumber(valueTime);
        valueTime = TextUtils.textToIntegerNumber(valueTime);
        if(valueTime >= 1000){
            valueTime = 1000;
        }
        if(alsoMinEdge){
            if(valueTime < 50){
                valueTime = 50;
            }
        }
        this.mainView.setTimingInput(valueTime);
        this.mainView.setTimingRangeInput(valueTime);
    }

    showSettings() {
        this.settingController.showModal();
    }

    async showWait() {
        this.waitView.show();
    }

    async hideWait() {
        this.waitView.hide();
    }

    // SUCCESS AND FAILURE METHOD
    genericSuccessAlert(nameEsp32) {
        this.alertMessageView.alert(nameEsp32 + FrontEndMessage.titleSuccess, FrontEndMessage.genericSuccessOperation);
    }

    customAlert(title, content){
        this.alertMessageView.alert(title, content);
    }

    genericFailureAlert(infoTitle, content) {
        this.alertMessageView.alert(infoTitle + FrontEndMessage.titleError, content);
    }

    async updateEffectList(){
        try {
            this.waitView.show();
            await TimeUtils.wait(200);
            let firstConnection = this.context.espConnectionView.getActiveConnections()[0];
            let list = await this.ledService.getAvaibleEffects(firstConnection);
            await TimeUtils.wait(200);
            this.waitView.hide();
            await TimeUtils.wait(200);
            this.localStorageService.setEffectList(list);
            this.mainView.render( this.mainView.getLedMainModel(), list);
            this.alertMessageView.alert(FrontEndMessage.titleSuccess, FrontEndMessage.updateEffectListSuccess);
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
                this.hideWait();
                this.alertMessageView.alert(FrontEndMessage.titleError, FrontEndMessage.genericError);
            }
            else {
                this.waitView.hide();
                this.alertMessageView.alert(FrontEndMessage.titleError, FrontEndMessage.genericError);
                console.log(error);
            }
        }
    }

    async saveInitialEffect() {
        let activeConnections = this.context.espConnectionView.getActiveConnections();
        let ledModel = new LedMainModel();
        ledModel =  this.mainView.getLedMainModel();
        let request = new InitialSettingSaveModel(ledModel.effect, ledModel.deltaT, ledModel.colors);
        this.showWait();
        activeConnections.forEach( async (esp32SingleReference) => {
            let esp32Model = esp32SingleReference.espConnection;
            let connectionInfo = esp32SingleReference.espConnection.infoConnection;
            let result = await this.ledService.saveInitialEffect(connectionInfo.ip, request);
            this.valutateResponseAlertMessage(result,esp32Model);
        });
        this.hideWait();
    }

    async clearInitialEffect() {
        let activeConnections = this.context.espConnectionView.getActiveConnections();
        this.showWait();
        activeConnections.forEach( async (esp32SingleReference) => {
            let esp32Model = esp32SingleReference.espConnection;
            let connectionInfo = esp32SingleReference.espConnection.infoConnection;
            let result = await this.ledService.saveInitialEffect(connectionInfo.ip, new InitialSettingSaveModel());
            this.valutateResponseAlertMessage(result,esp32Model);
        });
        this.hideWait();
    }

    async sendStartEffect() {
        let activeConnections = this.context.espConnectionView.getActiveConnections();
        let ledModel = new LedMainModel();
        ledModel =  this.mainView.getLedMainModel();
        let request = new LedEffectRequest(ledModel.effect, ledModel.colors, ledModel.deltaT, ledModel.rgbCheck, ledModel.ws2811Check);
        this.showWait();
        activeConnections.forEach( async (esp32SingleReference) => {
            let esp32Model = esp32SingleReference.espConnection;
            let connectionInfo = esp32SingleReference.espConnection.infoConnection;
            let result = await this.ledService.postStartEffect(connectionInfo.ip, request);
            this.valutateResponseAlert(result, esp32Model);
        });
        this.hideWait();
    }

    async sendStopEffect() {
        let activeConnections = this.context.espConnectionView.getActiveConnections();
        let ledModel = new LedMainModel();
        ledModel =  this.mainView.getLedMainModel();
        let request = new LedEffectRequest(ledModel.effect, ledModel.colors, ledModel.deltaT, ledModel.rgbCheck, ledModel.ws2811Check);
        this.showWait();
        activeConnections.forEach( async (esp32SingleReference) => {
            let esp32Model = esp32SingleReference.espConnection;
            let connectionInfo = esp32SingleReference.espConnection.infoConnection;
            let result = await this.ledService.postStoptEffect(connectionInfo.ip, request);
            this.valutateResponseAlert(result, esp32Model);
        });
        this.hideWait();
    }

    valutateResponseAlert(response, esp32Model = "") {
        let nameEsp32 = "";
        if(typeof esp32Model.infoConnection.deviceName == 'string'){
            nameEsp32 = esp32Model.infoConnection.deviceName+" - ";
        }
        switch (response.status) {
            case -4:
                this.genericFailureAlert(nameEsp32, FrontEndMessage.noConnect);
                this.setOfflineDevice(esp32Model);
                break;
            case 200:
                this.genericSuccessAlert(nameEsp32);
                break;
            default:
                this.genericFailureAlert(nameEsp32, FrontEndMessage.noConnect);
                this.setOfflineDevice(esp32Model);
                break;
        }
    }

    valutateResponseAlertMessage(response, esp32Model = "") {
        let nameEsp32 = "";
        if(typeof esp32Model.infoConnection.deviceName == 'string'){
            nameEsp32 = esp32Model.infoConnection.deviceName+" - ";
        }
        switch (response.status) {
            case -4:
                this.genericFailureAlert(nameEsp32, FrontEndMessage.noConnect);
                this.setOfflineDevice(esp32Model);
                break;
            case 200:
                this.customAlert(nameEsp32 + FrontEndMessage.titleSuccess, response.data.status.description);//todo change data.status.description in data.infoStatus.description
                break;
            default:
                break;
        }
    }

    setOfflineDevice(esp32Model){
        this.esp32ConnectionService.setSingleDeviceOffline(esp32Model);
        this.espConnectionView.render(this.localStorageService.getEsp32InfoDeviceMem());
    }

}