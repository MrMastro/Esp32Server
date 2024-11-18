import TextUtils from "./TextUtils.js";

// IpV4StringUtils.js
const IpV4StringUtils = (() => {

    //192.168.1.200

    //only for 3 octects for network and 1 octect for host
    const getNumberLastDigit = (adress) => {
        let digitAdress = adress;
        digitAdress = digitAdress.split(".");
        return TextUtils.textToIntegerNumber(digitAdress.at(digitAdress.length -1));
    }

    //only for 3 octects for network and 1 octect for host
    const getAdressWithIndexHost = (adress, indexHost) => {
        let result = "";
        let digitAdress = adress;
        digitAdress = digitAdress.split(".");
        digitAdress[digitAdress.length -1] = new String(indexHost);
        digitAdress.forEach( (digit3) => {
            result += digit3 + ".";
        });
        result = result.slice(0,-1);
        return result;
    }

    return {
        getNumberLastDigit,
        getAdressWithIndexHost
    };
})();

// Exporta il modulo
export default IpV4StringUtils;