const ConstantApiList = (() => {
    // Funzione principale per inviare una richiesta POST
    const saveSettigsApi = "/api/saveSettings";
    const getSettingsApi = "/api/getSettings";
    const sendEffectLedApi ="/api/ws2811SetEffect";
    const sendStopEffectApi="/api/ws2811StopEffect";
    const loginApi="/api/login";
    const timeoutMs = 5000;
    
    return {
        saveSettigsApi,
        getSettingsApi,
        sendEffectLedApi,
        sendStopEffectApi,
        loginApi,
        timeoutMs
    };
})();

export default ConstantApiList;