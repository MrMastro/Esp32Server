const ConstantApiList = (() => {
    // Funzione principale per inviare una richiesta POST
    const saveSettigsApi =      "/api/v1/saveSettings";
    const getSettingsApi =      "/api/v1/getSettings";
    const sendEffectLedApi =    "/api/v2/ws2811SetEffect";
    const sendStopEffectApi=    "/api/v2/ws2811StopEffect";
    const saveInitialEffectApi= "/api/v1/saveInitialEffect";
    const getAvaibleEffects =   "/api/v1/getAvaibleEffects";
    const loginApi =            "/api/v1/login";
    const getInfoEsp32Hello =   "/api/v1/getInfoHello";
    const timeoutMs = 3;
    const timeoutForSearchtMs = 1;
    
    return {
        saveSettigsApi,
        getSettingsApi,
        sendEffectLedApi,
        sendStopEffectApi,
        saveInitialEffectApi,
        loginApi,
        getInfoEsp32Hello,
        getAvaibleEffects,
        timeoutMs,
        timeoutForSearchtMs
    };
})();

export default ConstantApiList;