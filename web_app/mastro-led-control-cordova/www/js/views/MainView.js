import DefaultConstants from "../constants/DefaultConstants.js";
import LedMainModel from "../models/LedMainModel.js";
import TextUtils from "../utils/TextUtils.js";
import Esp32ConnectionView from "./Esp32ConnectionView.js";
import GroupColorView from "./GroupColorView.js";

export default class MainView {
    constructor(rootElement, listEffects) { //empty cause modify html yet written
        if (!(rootElement instanceof HTMLElement)) {
            console.error('rootElement non è un elemento DOM valido', rootElement);
        }

        this.rootElement = rootElement;
        this.buttonWs2811SetEffect = {};
        this.buttonWs2811StopEffect = {};
        this.buttonUpdateEffect = {};
        this.saveInitialEffect = {};
        this.clearInitialEffect = {};
        this.labelIp = {};
        this.aPConnection = {};
        this.timingRangeInput = {};
        this.timingInput = {};

        this.handlerButtonWs2811SetEffect = {};
        this.handlerButtonWs2811StopEffect = {};
        this.handleButtonUpdateEffect = {};
        this.handlerSaveInitialEffect = {};
        this.handlerClearInitialEffect = {};
        this.handlerFieldIpInput = {};
        this.handlerAPConnectionSwitch = {};
        this.handelrRangeInputChange = {};
        this.handelrInputChange = {};

        this.groupColorMainView = {};

        this.render(new LedMainModel(), listEffects);
    }

