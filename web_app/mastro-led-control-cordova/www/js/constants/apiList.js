const ConstantApiList = (() => {
    // Funzione principale per inviare una richiesta POST
    const saveSettigsApi =      "/api/v1/saveSettings";
    const getSettingsApi =      "/api/v1/getSettings";
    const sendEffectLedApi =    "/api/v2/ws2811SetEffect";
    const sendStopEffectApi=    "/api/v2/ws2811StopEffect";
    const saveInitialEffectApi= "/api/v1/saveInitialEffect";
    const getAvaibleEffects =   "/api/v1/getAvaibleEffects";
    const loginApi=             "/api/v1/login";
    const timeoutMs = 3;
    
    return {
        saveSettigsApi,
        getSettingsApi,
        sendEffectLedApi,
        sendStopEffectApi,
        saveInitialEffectApi,
        loginApi,
        getAvaibleEffects,
        timeoutMs
    };
})();

export default ConstantApiList;