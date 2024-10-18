import ColorUtils from '../utils/ColorUtils.js';

export default class SettingView {
    constructor(rootElement) { //empty cause modify html yet written
        if (!(rootElement instanceof HTMLElement)) {
            console.error('rootElement non è un elemento DOM valido', rootElement);
        }

        this.modal = rootElement;
        this.saveSettingsBtn = {};

        this.handlerSaveSettingsBtn = {};

        this.render();
        this.deviceNameInput = document.querySelector('#deviceNameInput');
        this.devicePasswordInput = document.querySelector('#devicePasswordInput');
        this.communicationModeInput = document.querySelector('#communicationModeInput');
        this.debugInput = document.querySelector('#debugInput');
        this.ssidAPInput = document.querySelector('#ssidAPInput');
        this.passwordAPInput = document.querySelector('#passwordAPInput');
        this.ssidWIFIInput = document.querySelector('#ssidWIFIInput');
        this.passwordWIFIInput = document.querySelector('#passwordWIFIInput');
        this.initialEffectInput = document.querySelector('#initialEffectInput');
    }

    render(settings) {
        this.modal.innerHTML = (`
            <div class="modal-dialog" role="document">
                <div class="modal-content">
                    <div class="modal-header d-flex flex-column justify-content-xl-center align-items-xl-center" style="text-align: center;display: flex;position: relative;"><button class="btn-close" type="button" aria-label="Close" data-bs-dismiss="modal"></button>
                        <h1 class="modal-title d-flex d-xl-flex justify-content-xl-center align-items-xl-center">Impostazioni</h1>
                    </div>
                    <div class="modal-body d-xl-flex flex-column align-self-stretch justify-content-xl-center align-items-xl-center">
                        <div id="Settings-Card-General" class="card" style="margin-bottom: 10px;margin-top: 10px;min-width: initial;max-width: initial;">
                            <h2>Impostazioni Dispositivo</h2>
                            <div class="card-body">
                                <div id="Settings-Panel-General" class="d-flex flex-column align-items-xl-center" style="margin-top: 10px;margin-bottom: 10px;">
                                    <h4>Impostazioni Principali</h4>
                                    <div id="Settings0" class="d-xl-flex flex-row justify-content-xl-center align-items-xl-center ContainAction">
                                        <div class="container d-xl-flex containerLabel" style="height: auto;max-height: 25px;"><label class="form-label form-label-title" style="font-size: 12px;text-align: left;min-width: 90px;">Nome Dispositivo</label></div>
                                        <div class="text-center d-xl-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent">
                                            <div class="text-center d-xl-flex justify-content-xl-center align-items-xl-center"><input id="deviceNameInput" type="text" style="width: 175px;" /></div>
                                        </div>
                                    </div>
                                    <div id="Settings1" class="d-xl-flex flex-row justify-content-xl-center align-items-xl-center ContainAction">
                                        <div class="container d-xl-flex containerLabel" style="height: auto;max-height: 25px;"><label class="form-label text-center form-label-title" style="font-size: 12px;text-align: left;min-width: 90px;">Password Dispositivo</label></div>
                                        <div class="text-center d-xl-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent">
                                            <div class="text-center d-xl-flex justify-content-xl-center align-items-xl-center"><input id="devicePasswordInput" type="password" style="width: 175px;" /></div>
                                        </div>
                                    </div>
                                    <div id="Settings2" class="d-xl-flex flex-row justify-content-xl-center align-items-xl-center ContainAction">
                                        <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label text-center form-label-title" style="font-size: 12px;text-align: left;min-width: 90px;">Tipologia di comunicazione</label></div>
                                        <div class="text-center d-xl-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent">
                                            <div class="text-center d-xl-flex justify-content-xl-center align-items-xl-center"><select id="communicationModeInput" class="border rounded-pill form-select-sm d-flex justify-content-center" style="font-size: 11px;text-align: center;width: 175px;" value name="comunicationMode" required>
                                                    <option value="NONE" selected></option>
                                                    <option value="AP_MODE">Access Point</option>
                                                    <option value="WIFI_MODE">WiFi</option>
                                                    <option value="BLUETOOTH_MODE">Bluetooth</option>
                                                </select></div>
                                        </div>
                                    </div>
                                    <div id="Settings2-1" class="d-xl-flex flex-row justify-content-xl-center align-items-xl-center ContainAction">
                                        <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label text-center form-label-title" style="font-size: 12px;text-align: left;min-width: 90px;">Modalità Debug</label></div>
                                        <div class="text-center d-xl-flex flex-column align-items-xl-center containerOfComponent" style="width: 175px;height: 40px;">
                                            <div class="text-center d-xl-flex justify-content-xl-center align-items-xl-center"><input id="debugInput" class="d-xl-flex justify-content-xl-center align-items-xl-start" type="checkbox" style="width: 30px;height: 30px;" /></div>
                                        </div>
                                    </div>
                                </div>
                                <div id="Settings-Panel-ApMode" class="d-flex flex-column" style="margin-top: 10px;margin-bottom: 10px;">
                                    <h4>Impostazioni Connessione AP</h4>
                                    <div id="Settings2-2" class="d-xl-flex flex-row justify-content-xl-center align-items-xl-center ContainAction">
                                        <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label text-center form-label-title" style="font-size: 12px;text-align: left;min-width: 90px;">Nome Access Point</label></div>
                                        <div class="text-center d-xl-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent">
                                            <div class="text-center d-xl-flex justify-content-xl-center align-items-xl-center"><input id="ssidAPInput" type="text" style="width: 175px;" /></div>
                                        </div>
                                    </div>
                                    <div id="Settings2-3" class="d-xl-flex flex-row justify-content-xl-center align-items-xl-center ContainAction">
                                        <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label text-center form-label-title" style="font-size: 12px;text-align: left;min-width: 90px;">Password Access Point</label></div>
                                        <div class="text-center d-xl-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent">
                                            <div class="text-center d-xl-flex justify-content-xl-center align-items-xl-center"><input id="passwordAPInput" type="password" style="width: 175px;" /></div>
                                        </div>
                                    </div>
                                </div>
                                <div id="Settings-Panel-WifiMode" class="d-flex flex-column" style="margin-top: 10px;margin-bottom: 10px;">
                                    <h4>Impostazioni Connessione WIFI</h4>
                                    <div id="Settings1-2" class="d-xl-flex flex-row justify-content-xl-center align-items-xl-center ContainAction">
                                        <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label text-center form-label-title" style="font-size: 12px;text-align: left;width: 90px;min-width: 90px;">Nome rete</label></div>
                                        <div class="text-center d-xl-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent">
                                            <div class="text-center d-xl-flex justify-content-xl-center align-items-xl-center"><input id="ssidWIFIInput" type="text" style="width: 175px;" /></div>
                                        </div>
                                    </div>
                                    <div id="Settings2-4" class="d-xl-flex flex-row justify-content-xl-center align-items-xl-center ContainAction">
                                        <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label text-center form-label-title" style="font-size: 12px;text-align: left;min-width: 90px;">Password rete</label></div>
                                        <div class="text-center d-xl-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent">
                                            <div class="text-center d-xl-flex justify-content-xl-center align-items-xl-center"><input id="passwordWIFIInput" type="password" style="width: 175px;" /></div>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>
                        <div id="Settings-Card-Leds" class="card" style="margin-top: 10px;margin-bottom: 10px;min-width: initial;max-width: initial;">
                            <h2>Impostazioni Luci Led</h2>
                            <div class="card-body" style="margin-top: 5px;margin-bottom: 5px;">
                                <div id="Settings-Panel-LedService" style="margin-top: 10px;margin-bottom: 10px;">
                                    <h4>Impostazioni Strisce led</h4>
                                    <div id="Settings-Panel-RGB" class="d-flex d-xl-flex flex-row justify-content-center align-items-center justify-content-xl-center align-items-xl-center ContainAction" style="height: 95px;">
                                        <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label text-center form-label-title" style="font-size: 12px;text-align: left;min-width: initial;">Striscia RGB</label></div>
                                        <div id="Settings-panel-RGB" class="text-center d-xl-flex flex-row justify-content-xl-center align-items-xl-center containerOfComponent">
                                            <div class="d-flex d-xl-flex flex-column flex-shrink-1 justify-content-center align-items-center m-auto justify-content-xl-end align-items-xl-center" style="width: 88px;height: 65px;margin: initial;margin-top: initial;margin-left: initial;margin-right: 0px;"><input id="enableStripRgbInput" class="d-xl-flex justify-content-xl-center" type="checkbox" style="width: 50px;height: 30px;" /><label class="form-label">Abilitato</label></div>
                                            <div class="d-flex d-xl-flex flex-column flex-shrink-1 justify-content-center align-items-center m-auto justify-content-xl-end align-items-xl-center" style="width: 88px;height: 65px;"><input id="pinLedCinRgbInput" class="d-xl-flex justify-content-xl-center" type="number" style="max-width: 55px;margin-right: 5px;margin-left: 5px;" /><label class="form-label" style="min-width: 72px;">Pin Cin</label></div>
                                            <div class="d-flex d-xl-flex flex-column flex-shrink-1 justify-content-center align-items-center m-auto justify-content-xl-end align-items-xl-center" style="width: 88px;height: 65px;margin: initial;margin-top: initial;margin-left: initial;margin-right: 0px;"><input id="pinLedDinRgbInput" class="d-xl-flex justify-content-xl-center" type="number" style="max-width: 53px;" /><label class="form-label">Pin Din</label></div>
                                        </div>
                                    </div>
                                    <div id="Settings-Panel-WS2811" class="d-flex d-xl-flex flex-row justify-content-center align-items-center justify-content-xl-center align-items-xl-center ContainAction" style="height: 95px;">
                                        <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label text-center form-label-title" style="font-size: 12px;text-align: left;min-width: initial;">Striscia RGB</label></div>
                                        <div id="Settings-panel-RGB-2" class="text-center d-xl-flex flex-row justify-content-xl-center align-items-xl-center containerOfComponent">
                                            <div class="d-flex d-xl-flex flex-column flex-shrink-1 justify-content-center align-items-center m-auto justify-content-xl-end align-items-xl-center" style="width: 88px;height: 65px;margin: initial;margin-top: initial;margin-left: initial;margin-right: 0px;"><input id="enableStripWs2811Input" class="d-xl-flex justify-content-xl-center" type="checkbox" style="width: 50px;height: 30px;" /><label class="form-label">Abilitato</label></div>
                                            <div class="d-flex d-xl-flex flex-column flex-shrink-1 justify-content-center align-items-center m-auto justify-content-xl-end align-items-xl-center" style="width: 88px;height: 65px;"><input id="numLedWs2811Input" class="d-xl-flex justify-content-xl-center" type="number" style="max-width: 55px;" /><label class="form-label" style="min-width: 72px;">Qt Pixel</label></div>
                                            <div class="d-flex d-xl-flex flex-column flex-shrink-1 justify-content-center align-items-center m-auto justify-content-xl-end align-items-xl-center" style="width: 88px;height: 65px;margin: initial;margin-top: initial;margin-left: initial;margin-right: 0px;"><input id="pinLedWs2811Input" class="d-xl-flex justify-content-xl-center" type="number" style="max-width: 53px;" /><label class="form-label">Pin Din</label></div>
                                        </div>
                                    </div>
                                </div>
                                <div id="Settings-Panel-InitialEffects" class="border rounded-0 d-flex flex-column visible" style="margin-top: 10px;margin-bottom: 10px;">
                                    <h4>Informazioni effetto iniziale</h4>
                                    <h6 class="text-center">(Effetto iniziale modificabile altrove)</h6>
                                    <div id="Settings2-8" class="d-xl-flex flex-row justify-content-xl-center align-items-xl-center ContainAction">
                                        <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label text-center form-label-title" style="font-size: 12px;text-align: left;min-width: 90px;">Effetto iniziale</label></div>
                                        <div class="text-center d-xl-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent">
                                            <div class="text-center d-xl-flex justify-content-xl-center align-items-xl-center"><input id="initialEffectInput" type="text" style="max-width: 150px;font-size: 11px;text-align: center;width: 150px;" value="effect" name="effect select" required readonly /></div>
                                        </div>
                                    </div>
                                    <div id="Settings0-2" class="d-xl-flex flex-row justify-content-xl-center align-items-xl-center ContainAction">
                                        <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label text-center form-label-title" style="font-size: 12px;text-align: left;min-width: 90px;">Tempo di animazione</label></div>
                                        <div class="text-center d-xl-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent">
                                            <div class="d-flex flex-row justify-content-xl-center"><label id="initialDeltaTInput" class="form-label d-xl-flex align-items-xl-end" style="margin-right: 5px;margin-left: 2px;"></label><label class="form-label d-xl-flex align-items-xl-end" style="margin-right: 5px;margin-left: 2px;">ms</label></div>
                                        </div>
                                    </div>
                                    <div id="Settings1-3" class="d-xl-flex flex-row justify-content-xl-center align-items-xl-center ContainAction">
                                        <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label text-center form-label-title" style="font-size: 12px;text-align: left;min-width: 90px;">Colore iniziale</label></div>
                                        <div class="text-center d-xl-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent">
                                            <div class="text-center d-xl-flex justify-content-xl-center align-items-xl-center"><input id="initialRgbInput" class="colorInput" type="color" style="width: 50px;height: 40px;" disabled /></div>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class="modal-footer"><button class="btn btn-light" type="button" data-bs-dismiss="modal">Annulla</button><button class="btn btn-primary saveSettingsBtn" type="button">Salva</button></div>
                </div>
            </div>
        `);

        this.saveSettingsBtn = document.querySelector('.saveSettingsBtn');

        if(this.handlerSaveSettingsBtn){
            this.saveSettingsBtn.addEventListener('click', this.handlerSaveSettingsBtn);
        }

        if (settings) {
            // Aggiorna i campi della modale con i dati ricevuti dal Controller
            document.querySelector('#deviceNameInput').value = settings.deviceName;
            document.querySelector('#devicePasswordInput').value = settings.devicePassword;
            document.querySelector('#communicationModeInput').value = settings.communicationMode;
            document.querySelector('#debugInput').checked = settings.debug;
            document.querySelector('#ssidAPInput').value = settings.ssidAP;
            document.querySelector('#passwordAPInput').value = settings.passwordAP;
            document.querySelector('#ssidWIFIInput').value = settings.ssidWIFI;
            document.querySelector('#passwordWIFIInput').value = settings.passwordWIFI;
            document.querySelector('#initialEffectInput').value = settings.initialEffect;
            document.querySelector('#initialDeltaTInput').innerText = settings.initialDeltaT;

            // Imposta i valori RGB iniziali
            ColorUtils.setRgbInput(settings.initialR, settings.initialG, settings.initialB);

            // Aggiorna le impostazioni delle luci LED
            document.querySelector('#enableStripRgbInput').checked = settings.ledSettings.enableStripRgb;
            document.querySelector('#pinLedCinRgbInput').value = settings.ledSettings.pinLedCinRgb;
            document.querySelector('#pinLedDinRgbInput').value = settings.ledSettings.pinLedDinRgb;
            document.querySelector('#enableStripWs2811Input').checked = settings.ledSettings.enableStripWs2811;
            document.querySelector('#numLedWs2811Input').value = settings.ledSettings.numLedWs2811;
            document.querySelector('#pinLedWs2811Input').value = settings.ledSettings.pinLedWs2811;
        }
    }

    bindBtnSaveSettings(handler){
        this.handlerSaveSettingsBtn = handler;
        this.saveSettingsBtn.addEventListener('click', this.handlerSaveSettingsBtn);
    }

    show() {
        $(this.modal).modal('show');
    }

    hide() {
        // Nascondi la modale
        $(this.modal).modal('hide');
    }
}