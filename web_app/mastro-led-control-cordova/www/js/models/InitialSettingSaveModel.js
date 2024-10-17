export default class InitialSettingSaveModel {
    constructor(initialEffect = "NO_EFFECT", initialDeltaT = 0, initialR = 0, initialG = 0, initialB = 0) {
        this.initialEffect = initialEffect;
        this.initialDeltaT = initialDeltaT;
        this.initialR = initialR;
        this.initialG = initialG;
        this.initialB = initialB;
    }
}