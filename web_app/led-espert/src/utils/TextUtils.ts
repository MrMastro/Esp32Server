const TextUtils = (() => {
    // Metodo ausiliario per impostare i valori RGB nei campi di input
    const convertUnderscoreIntoSpace = (text: string) => {
        return text.replace(/_/g, ' ');
    };

    const convertSpaceIntoUnderscore = (text: string) => {
        return text.replace(/ /g, '_');
    };

    const fixTextNumber = (text: string) =>{
        console.log(text);
        if(text == ""){
            return '0';
        }
        if(text.length > 1 && text.charAt(0) == '0'){
            return text.substr(1);
        }
        return text;
    };

    const textToIntegerNumber = (text: string) => {
        const num = parseInt(text);
        return (!isNaN(num) && isFinite(num)) ? num : 0;
    };

    const isJSON = (str: string) => {
        try {
            JSON.stringify(JSON.parse(str));
            return true;
        } catch (e) {
            return false;
        }
    };

    return {
        convertUnderscoreIntoSpace,
        convertSpaceIntoUnderscore,
        fixTextNumber,
        textToIntegerNumber,
        isJSON
    };
})();



// Exporta il modulo
export default TextUtils;