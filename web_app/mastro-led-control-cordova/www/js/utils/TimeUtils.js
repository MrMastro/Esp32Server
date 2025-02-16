// TimeUtils.js
const TimeUtils = (() => {
    // Funzione principale per inviare una richiesta POST
    const wait = async (ms) => {
        return new Promise(resolve => {
            setTimeout(resolve, ms);
        });
    };

    const processAndWait = async (callback, ...param) => {
        return new Promise(async (resolve) => {
            await callback(...param);
            resolve();
        });
    }

    return {
        wait,
        processAndWait
    };
})();

// Exporta il modulo
export default TimeUtils;