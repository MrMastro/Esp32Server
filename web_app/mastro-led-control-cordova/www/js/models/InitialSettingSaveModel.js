export default class InitialSettingSaveModel {
    constructor(initialEffect = "NO_EFFECT", initialDeltaT = 0, initialColors = [{r:0,g:0,b:0}]) {
        this.initialEffect = initialEffect;
        this.initialDeltaT = initialDeltaT;
        this.initialColors = initialColors;
    }
}