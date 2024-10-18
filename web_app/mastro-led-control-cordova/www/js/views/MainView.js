import DefaultConstants from "../constants/DefaultConstants.js";
import LedMainModel from "../models/LedMainModel.js";
import TextUtils from "../utils/TextUtils.js";

export default class MainView {
    constructor(rootElement, listEffects) { //empty cause modify html yet written
        if (!(rootElement instanceof HTMLElement)) {
            console.error('rootElement non è un elemento DOM valido', rootElement);
        }

        this.rootElement = rootElement;
        this.buttonWs2811SetEffect = {};
        this.buttonWs2811StopEffect = {};
        this.saveInitialEffect = {};
        this.clearInitialEffect = {};
        this.fieldIp = {};
        this.labelIp = {};
        this.aPConnection = {};

        this.handlerButtonWs2811SetEffect = {};
        this.handlerButtonWs2811StopEffect = {};
        this.handlerSaveInitialEffect = {};
        this.handlerClearInitialEffect = {};
        this.handlerFieldIpInput = {};
        this.handlerAPConnectionSwitch = {};

        this.render(new LedMainModel(), listEffects);
    }

    render(ledMainModel, listEffects) {
        this.rootElement.innerHTML = (`
            <div class="text-white bg-primary border rounded border-0 p-4 py-5" style="margin-right: 10px;margin-left: 10px;margin-top: 0;margin-bottom: 10px;background: rgb(49,94,197);">
                <div>
                    <div class="d-lg-flex containerSetLedEffect"><label class="form-label form-label-title">Indirizzo IP</label>
                        <div id="ContainAction-0" class="ContainAction">
                            <div class="border rounded-pill d-flex d-md-flex d-xxl-flex flex-column justify-content-center align-items-center align-content-center justify-content-md-center justify-content-xxl-center componentForm rangeForm dark" value="timing" name="timing" min="0" max="1000" step="1" title="Value" data-bs-theme="dark" style="padding: 0px;padding-right: 12px;padding-left: 12px;padding-top: 10px;width: auto;--bs-body-bg: #2b3035;background: var(--bs-body-bg);">
                                <div class="text-center d-xl-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent dark">
                                    <div class="text-center d-flex d-xl-flex flex-column justify-content-xl-center align-items-xl-center">
                                        <div class="form-check text-center d-flex d-lg-flex flex-column justify-content-center align-items-center align-content-center align-self-center flex-wrap justify-content-lg-center align-items-lg-center" style="min-width: auto;margin-right: 5px;margin-top: 2px;margin-left: 5px;/*text-align: center;*/padding: 0;width: auto;padding-top: 0;padding-right: 0;padding-bottom: 0;padding-left: 0;margin-bottom: 2px;" data-bs-theme="dark"><input id="APConnection" class="form-check-input" type="checkbox" style="margin-left: 0px;width: 30px;height: 30px;" name="APConnection" checked /><label class="form-check-label text-center d-flex flex-fill justify-content-center align-items-center" for="rgbCheck-1" style="position: relative;margin-right: 0px;margin-left: 0px;text-align: center;max-width: initial;min-width: 75px;">Connessione AP</label></div><input class="fieldIp" type="text" minlength="7" maxlength="15" size="15" pattern="^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$" />
                                    </div><small class="label-ip">Indirizzo attuale: </small>
                                </div>
                            </div>
                        </div>
                        <div id="ContainAction-4" class="ContainAction">
                            <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label form-label-title">Effetto</label></div>
                            <div class="containerOfComponent"><select class="border rounded-pill d-flex justify-content-center effectInput form-control" style="max-width: 250px;font-size: 11px;text-align: center;" value="effect" name="effect select" required>
                                        <option value="PROGRESSIVE_BAR_UNIQUE_COLOR" selected>PROGRESSIVE BAR UNIQUE COLOR</option>
                                        <option value="CONSTANT_UNIQUE_COLOR">CONSTANT UNIQUE COLOR</option>
                                </select></div>
                        </div>
                        <div id="ContainAction-3" class="ContainAction">
                            <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label componentForm form-label-title">Tempo (ms)</label></div>
                            <div class="d-flex flex-column containerOfComponent" style="overflow: hidden;">
                                <div class="border rounded-pill d-flex d-md-flex d-xxl-flex justify-content-center align-items-center align-content-center justify-content-md-center justify-content-xxl-center componentForm rangeForm dark" value="timing" name="timing" min="0" max="1000" step="1" title="Value" data-bs-theme="dark" style="padding: 0px;padding-right: 12px;padding-left: 12px;padding-top: 10px;width: auto;--bs-body-bg: #2b3035;background: var(--bs-body-bg);"><input class="form-range border-1 border-secondary focus-ring focus-ring-secondary d-flex align-content-center align-self-center justify-content-md-center componentForm rangeForm timingRangeInput" type="range" value="500" name="timingSlide" min="0" max="1000" step="1" title="Value" data-bs-theme="dark" style="max-width: 420px;" required />
                                    <div class="d-flex flex-row justify-content-center align-items-center"><input class="form-control-sm d-md-flex justify-content-md-center timingInput" type="number" name="timing" min="0" max="1000" step="1" style="max-width: 70px;width: auto;margin-bottom: 10px;margin-right: 5px;margin-left: 5px;" value="500" /><label class="form-label" style="margin-right: 5px;margin-left: 2px;">ms</label></div>
                                </div>
                            </div>
                        </div>
                        <div id="ContainAction-2" class="ContainAction">
                            <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label form-label-title">Colore</label></div>
                            <div class="containerOfComponent"><input class="colorInput form-control" type="color" style="width: 50px;height: 40px;" /></div>
                        </div>
                        <div id="ContainAction-1" class="ContainAction">
                            <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label componentForm form-label-title">Striscia led soggetta all&#39;effetto</label></div>
                            <div class="d-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent" style="overflow: hidden;">
                                <div class="text-center border rounded-pill d-flex d-md-flex d-xxl-flex flex-row justify-content-center align-items-center align-content-center align-self-center m-auto justify-content-lg-center justify-content-xxl-center componentForm rangeForm dark" value="timing" name="timing" min="0" max="1000" step="1" title="Value" data-bs-theme="dark" style="padding: 0px;padding-right: 12px;padding-left: 12px;padding-top: 10px;width: auto;--bs-body-bg: #2b3035;background: var(--bs-body-bg);min-width: 200px;" data-bs-target="#navcol-3">
                                    <div class="form-check text-center d-flex d-lg-flex flex-column justify-content-center align-items-center align-content-center align-self-center flex-wrap justify-content-lg-center align-items-lg-center" style="min-width: auto;margin-right: 5px;margin-top: 2px;margin-left: 5px;/*text-align: center;*/padding: 0;width: auto;padding-top: 0;padding-right: 0;padding-bottom: 0;padding-left: 0;margin-bottom: 2px;" data-bs-theme="dark"><input id="rgbCheck" class="form-check-input" type="checkbox" style="margin-left: 0px;width: 30px;height: 30px;" name="RGB" checked /><label class="form-check-label text-center d-flex flex-fill justify-content-center align-items-center" for="RGB" style="position: relative;margin-right: 0px;margin-left: 0px;text-align: center;max-width: initial;min-width: 75px;">RGB</label></div>
                                    <div class="form-check text-center d-flex d-lg-flex flex-column justify-content-center align-items-center align-content-center justify-content-lg-center align-items-lg-center" style="padding: 0;padding-left: 0;padding-right: 0;margin-right: 5px;padding-top: 0;margin-top: 2px;min-width: auto;margin-left: 5px;text-align: center;width: auto;padding-bottom: 0;margin-bottom: 2px;" data-bs-theme="dark"><input id="ws2811Check" class="form-check-input d-flex d-lg-flex justify-content-center align-items-center justify-content-lg-center align-items-lg-center" type="checkbox" style="margin-left: 0px;width: 30px;height: 30px;" name="WS2811" checked /><label class="form-check-label d-flex d-lg-flex flex-fill justify-content-center align-items-center align-self-center justify-content-lg-center align-items-lg-center" for="WS2811" style="margin-left: 0px;margin-right: 0px;position: relative;text-align: center;max-width: initial;min-width: 75px;">WS2811</label></div>
                                    <div class="d-flex flex-row justify-content-center align-items-center"></div>
                                </div>
                            </div>
                        </div>
                        <div id="ContainAction-5" class="d-lg-flex justify-content-center align-items-center"><button class="btn btn-dark fs-5 py-2 px-4 mb-3 saveInitialEffect" type="button">SALVA EFFETTO INIZIALE</button>
                            <div style="width: 20px;"></div><button class="btn btn-dark fs-5 py-2 px-4 mb-3 clearInitialEffect" type="button">RIMUOVI EFFETTO INIZIALE</button>
                        </div>
                    </div>
                    <div class="d-lg-flex justify-content-lg-center align-items-lg-center containerExecutiveLed">
                        <div id="ContainAction-6" class="d-flex d-lg-flex flex-column justify-content-center align-items-center"><button class="btn btn-dark fs-5 py-2 px-4 mb-3 buttonForm buttonWs2811SetEffect" type="button">INVIA INIZIO EFFETTO</button><button class="btn btn-dark fs-5 py-2 px-4 mb-3 buttonForm buttonWs2811StopEffect" type="button">INVIA FINE EFFETTO</button></div>
                    </div>
                    <div id="Character-div" class="character-container">
                        <div class="character"></div>
                    </div>
                </div>
            </div>
        `);

        this.selectInputEffect = document.querySelector('.effectInput');
        this.cleanOptionEffects();
        this.createOptionEffects(listEffects);
        this.buttonWs2811SetEffect = document.querySelector('.buttonWs2811SetEffect');
        this.buttonWs2811StopEffect = document.querySelector('.buttonWs2811StopEffect');
        this.saveInitialEffectBtn = document.querySelector('.saveInitialEffect');
        this.clearInitialEffectBtn = document.querySelector('.clearInitialEffect');
        this.fieldIp = document.querySelector('.fieldIp');
        this.labelIp = document.querySelector('.label-ip');
        this.aPConnection = document.getElementById('APConnection');

        if (ledMainModel.aPConnection) {
            this.hideFieldIp();
            document.querySelector('.fieldIp').value = DefaultConstants.defaultDebug;
        }

        document.querySelector('#APConnection').checked = ledMainModel.aPConnection;
        document.querySelector('.fieldIp').value = ledMainModel.actualHost;
        this.setLabelIp("Indirizzo attuale: " + ledMainModel.actualHost);
        //Effect //todo
        document.querySelector(".timingRangeInput").value = ledMainModel.deltaT;
        document.querySelector('.timingInput').value = ledMainModel.deltaT;
        document.querySelector('.colorInput').value = ledMainModel.color;
        document.querySelector('#rgbCheck').checked = ledMainModel.rgbCheck;
        document.querySelector('#ws2811Check').checked = ledMainModel.ws2811Check;

        //Reassign handler
        this.reassignHandler();

    }

