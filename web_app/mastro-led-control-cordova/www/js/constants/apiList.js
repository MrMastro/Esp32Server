const ConstantApiList = (() => {
    // Funzione principale per inviare una richiesta POST
    const saveSettigsApi = "/api/saveSettings";
    const getSettingsApi = "/api/getSettings";
    const sendEffectLedApi ="/api/ws2811SetEffect";
    const sendStopEffectApi="/api/ws2811StopEffect";
    const saveInitialEffectApi="/api/saveInitialEffect";
    const getAvaibleEffects = "/api/getAvaibleEffects";
    const loginApi="/api/login";
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