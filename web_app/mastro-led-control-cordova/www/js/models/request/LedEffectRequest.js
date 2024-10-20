import LedColorRequest from './LedColorRequest.js';  // Assicurati di importare la classe LedColorRequest

export default class LedEffectRequest {
    constructor(effect = '', colors = [], ms = 0, rgbAction = false, ws2811Action = false) {
        this.effect = effect;
        this.colors = colors; // Array di LedColorRequest
        this.ms = ms;
        this.rgbAction = rgbAction;
        this.ws2811Action = ws2811Action;
    }

    // Serializzazione in JSON
    toJson() {
        return {
            effect: this.effect,
            colors: this.colors.map(color => color.toJson()), // Serializza ogni LedColorRequest
            ms: this.ms,
            rgbAction: this.rgbAction,
            ws2811Action: this.ws2811Action
        };
    }

    // Deserializzazione da JSON
    static fromJson(json) {
        if (!json || typeof json.effect === 'undefined' || typeof json.ms === 'undefined' ||
            typeof json.rgbAction === 'undefined' || typeof json.ws2811Action === 'undefined' || !Array.isArray(json.colors)) {
            throw new Error('Invalid JSON structure for LedEffectRequest');
        }

        // Deserializza l'array di colori
        const colors = json.colors.map(colorJson => LedColorRequest.fromJson(colorJson));

        return new LedEffectRequest(json.effect, colors, json.ms, json.rgbAction, json.ws2811Action);
    }
}