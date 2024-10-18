import DefaultConstants from "../constants/DefaultConstants.js";

export default class LedMainModel {
    constructor(aPConnection = true, textFieldActualHost = "", effect = DefaultConstants.defaultEffectOption, deltaT = 500, color = "#000000", rgbCheck = true, ws2811Check = true) {
        this.aPConnection = aPConnection;
        this.textFieldActualHost = textFieldActualHost;
        this.effect = effect;
        this.deltaT = deltaT;
        this.color = color;
        this.rgbCheck = rgbCheck;
        this.ws2811Check = ws2811Check;
    }
}