    getFieldIp() {
        return document.querySelector('.fieldIp').value;
    }

    setLabelIp(value) {
        this.labelIp.textContent = value;
    }

    getLabelIp(){
        return this.labelIp.textContent;
    }

    isAPconnection() {
        return document.querySelector('#APConnection').checked;
    }

    hideFieldIp(){
        this.fieldIp.style.display = 'none';
    }

    showFieldIp() {
        this.fieldIp.style.display = 'inline';
    }

    cleanOptionEffects(){
        this.selectInputEffect.innerHTML = '';
    }

    createOptionEffects(effects) {
        // Crea e aggiungi le nuove opzioni
        effects.forEach( (e, index) => {
            const option = document.createElement('option');
            option.value = e;
            option.textContent = TextUtils.convertUnderscoreIntoSpace(e);
            if (index === 0) {
                option.selected = true; // Imposta l'opzione come selezionata
            }
            this.selectInputEffect.appendChild(option);
        });
    }

    reassignHandler() {
        if (typeof this.handlerButtonWs2811SetEffect === 'function') {
            this.bindbuttonWs2811SetEffect(this.handlerButtonWs2811SetEffect);
        }

        if (typeof this.handlerButtonWs2811StopEffect === 'function') {
            this.bindBtnStopEffect(this.handlerButtonWs2811StopEffect);
        }

        if (typeof this.handlerSaveInitialEffect === 'function') {
            this.bindBtnSaveInitialEffect(this.handlerSaveInitialEffect);
        }

        if (typeof this.handlerClearInitialEffect === 'function') {
            this.bindBtnClearInitialEffect(this.handlerClearInitialEffect);
        }

        if (typeof this.handlerFieldIpInput === 'function') {
            this.bindFieldIpInput(this.handlerFieldIpInput);
        }

        if (typeof this.handlerAPConnectionSwitch === 'function') {
            this.bindAPConnectionSwitch(this.handlerAPConnectionSwitch);
        }
    }