    render(ledMainModel, listEffects) {
        this.rootElement.innerHTML = (`
            
                    <div id="ContainAction-4" class="d-lg-flex justify-content-lg-center ContainAction">
                        <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label form-label-title" style="padding-top: 0px;padding-right: 0px;padding-bottom: 0px;padding-left: 0px;">Effetto</label><svg class="bi bi-arrow-clockwise fs-4 border rounded-circle buttonUpdateEffect btn btn-dark" xmlns="http://www.w3.org/2000/svg" width="1em" height="1em" fill="currentColor" viewBox="0 0 16 16" data-bs-toggle="tooltip" data-bss-tooltip style="padding-top: 0px;padding-right: 0px;padding-bottom: 0px;padding-left: 0px;" title="Aggiorna effetti">
                                <path fill-rule="evenodd" d="M8 3a5 5 0 1 0 4.546 2.914.5.5 0 0 1 .908-.417A6 6 0 1 1 8 2z"></path>
                                <path d="M8 4.466V.534a.25.25 0 0 1 .41-.192l2.36 1.966c.12.1.12.284 0 .384L8.41 4.658A.25.25 0 0 1 8 4.466"></path>
                            </svg>
                        </div>
                        <div class="d-lg-flex flex-column justify-content-center align-items-center align-items-sm-center align-items-md-center align-items-lg-center align-items-xl-center align-items-xxl-center containerOfComponent"><select class="border rounded-pill d-flex effectInput form-control" style="max-width: 250px;font-size: 11px;text-align: center;" value="effect" name="effect select" required>
                                <option value="PROGRESSIVE_BAR_UNIQUE_COLOR" selected>PROGRESSIVE BAR UNIQUE COLOR</option>
                                <option value="CONSTANT_UNIQUE_COLOR">CONSTANT UNIQUE COLOR</option>
                            </select></div>
                    </div>

                    <div id="ContainAction-3" class="ContainAction">
                        <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label componentForm form-label-title">Tempo (ms)</label></div>
                        <div class="d-flex flex-column containerOfComponent" style="overflow: hidden;">
                            <div class="border rounded-pill d-flex d-md-flex d-xxl-flex justify-content-center align-items-center align-content-center justify-content-md-center justify-content-xxl-center componentForm rangeForm dark" value="timing" name="timing" min="0" max="1000" step="1" title="Value" data-bs-theme="dark" style="padding: 0px;padding-right: 12px;padding-left: 12px;padding-top: 10px;width: auto;--bs-body-bg: #2b3035;background: var(--bs-body-bg);"><input class="form-range border-1 border-secondary focus-ring focus-ring-secondary d-flex align-content-center align-self-center justify-content-md-center componentForm rangeForm timingRangeInput" type="range" value="500" name="timingSlide" min="50" max="1000" step="1" title="Value" data-bs-theme="dark" style="max-width: 420px;" required />
                                <div class="d-flex flex-row justify-content-center align-items-center"><input class="form-control-sm d-md-flex justify-content-md-center timingInput" type="number" name="timing" min="50" max="1000" step="1" style="max-width: 70px;width: auto;margin-bottom: 10px;margin-right: 5px;margin-left: 5px;" value="500" /><label class="form-label" style="margin-right: 5px;margin-left: 2px;">ms</label></div>
                            </div>
                        </div>
                    </div>

                    <div id="ContainAction-2" class="ContainAction">
                        <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label form-label-title">Colore</label></div>
                        <div class="containerOfComponent">
                            <div id="GroupColorViewMainContainer" class="d-flex flex-row justify-content-center align-items-center"></div>
                            <!--Generated by GroupColorView passing id = GroupColorViewMainContainer-->
                        </div>
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

                    <div id="ContainAction-5" class="d-lg-flex justify-content-center align-items-center">
                        <div class="dropdown d-inline btn-black btn-dark"><button class="btn btn-dark dropdown-toggle btn-black btn-primary dark" aria-expanded="false" data-bs-toggle="dropdown" type="button">Gestisci Effetto Iniziale</button>
                            <div class="dropdown-menu"><button class="btn btn-dark fs-5 saveInitialEffect" type="button">SALVA EFFETTO INIZIALE</button><button class="btn btn-dark fs-5 clearInitialEffect" type="button">RIMUOVI EFFETTO INIZIALE</button></div>
                        </div>
                    </div>

                    <div class="d-lg-flex justify-content-lg-center align-items-lg-center containerExecutiveLed">
                        <div id="ContainAction-6" class="d-flex d-lg-flex flex-column justify-content-center align-items-center">
                            <button class="btn btn-dark fs-5 py-2 px-4 mb-3 buttonForm buttonWs2811SetEffect" type="button">INVIA INIZIO EFFETTO</button>
                            <button class="btn btn-dark fs-5 py-2 px-4 mb-3 buttonForm buttonWs2811StopEffect" type="button">INVIA FINE EFFETTO</button>
                        </div>
                    </div>
        `);

        this.selectInputEffect = document.querySelector('.effectInput');
        this.cleanOptionEffects();
        this.buttonWs2811SetEffect = document.querySelector('.buttonWs2811SetEffect');
        this.buttonWs2811StopEffect = document.querySelector('.buttonWs2811StopEffect');
        this.buttonUpdateEffect = document.querySelector('.buttonUpdateEffect');
        this.saveInitialEffectBtn = document.querySelector('.saveInitialEffect');
        this.clearInitialEffectBtn = document.querySelector('.clearInitialEffect');
        // this.aPConnection = document.getElementById('APConnection');
        this.timingRangeInput = document.querySelector(".timingRangeInput");
        this.timingInput = document.querySelector(".timingInput");
        // document.querySelector('.fieldIp').value = ledMainModel.textFieldActualHost;
        document.querySelector('.effectInput').value = ledMainModel.effect;
        document.querySelector(".timingRangeInput").value = ledMainModel.deltaT;
        document.querySelector('.timingInput').value = ledMainModel.deltaT;
        this.groupColorMainView = new GroupColorView(document.querySelector('#GroupColorViewMainContainer'), "effectRequestColor", false);
        this.groupColorMainView.render(ledMainModel.colors, false, true);
        document.querySelector('#rgbCheck').checked = ledMainModel.rgbCheck;
        document.querySelector('#ws2811Check').checked = ledMainModel.ws2811Check;
        if (Array.isArray(listEffects)) {
            if (listEffects.length > 0) {
                this.createOptionEffects(listEffects);
            }
        }


        //Reassign handler
        this.reassignHandler();

    }

    getFieldIp() {
        return document.querySelector('.fieldIp').value;
    }

    getTimingRangeInput() {
        return this.timingRangeInput.value;
    }

    setTimingRangeInput(value) {
        this.timingRangeInput.value = value;
    }

    getTimingInput() {
        return this.timingInput.value;
    }

    setTimingInput(value) {
        this.timingInput.value = value;
    }

    isAPconnection() {
        return document.querySelector('#APConnection').checked;
    }


