const DefaultConstants = (() => {

    const defaultEffects = ["PROGRESSIVE_BAR_UNIQUE_COLOR", "CONSTANT_UNIQUE_COLOR"];
    const defaultEffectOption = "PROGRESSIVE_BAR_UNIQUE_COLOR";
    const defaultApHost = "192.168.4.1";
    const defaultDebug = false;
    
    return {
        defaultEffects,
        defaultEffectOption,
        defaultApHost,
        defaultDebug
    };
})();

export default DefaultConstants;