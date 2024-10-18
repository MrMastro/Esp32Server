export default class LedMainModel {
    constructor(aPConnection = true, actualHost = "", effect = "", deltaT = 500, color = "#000000", rgbCheck = true, ws2811Check = true) {
        this.aPConnection = aPConnection;
        this.actualHost = actualHost;
        this.effect = effect;
        this.deltaT = deltaT;
        this.color = color;
        this.rgbCheck = rgbCheck;
        this.ws2811Check = ws2811Check;
    }
}