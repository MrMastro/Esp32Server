import SettingsModel from '../models/SettingModel.js';
import GroupColorView from './GroupColorView.js';

export default class SettingView {

    constructor(rootElement) { //empty cause modify html yet written
        if (!(rootElement instanceof HTMLElement)) {
            console.error('rootElement non è un elemento DOM valido', rootElement);
        }

        //modal
        this.settings = {};

        this.modal = rootElement;

        //element html
        this.deviceNameInput = {};
        this.devicePasswordInput = {};
        this.communicationModeInput = {};
        this.debugInput = {};
        this.ssidAPInput = {};
        this.passwordAPInput = {};
        this.ssidWIFIInput = {};
        this.passwordWIFIInput = {};
        this.initialEffectInput = {};
        this.initialDeltaTInput = {};
        this.groupColorSettingView = {};
        this.enableStripRgbInput = {};
        this.pinLedCinRgbInput = {};
        this.pinLedDinRgbInput = {};
        this.enableStripWs2811Input = {};
        this.numLedWs2811Input = {};
        this.pinLedWs2811Input = {};
        //element btn and handling
        this.saveSettingsBtn = {};
        this.handlerSaveSettingsBtn = {};
        this.groupColorSettingView = {};
        this.render();
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
                                    <div id="Settings-Panel-WS2811" class="d-flex d-xl-flex flex-row align-items-center justify-content-lg-center align-items-lg-center justify-content-xl-center align-items-xl-center ContainAction" style="height: 95px;">
                                        <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label text-center form-label-title" style="font-size: 12px;text-align: left;min-width: initial;">Striscia WS2811</label></div>
                                        <div id="Settings-panel-RGB-2" class="text-center d-flex d-lg-flex d-xl-flex flex-row justify-content-lg-center align-items-lg-center justify-content-xl-center align-items-xl-center containerOfComponent">
                                            <div class="d-flex d-xl-flex flex-column flex-shrink-1 justify-content-center align-items-center m-auto justify-content-lg-center align-items-lg-center justify-content-xl-end align-items-xl-center" style="width: 88px;height: 65px;margin: initial;margin-top: initial;margin-left: initial;margin-right: 0px;"><input id="enableStripWs2811Input" class="d-xl-flex justify-content-xl-center" type="checkbox" style="width: 50px;height: 30px;" /><label class="form-label">Abilitato</label></div>
                                            <div class="d-flex d-xl-flex flex-column flex-shrink-1 justify-content-center align-items-center m-auto justify-content-lg-center align-items-lg-center justify-content-xl-end align-items-xl-center" style="width: 88px;height: 65px;"><input id="numLedWs2811Input" class="d-xl-flex justify-content-xl-center" type="number" style="max-width: 55px;" /><label class="form-label" style="min-width: 72px;">Qt Pixel</label></div>
                                            <div class="d-flex d-xl-flex flex-column flex-shrink-1 justify-content-center align-items-center m-auto justify-content-lg-center align-items-lg-center justify-content-xl-end align-items-xl-center" style="width: 88px;height: 65px;margin: initial;margin-top: initial;margin-left: initial;margin-right: 0px;"><input id="pinLedWs2811Input" class="d-xl-flex justify-content-xl-center" type="number" style="max-width: 53px;" /><label class="form-label">Pin Din</label></div>
                                        </div>
                                    </div>
                                </div>
                                <div id="Settings-Panel-InitialEffects" class="border rounded-0 d-flex flex-column visible" style="margin-top: 10px;margin-bottom: 10px;">
                                    <h4>Informazioni effetto iniziale</h4>
                                    <h6 class="text-center">(Effetto iniziale modificabile altrove)</h6>
                                    <div id="Settings2-8" class="d-lg-flex d-xl-flex flex-row align-items-lg-center justify-content-xl-center align-items-xl-center ContainAction">
                                        <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label text-center form-label-title" style="font-size: 12px;text-align: left;min-width: 90px;">Effetto iniziale</label></div>
                                        <div class="text-center d-xl-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent">
                                            <div class="text-center d-xl-flex justify-content-xl-center align-items-xl-center"><input id="initialEffectInput" class="d-lg-flex justify-content-lg-center align-items-lg-center" type="text" style="max-width: 150px;font-size: 11px;text-align: center;width: 150px;" value="effect" name="effect select" required readonly /></div>
                                        </div>
                                    </div>
                                    <div id="Settings0-2" class="d-lg-flex d-xl-flex flex-row align-items-lg-center justify-content-xl-center align-items-xl-center ContainAction">
                                        <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label text-center form-label-title" style="font-size: 12px;text-align: left;min-width: 90px;">Tempo di animazione</label></div>
                                        <div class="text-center d-lg-flex d-xl-flex flex-row justify-content-lg-center align-items-lg-center justify-content-xl-center align-items-xl-center containerOfComponent"><label id="initialDeltaTInput" class="form-label d-lg-flex d-xl-flex align-items-lg-center align-items-xl-end" style="margin-right: 5px;margin-left: 2px;">0</label><label class="form-label d-lg-flex d-xl-flex align-items-lg-center align-items-xl-end" style="margin-right: 5px;margin-left: 2px;">ms</label></div>
                                    </div>
                                    <div id="Settings1-3" class="d-lg-flex d-xl-flex flex-row align-items-lg-center justify-content-xl-center align-items-xl-center ContainAction">
                                        <div class="container containerLabel" style="height: auto;max-height: 25px;"><label class="form-label text-center form-label-title" style="font-size: 12px;text-align: left;min-width: 90px;">Colore iniziale</label></div>
                                        <div class="text-center d-lg-flex d-xl-flex flex-row justify-content-lg-center align-items-lg-center justify-content-xl-center align-items-xl-center containerOfComponent">
                                            <div id="GroupColorViewSettingsContainer" class="d-flex flex-row justify-content-center align-items-center">
                                                <!--Generated by GroupColorView passing id = GroupColorViewSettingsContainer-->
                                            </div>
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

        this.deviceNameInput = document.querySelector('#deviceNameInput');
        this.devicePasswordInput = document.querySelector('#devicePasswordInput');
        this.communicationModeInput = document.querySelector('#communicationModeInput');
        this.debugInput = document.querySelector('#debugInput');
        this.ssidAPInput = document.querySelector('#ssidAPInput');
        this.passwordAPInput = document.querySelector('#passwordAPInput');
        this.ssidWIFIInput = document.querySelector('#ssidWIFIInput');
        this.passwordWIFIInput = document.querySelector('#passwordWIFIInput');
        this.initialEffectInput = document.querySelector('#initialEffectInput');
        this.initialDeltaTInput = document.querySelector('#initialDeltaTInput');
        this.enableStripRgbInput = document.querySelector('#enableStripRgbInput');
        this.pinLedCinRgbInput = document.querySelector('#pinLedCinRgbInput');
        this.pinLedDinRgbInput = document.querySelector('#pinLedDinRgbInput');
        this.enableStripWs2811Input = document.querySelector('#enableStripWs2811Input');
        this.numLedWs2811Input = document.querySelector('#numLedWs2811Input');
        this.pinLedWs2811Input = document.querySelector('#pinLedWs2811Input');

        this.saveSettingsBtn = document.querySelector('.saveSettingsBtn');

        if(this.handlerSaveSettingsBtn){
            this.saveSettingsBtn.addEventListener('click', this.handlerSaveSettingsBtn);
        }

        if (settings) {
            this.settings = settings;
            // Aggiorna i campi della modale con i dati ricevuti dal Controller
            this.deviceNameInput.value = settings.deviceName;
            this.devicePasswordInput.value = settings.devicePassword;
            this.communicationModeInput.value = settings.communicationMode;
            this.debugInput.checked = settings.debug;
            this.ssidAPInput.value = settings.ssidAP;
            this.passwordAPInput.value = settings.passwordAP;
            this.ssidWIFIInput.value = settings.ssidWIFI;
            this.passwordWIFIInput.value = settings.passwordWIFI;
            this.initialEffectInput.value = settings.initialEffect;
            this.initialDeltaTInput.innerText = settings.initialDeltaT;
            this.groupColorSettingView = new GroupColorView(document.querySelector('#GroupColorViewSettingsContainer'), "settingColor", true);
            this.groupColorSettingView.render(settings.initialColors);

            // Aggiorna le impostazioni delle luci LED
            this.enableStripRgbInput.checked = settings.ledSettings.enableStripRgb
            this.pinLedCinRgbInput.value = settings.ledSettings.pinLedCinRgb;
            this.pinLedDinRgbInput.value = settings.ledSettings.pinLedDinRgb;
            this.enableStripWs2811Input.checked = settings.ledSettings.enableStripWs2811;
            this.numLedWs2811Input.value = settings.ledSettings.numLedWs2811;
            this.pinLedWs2811Input.value = settings.ledSettings.pinLedWs2811;

            // document.querySelector('#enableStripRgbInput').checked = settings.ledSettings.enableStripRgb;
            // document.querySelector('#pinLedCinRgbInput').value = settings.ledSettings.pinLedCinRgb;
            // document.querySelector('#pinLedDinRgbInput').value = settings.ledSettings.pinLedDinRgb;
            // document.querySelector('#enableStripWs2811Input').checked = settings.ledSettings.enableStripWs2811;
            // document.querySelector('#numLedWs2811Input').value = settings.ledSettings.numLedWs2811;
            // document.querySelector('#pinLedWs2811Input').value = settings.ledSettings.pinLedWs2811;
        }
    }

