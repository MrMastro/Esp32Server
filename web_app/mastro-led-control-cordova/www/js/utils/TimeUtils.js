// TimeUtils.js
const TimeUtils = (() => {
    // Funzione principale per inviare una richiesta POST
    const wait = async (ms) => {
        return new Promise(resolve => {
            setTimeout(resolve, ms);
        });
    };

    return {
        wait
    };
})();

// Exporta il modulo
export default TimeUtils;