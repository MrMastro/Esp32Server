// colorUtils.js
const ColorUtils = (() => {
    // Metodo ausiliario per impostare i valori RGB nei campi di input
    const setRgbInput = (r, g, b) => {
        const rgbHex = rgbToHex(r, g, b);
        const inputElement = document.querySelector('#initialRgbInput');
        if (inputElement) {
            inputElement.value = rgbHex;
        } else {
            console.warn("Input element with ID 'initialRgbInput' not found.");
        }
    };

    // Metodo ausiliario per convertire valori RGB in formato esadecimale
    const rgbToHex = (r, g, b) => {
        // Funzione interna per limitare i valori a 0-255 e convertirli in esadecimale
        const clampAndToHex = (c) => {
            // Assicurati che il valore sia un numero e lo limiti tra 0 e 255
            const clampedValue = Math.max(0, Math.min(255, Number(c) || 0));
            return clampedValue.toString(16).padStart(2, '0');
        };

        return `#${clampAndToHex(r)}${clampAndToHex(g)}${clampAndToHex(b)}`;
    };

    const hexToRgb = (hex) => {
        // Expand shorthand form (e.g. "03F") to full form (e.g. "0033FF")
        var shorthandRegex = /^#?([a-f\d])([a-f\d])([a-f\d])$/i;
        hex = hex.replace(shorthandRegex, function (m, r, g, b) {
            return r + r + g + g + b + b;
        });

        var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
        return result ? {
            r: parseInt(result[1], 16),
            g: parseInt(result[2], 16),
            b: parseInt(result[3], 16)
        } : null;
    }

    return {
        setRgbInput,
        rgbToHex,
        hexToRgb
    };
})();



// Exporta il modulo
export default ColorUtils;