    getLedMainModel() {
        let colors = this.groupColorMainView.getColors();
        let rgbCheck = document.querySelector('#rgbCheck').checked;
        let ws2811Check = document.querySelector('#ws2811Check').checked;
        let effect = document.querySelector('.effectInput').value;
        let textFieldActualHost = this.getFieldIp();
        return new LedMainModel(this.aPConnection.value, textFieldActualHost, effect, this.getTimingInput(), colors, rgbCheck, ws2811Check)
    }

    cleanOptionEffects() {
        this.selectInputEffect.innerHTML = '';
    }

    createOptionEffects(effects) {
        // Crea e aggiungi le nuove opzioni
        effects.forEach((e, index) => {
            const option = document.createElement('option');
            option.value = e.name;
            option.minColorsNumber = e.minColorsNumber;
            option.maxColorsNumber = e.maxColorsNumber;
            option.textContent = TextUtils.convertUnderscoreIntoSpace(e.name);
            if (index === 0) {
                option.selected = true; // Imposta l'opzione come selezionata
            }
            this.selectInputEffect.appendChild(option);
        });

        this.selectInputEffect.addEventListener('change', (e) => {
            const selectedOption = e.target.options[e.target.selectedIndex];
            this.showMinColorsAndSetBound(selectedOption.minColorsNumber, selectedOption.maxColorsNumber);
        })
    }

    showMinColorsAndSetBound(min, max) {
        this.groupColorMainView.hideAllColor();
        this.groupColorMainView.setBound(min, max);
        this.groupColorMainView.showColors(min);
    }

    reassignHandler() {
        if (typeof this.handlerButtonWs2811SetEffect === 'function') {
            this.bindBtnWs2811SetEffect(this.handlerButtonWs2811SetEffect);
        }

        if (typeof this.handlerButtonWs2811StopEffect === 'function') {
            this.bindBtnStopEffect(this.handlerButtonWs2811StopEffect);
        }

        if (typeof this.handleButtonUpdateEffect === 'function') {
            this.bindBtnUpdateEffect(this.handleButtonUpdateEffect);
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

        if (typeof this.handelrRangeInputChange === 'function') {
            this.bindRangeInputChange(this.handelrRangeInputChange);
        }

        if (typeof this.handelrInputChange === 'function') {
            this.bindInputChange(this.handelrInputChange);
        }


    }

    bindBtnWs2811SetEffect(handler) {
        this.handlerButtonWs2811SetEffect = handler;
        this.buttonWs2811SetEffect.addEventListener('click', this.handlerButtonWs2811SetEffect);
    }


    bindBtnStopEffect(handler) {
        this.handlerButtonWs2811StopEffect = handler;
        this.buttonWs2811StopEffect.addEventListener('click', this.handlerButtonWs2811StopEffect);
    }

    bindBtnUpdateEffect(handler) {
        this.handleButtonUpdateEffect = handler;
        this.buttonUpdateEffect.addEventListener('click', this.handleButtonUpdateEffect);
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

    bindRangeInputChange(handler) {
        this.handelrRangeInputChange = handler;
        this.timingRangeInput.addEventListener('input', this.handelrRangeInputChange);
    }

    bindInputChange(handler) {
        this.handelrInputChange = handler;
        this.timingInput.addEventListener('input', this.handelrInputChange.bind(this, false));
        this.timingInput.addEventListener('blur', this.handelrInputChange.bind(this, true));
    }


}

// <div class="form-check text-center d-flex d-lg-flex flex-column justify-content-center align-items-center align-content-center align-self-center flex-wrap justify-content-lg-center align-items-lg-center" style="min-width: auto;margin-right: 5px;margin-top: 2px;margin-left: 5px;/*text-align: center;*/padding: 0;width: auto;padding-top: 0;padding-right: 0;padding-bottom: 0;padding-left: 0;margin-bottom: 2px;" data-bs-theme="dark">
// <input id="APConnection" class="form-check-input" type="checkbox" style="margin-left: 0px;width: 30px;height: 30px;" name="APConnection" checked /><label class="form-check-label text-center d-flex flex-fill justify-content-center align-items-center" for="rgbCheck-1" style="position: relative;margin-right: 0px;margin-left: 0px;text-align: center;max-width: initial;min-width: 75px;">Connessione AP</label>
// </div>
// <input class="fieldIp" type="text" minlength="7" maxlength="15" size="15" pattern="^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$" />
// </div>
// <small class="label-ip">Indirizzo attuale: </small>