const HttpUtils = (() => {
    // Funzione principale per inviare una richiesta POST
    const saveSettigsApi = "/api/saveSettings";
    const getSettingsApi = "/api/getSettings";
    const sendEffectLedApi ="/api/ws2811SetEffect";
    const sendStopEffectApi="/api/ws2811StopEffect";
    const loginApi="/api/login";
    
    return {
        saveSettigsApi,
        getSettingsApi,
        sendEffectLedApi,
        sendStopEffectApi,
        loginApi
    };
})();

export default HttpUtils;