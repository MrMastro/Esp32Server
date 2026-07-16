import LedColorRequest from './LedColorRequest';  // Assicurati di importare la classe LedColorRequest

export default class LedEffectRequest {
    effect: string;
    colors: LedColorRequest[];
    ms: number;
    rgbAction: boolean;
    ws2811Action: boolean;
    ws2811MatrixAction: boolean;
    constructor(effect = '', colors: LedColorRequest[] = [], ms = 0, rgbAction = false, ws2811Action = false, ws2811MatrixAction = false) {
        this.effect = effect;
        this.colors = colors; // Array di LedColorRequest
        this.ms = ms;
        this.rgbAction = rgbAction;
        this.ws2811Action = ws2811Action;
        this.ws2811MatrixAction = ws2811MatrixAction;
    }

    // Serializzazione in JSON
    toJson() {
        return {
            effect: this.effect,
            colors: this.colors.map(color => color.toJson()), // Serializza ogni LedColorRequest
            ms: this.ms,
            rgbAction: this.rgbAction,
            ws2811Action: this.ws2811Action,
            ws2811MatrixAction: this.ws2811MatrixAction
        };
    }

    // Deserializzazione da JSON
    static fromJson(json: { effect: string | undefined; ms: number | undefined; rgbAction: boolean | undefined; ws2811Action: boolean | undefined; ws2811MatrixAction: boolean | undefined; colors: any[]; }) {
        if (!json || typeof json.effect === 'undefined' || typeof json.ms === 'undefined' ||
            typeof json.rgbAction === 'undefined' || typeof json.ws2811Action === 'undefined' || typeof json.ws2811MatrixAction === 'undefined' || !Array.isArray(json.colors)) {
            throw new Error('Invalid JSON structure for LedEffectRequest');
        }

        // Deserializza l'array di colori
        const colors = json.colors.map((colorJson: any) => LedColorRequest.fromJson(colorJson));

        return new LedEffectRequest(json.effect, colors, json.ms, json.rgbAction, json.ws2811Action, json.ws2811MatrixAction);
    }
}