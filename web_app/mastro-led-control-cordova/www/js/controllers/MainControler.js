import WaitView from '../views/WaitView.js';
import AlertMessageView from '../views/AlertMessageView.js';
import LedService from '../services/LedService.js';
import ColorUtils from '../utils/ColorUtils.js';
import FrontEndMessage from '../constants/FrontEndMessageItalian.js'
import InitialSettingSaveModel from '../models/InitialSettingSaveModel.js';

export default class MainController {
    constructor(host) {
        this.ledService = new LedService();
        this.waitView = new WaitView(document.getElementById('WaitViewContainer'));
        this.alertMessageView = new AlertMessageView(document.getElementById('AlertMessageViewContainer'));
        this.referenceHost = host;
        this.init();
    }

    async init() {
        this.waitView.render();
        this.bindEvents();
    }

    async bindEvents() {

        $('.buttonWs2811SetEffect').on('click', this.sendStartEffect.bind(this));
        $('.buttonWs2811StopEffect').on('click', this.sendStopEffect.bind(this));

        $('.saveInitialEffect').on('click', this.saveInitialEffect.bind(this));
        $('.clearInitialEffect').on('click', this.clearInitialEffect.bind(this));

        // requestAnimationFrame(() => {
        //     document.querySelector('.saveSettingsBtn').addEventListener('click', () => {
        //         this.saveSettings();
        //     });
        // });
    }

    async setReferenceHost(newHost) {
        this.referenceHost = newHost;
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
        //this.valutateResponseAlert(result);
    }

    async clearInitialEffect() {
        this.showWait();
        let result = await this.ledService.saveInitialEffect(this.referenceHost, new InitialSettingSaveModel());
        this.hideWait();
        this.valutateResponseAlertMessage(result);
        //this.valutateResponseAlert(result);
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