    async getSettings() {

        let colors = this.groupColorSettingView.getColors();
        let model = new SettingsModel(
            document.querySelector('#deviceNameInput').value,
            document.querySelector('#devicePasswordInput').value,
            document.querySelector('#communicationModeInput').value,
            document.querySelector('#debugInput').checked,
            document.querySelector('#initialEffectInput').value,
            document.querySelector('#initialDeltaTInput').innerText,
            colors,
            document.querySelector('#ssidAPInput').value,
            document.querySelector('#passwordAPInput').value,
            document.querySelector('#ssidWIFIInput').value,
            document.querySelector('#passwordWIFIInput').value,
            document.querySelector('#enableStripRgbInput').checked,
            document.querySelector('#pinLedDinRgbInput').value,
            document.querySelector('#pinLedCinRgbInput').value,
            document.querySelector('#enableStripWs2811Input').checked,
            document.querySelector('#numLedWs2811Input').value,
            document.querySelector('#pinLedWs2811Input').value
        );
        return model;
    }

    bindBtnSaveSettings(handler){
        this.handlerSaveSettingsBtn = handler;
        this.saveSettingsBtn.addEventListener('click', this.handlerSaveSettingsBtn);
    }

    show() {
        $(this.modal).modal('show');
        this.render(this.settings);
    }

    hide() {
        // Nascondi la modale
        $(this.modal).modal('hide');
    }
}