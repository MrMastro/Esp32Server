import DefaultConstants from "../constants/DefaultConstants.js";

export default class LedMainModel {
    constructor(aPConnection = true, textFieldActualHost = "", effect = DefaultConstants.defaultEffectOption, deltaT = 500, colors = [{r:0,g:0,b:0}], rgbCheck = true, ws2811Check = true) {
        this.aPConnection = aPConnection;
        this.textFieldActualHost = textFieldActualHost;
        this.effect = effect;
        this.deltaT = deltaT;
        this.colors = colors;
        this.rgbCheck = rgbCheck;
        this.ws2811Check = ws2811Check;
    }
}