    bindBtnSetEffect(handler) {
        this.handlerButtonWs2811SetEffect = handler;
        this.buttonWs2811SetEffect.addEventListener('click', this.handlerButtonWs2811SetEffect);
    }

    bindbuttonWs2811SetEffect(handler) {
        this.handlerbuttonWs2811SetEffect = handler;
        this.buttonWs2811SetEffect.addEventListener('click', this.handlerSaveSettingsBtn);
    }


    bindBtnStopEffect(handler) {
        this.handlerButtonWs2811StopEffect = handler;
        this.buttonWs2811StopEffect.addEventListener('click', this.handlerButtonWs2811StopEffect);
    }

    bindBtnSaveInitialEffect(handler) {
        this.handlerSaveInitialEffect = handler;
        this.saveInitialEffectBtn.addEventListener('click', this.handlerSaveInitialEffect);
    }

    bindBtnClearInitialEffect(handler) {
        this.handlerClearInitialEffect = handler;
        this.clearInitialEffectBtn.addEventListener('click', this.handlerClearInitialEffect);
    }

    bindFieldIpInput(handler) {
        this.handlerFieldIpInput = handler;
        this.fieldIp.addEventListener('input', this.handlerFieldIpInput);
    }

    bindAPConnectionSwitch(handler) {
        this.handlerAPConnectionSwitch = handler;
        this.aPConnection.addEventListener('click', this.handlerAPConnectionSwitch);
    }

}