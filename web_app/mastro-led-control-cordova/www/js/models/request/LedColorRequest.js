export default class LedColorRequest {
    constructor(r = 0, g = 0, b = 0) {
        this.r = r;
        this.g = g;
        this.b = b;
    }

    // Serializzazione in JSON
    toJson() {
        return {
            r: this.r,
            g: this.g,
            b: this.b
        };
    }

    // Deserializzazione da JSON
    static fromJson(json) {
        if (!json || typeof json.r === 'undefined' || typeof json.g === 'undefined' || typeof json.b === 'undefined') {
            throw new Error('Invalid JSON structure for LedColorRequest');
        }
        return new LedColorRequest(json.r, json.g, json.b);
    }
}