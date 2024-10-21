const TextUtils = (() => {
    // Metodo ausiliario per impostare i valori RGB nei campi di input
    const convertUnderscoreIntoSpace = (text) => {
        return text.replace(/_/g, ' ');
    };

    const convertSpaceIntoUnderscore = (text) => {
        return text.replace(/ /g, '_');
    };

    const fixTextNumber = (text) =>{
        console.log(text);
        if(text == ""){
            return '0';
        }
        if(text.length > 1 && text.at(0) == '0'){
            return text.substr(1);
        }
        return text;
    };

    const textToNumber = (text) => {
        const num = parseFloat(text);
        return (!isNaN(num) && isFinite(num)) ? num : 0;
    };

    return {
        convertUnderscoreIntoSpace,
        convertSpaceIntoUnderscore,
        fixTextNumber,
        textToNumber
    };
})();



// Exporta il modulo
export default TextUtils;