const TextUtils = (() => {
    // Metodo ausiliario per impostare i valori RGB nei campi di input
    const convertUnderscoreIntoSpace = (text) => {
        return text.replace(/_/g, ' ');
    };

    const convertSpaceIntoUnderscore = (text) => {
        return text.replace(/ /g, '_');
    };

    return {
        convertUnderscoreIntoSpace,
        convertSpaceIntoUnderscore
    };
})();



// Exporta il modulo
export default TextUtils;