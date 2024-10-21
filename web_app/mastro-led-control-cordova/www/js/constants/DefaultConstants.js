const DefaultConstants = (() => {

    const defaultEffects = [
        {
            "name": "CONSTANT_UNIQUE_COLOR",
            "minColorsNumber": 1,
            "maxColorsNumber": 1
        }];

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