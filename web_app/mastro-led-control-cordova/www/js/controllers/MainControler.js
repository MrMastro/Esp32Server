import WaitView from '../views/WaitView.js';
import AlertMessageView from '../views/AlertMessageView.js';
import LedService from '../services/LedService.js';
import ColorUtils from '../utils/ColorUtils.js';
import FrontEndMessage from '../constants/FrontEndMessageItalian.js'
import InitialSettingSaveModel from '../models/InitialSettingSaveModel.js';
import HeaderView from '../views/HeaderView.js';
import FooterView from '../views/FooterView.js';
import SettingController from './SettingController.js';
import MainView from '../views/MainView.js';
import LocalStorageService from '../services/LocalStorageService.js';
import LedMainModel from '../models/LedMainModel.js';
import DefaultConstants from '../constants/DefaultConstants.js';

export default class MainController {
    constructor(host) {
        //status variable:
        this.referenceHost = host;
        this.apHost = DefaultConstants.defaultApHost;
        this.debug = DefaultConstants.defaultDebug;

        //Component
        this.ledService = new LedService();
        this.localStorageService = new LocalStorageService();
        this.mainView = new MainView(document.getElementById('MainViewContainer'),[]);
        this.headerView = new HeaderView(document.getElementById('HeaderViewContainer'));
        this.footerView = new FooterView(document.getElementById('FooterViewContainer'));
        this.settingController = new SettingController(host, this.headerView);
        this.waitView = new WaitView(document.getElementById('WaitViewContainer'));
        this.alertMessageView = new AlertMessageView(document.getElementById('AlertMessageViewContainer'));
        this.init();
    }

    async init() {
        this.initilizeStorage();
        this.mainView.render(new LedMainModel(),this.localStorageService.getLedEffectList());
        this.waitView.render();
        this.switchConnection();
        this.bindEvents();
    }

    initilizeStorage(){
        if(!this.localStorageService.checkExistenceEffectList()){
            this.localStorageService.formatEffectList();
        }
    }

    async bindEvents() {

        this.mainView.bindBtnSetEffect(this.sendStartEffect.bind(this));
        this.mainView.bindBtnStopEffect(this.sendStopEffect.bind(this));
        this.mainView.bindBtnSaveInitialEffect(this.saveInitialEffect.bind(this));
        this.mainView.bindBtnClearInitialEffect(this.clearInitialEffect.bind(this));
        this.mainView.bindFieldIpInput(this.changeIp.bind(this));
        this.mainView.bindAPConnectionSwitch(this.switchConnection.bind(this));
        this.mainView.bindRangeInputChange(this.rangeInputChange.bind(this));
        this.mainView.bindInputChange(this.inputChange.bind(this));
    }

    async setReferenceHost(newHost) {
        this.referenceHost = newHost;
    }

    switchConnection() {
        if (this.mainView.isAPconnection()) {
            this.mainView.hideFieldIp();
            this.referenceHost = this.apHost;
            this.mainView.setLabelIp("Indirizzo attuale: " + this.referenceHost);
        } else {
            this.mainView.showFieldIp();
            this.changeIp();
        }
    }

    rangeInputChange(){
        let valueRange = this.mainView.getTimingRangeInput();
        this.mainView.setTimingInput(valueRange);
    }

    inputChange(){
        let valueTime = this.mainView.getTimingInput();
        if(valueTime <= 0){
            valueTime = 0;
        }else if(valueTime >= 1000){
            valueTime = 1000;
        }
        this.mainView.setTimingInput(valueTime);
        this.mainView.setTimingRangeInput(valueTime);
    }

    changeIp() {
        let test = /^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/.test(this.mainView.getFieldIp());
        
        if (!this.mainView.getFieldIp()) {
            this.mainView.setLabelIp("Inserisci un indirizzo ip");
        }
        else if (test) {
            this.referenceHost = this.mainView.getFieldIp();
            this.mainView.setLabelIp("Indirizzo attuale: " + this.referenceHost);
            this.settingController.setReferenceHost(this.referenceHost);
            this.setReferenceHost(this.referenceHost);
        } else {
            this.mainView.setLabelIp("Inserisci un ip valido");
        }
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
    genericSuccessAlert() {
        this.alertMessageView.alert(FrontEndMessage.titleSuccess, FrontEndMessage.genericSuccessOperation);
    }

    customAlert(title, content){
        this.alertMessageView.alert(title, content);
    }

    genericFailureAlert(content) {
        this.alertMessageView.alert(FrontEndMessage.titleError, content);
    }

    async saveInitialEffect() {
        //todo create a view, for now this controller get html element
        let initialEffect = $(".effectInput")[0].value; //CONSTANTS_UNIQUE_COLOR;
        let initialDeltaT = $(".timingInput")[0].value; //100;
        let color = $(".colorInput")[0].value;
        let initialR = ColorUtils.hexToRgb(color).r;
        let initialG = ColorUtils.hexToRgb(color).g;
        let initialB = ColorUtils.hexToRgb(color).b;

        this.showWait();
        let result = await this.ledService.saveInitialEffect(this.referenceHost, new InitialSettingSaveModel(initialEffect, initialDeltaT, initialR, initialG, initialB));
        this.hideWait();
        this.valutateResponseAlertMessage(result);
    }

    async clearInitialEffect() {
        this.showWait();
        let result = await this.ledService.saveInitialEffect(this.referenceHost, new InitialSettingSaveModel());
        this.hideWait();
        this.valutateResponseAlertMessage(result);
    }

    async sendStartEffect() {
        //todo create a view, for now this controller get html element
        let effect = $(".effectInput")[0].value; //CONSTANTS_UNIQUE_COLOR;
        let timing = $(".timingInput")[0].value; //100;
        let color = $(".colorInput")[0].value;
        let r = ColorUtils.hexToRgb(color).r;
        let g = ColorUtils.hexToRgb(color).g;
        let b = ColorUtils.hexToRgb(color).b;
        let rgbAction = $("#rgbCheck")[0].checked;
        let ws2811Action = $("#ws2811Check")[0].checked;

        //todo subdtitute with model or get Model by View
        var queryParam =
            "effect=" + encodeURIComponent(effect) +
            "&timing=" + encodeURIComponent(timing) +
            "&color=" + encodeURIComponent(color) +
            "&r=" + encodeURIComponent(r) +
            "&g=" + encodeURIComponent(g) +
            "&b=" + encodeURIComponent(b) +
            "&rgbAction=" + encodeURIComponent(rgbAction) +
            "&ws2811Action=" + encodeURIComponent(ws2811Action);
        this.showWait();
        let result = await this.ledService.postStartEffect(this.referenceHost, queryParam);
        this.hideWait();
        console.log(result);
        this.valutateResponseAlert(result);
    }

    async sendStopEffect() {
        //todo create a view, for now this controller get html element
        let effect = $(".effectInput")[0].value; //CONSTANTS_UNIQUE_COLOR;
        let timing = $(".timingInput")[0].value; //100;
        let color = $(".colorInput")[0].value;
        let r = ColorUtils.hexToRgb(color).r;
        let g = ColorUtils.hexToRgb(color).g;
        let b = ColorUtils.hexToRgb(color).b;
        let rgbAction = $("#rgbCheck")[0].checked;
        let ws2811Action = $("#ws2811Check")[0].checked;

        //todo subdtitute with model or get Model by View
        var queryParam =
            "effect=" + encodeURIComponent(effect) +
            "&timing=" + encodeURIComponent(timing) +
            "&color=" + encodeURIComponent(color) +
            "&r=" + encodeURIComponent(r) +
            "&g=" + encodeURIComponent(g) +
            "&b=" + encodeURIComponent(b) +
            "&rgbAction=" + encodeURIComponent(rgbAction) +
            "&ws2811Action=" + encodeURIComponent(ws2811Action);

        this.showWait();
        let result = await this.ledService.postStoptEffect(this.referenceHost, queryParam);
        this.hideWait();
        console.log(result);
        this.valutateResponseAlert(result);
    }

    valutateResponseAlert(response) {
        switch (response.status) {
            case -4:
                this.genericFailureAlert(FrontEndMessage.noConnect);
                break;
            case 200:
                this.genericSuccessAlert();
                break;
            default:
                break;
        }
    }

    valutateResponseAlertMessage(response) {
        switch (response.status) {
            case -4:
                this.genericFailureAlert(FrontEndMessage.noConnect);
                break;
            case 200:
                this.customAlert(FrontEndMessage.titleSuccess, response.data.status.description);//todo change data.status.description in data.infoStatus.description
                break;
            default:
                break;
        }
    }

}