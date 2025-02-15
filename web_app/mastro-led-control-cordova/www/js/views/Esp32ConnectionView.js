import { ConnectionInfo } from "../models/Esp32Model.js";
import DefaultConstants from "../constants/DefaultConstants.js";

export default class Esp32ConnectionView {
    constructor(rootElement, nameView="") {
        if (!(rootElement instanceof HTMLElement)) {
            console.error('rootElement non è un elemento DOM valido', rootElement);
        }
        this.rootElement = rootElement;
        
        this.nameView = nameView != "" ? nameView : "connectionGeneric_"+Date.now();

        this.buttonSearchEsp32 = {};

        this.buttonUpdateStatusEsp32 = {};

        this.buttonInfoEsp32List = [];

        this.handlerButtonSearchEsp32 = {};

        this.handlerBtnUpdateStatusEsp32 = {};

        this.arrayConnections = [];

        this.render();
    }

    // Metodo render per creare la struttura del pannello delle connession (WIP DA FARE)
    render(listOfConnection = [], message = "") { //todo
        let htmlList = "";
        let groupSearchIp = "";
        let debugMsg = (message != "" && DefaultConstants.debugApp) ? "<label class='labelWrong'>" + message + "</label>" : "";
        if (Array.isArray(listOfConnection)) {
            if (listOfConnection.length < 1) {
                htmlList = `    <label class="form-label">Nessun Dispositivo disponibile.<br/>Cerca dispositivi cliccando sulla lente di ingrandimento</label>
                                ${debugMsg}    
                            `;
            }else{
                htmlList = this.getEsp32ConnectionElementHtml(listOfConnection);
            }
        } else {
            htmlList =  `
                            <label class="form-label">Errore: Nessun Dispositivo disponibile.<br/>Cerca dispositivi cliccando sulla lente di ingrandimento</label>
                            ${debugMsg}
                        `;
        }

        groupSearchIp = this.getEsp32AdvancedSearch(true,"");

        // let button = <button class='searchEsp32'> Cerca dispositivi </button>
        // this.rootElement.innerHTML = button + htmlList;
        this.rootElement.innerHTML = (`

            <div class="container containerLabel" style="height: auto;max-height: 25px;">
                <label class="form-label form-label-title" style="padding-top: 0px;padding-right: 0px;padding-bottom: 0px;padding-left: 0px;">Connessioni Esp32</label>
                <svg class="bi bi-search fs-4 border rounded-circle buttonSearchEsp32 btn btn-dark" xmlns="http://www.w3.org/2000/svg" width="1em" height="1em" fill="currentColor" viewBox="0 0 16 16" data-bs-toggle="tooltip" data-bss-tooltip style="padding-top: 0px;padding-right: 0px;padding-bottom: 0px;padding-left: 0px;" title="Cerca dispositivi">
                    <path d="M11.742 10.344a6.5 6.5 0 1 0-1.397 1.398h-.001c.03.04.062.078.098.115l3.85 3.85a1 1 0 0 0 1.415-1.414l-3.85-3.85a1.007 1.007 0 0 0-.115-.1zM12 6.5a5.5 5.5 0 1 1-11 0 5.5 5.5 0 0 1 11 0"></path>
                </svg>

                <svg class="bi bi-arrow-clockwise fs-4 border rounded-circle updateSatusEsp32 btn btn-dark" xmlns="http://www.w3.org/2000/svg" width="1em" height="1em" fill="currentColor" viewBox="0 0 16 16" data-bs-toggle="tooltip" data-bss-tooltip style="padding-top: 0px;padding-right: 0px;padding-bottom: 0px;padding-left: 0px;" title="Aggiorna effetti">
                    <path fill-rule="evenodd" d="M8 3a5 5 0 1 0 4.546 2.914.5.5 0 0 1 .908-.417A6 6 0 1 1 8 2z"></path>
                    <path d="M8 4.466V.534a.25.25 0 0 1 .41-.192l2.36 1.966c.12.1.12.284 0 .384L8.41 4.658A.25.25 0 0 1 8 4.466"></path>
                </svg>
            </div>

            <div class="panelConnections d-flex dark justify-content-center" style="width: auto;--bs-body-bg: #2b3035;background: var(--bs-body-bg);min-width: auto;">
                <div class="d-flex d-lg-flex flex-column align-items-center justify-content-center align-items-center" style="margin: 10px;">
                    ${htmlList}
                    ${groupSearchIp}
                </div>
            </div>


        `);

        this.buttonInfoEsp32List = [];

        this.arrayConnections.forEach( (el) => {
            let htmlElement = document.querySelector(el.idHtmlElement);
            htmlElement.checked = el.espConnection.active;
            if(el.espConnection.connectionState == ConnectionInfo.OFFLINE){
                htmlElement.checked = false;
                htmlElement.disabled = true;
            }
            this.buttonInfoEsp32List.push(document.querySelector(el.idBtnElement));
        });

        this.buttonSearchEsp32 = document.querySelector('.buttonSearchEsp32');
        this.buttonUpdateStatusEsp32 = document.querySelector('.updateSatusEsp32');
        this.reassignHandler();
    }

    getEsp32AdvancedSearch(show=false, msgAlert="") {
        let html =``;
        this.arrayConnections = [];
        let nameView = this.nameView;

        if(show){
            html = `
                    <div class="d-flex flex-column justify-content-center">
                        <div class="border rounded d-flex justify-content-between align-items-center esp32PanelConnection" style="margin: 10px 10px 0px 0px;">
                        
                            <input id="inputIp_${nameView}" class="fieldIp" type="text" minlength="7" maxlength="15" size="15" pattern="^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$" />
                            
                            <svg id="idIcon_nameView-1" class="bi bi-search" xmlns="http://www.w3.org/2000/svg" width="1em" height="1em" fill="currentColor" viewBox="0 0 16 16" style="width: 25px;height: 25px;">
                                <path d="M11.742 10.344a6.5 6.5 0 1 0-1.397 1.398h-.001c.03.04.062.078.098.115l3.85 3.85a1 1 0 0 0 1.415-1.414l-3.85-3.85a1.007 1.007 0 0 0-.115-.1zM12 6.5a5.5 5.5 0 1 1-11 0 5.5 5.5 0 0 1 11 0"></path>
                            </svg>

                        </div>
                        <small class="label-ip labelIpWrong">Inserisci un indirizzo ip valido</small>
                    </div>
                    `;
        }

        this.fieldIp = document.querySelector('.fieldIp');
        this.labelIp = document.querySelector('.label-ip');

        return html;
    }

    reassignHandler() {
        if (typeof this.handlerBtnSearchEsp32 === 'function') {
            this.bindButtonSearchEsp32(this.handlerBtnSearchEsp32);
        }

        if (typeof this.handlerBtnUpdateStatusEsp32=== 'function') {
            this.bindButtonUpdateStatusEsp32(this.handlerBtnUpdateStatusEsp32);
        }
        this.bindButtonInfoEsp32(this.handlerBtnInfoEsp32)
    }

    bindButtonUpdateStatusEsp32(handler){
        this.handlerBtnUpdateStatusEsp32 = handler;
        this.buttonUpdateStatusEsp32.addEventListener('click', this.handlerBtnUpdateStatusEsp32);
    }

    bindButtonSearchEsp32(handler){
        this.handlerBtnSearchEsp32 = handler;
        this.buttonSearchEsp32.addEventListener('click', this.handlerBtnSearchEsp32);
    }

    bindButtonInfoEsp32(handler){
        if(typeof handler === 'function'){
            this.handlerBtnInfoEsp32 = handler;
            let list = this.arrayConnections;
            list.forEach((esp32, count) => {
                this.buttonInfoEsp32List.at(count).addEventListener('click', () => {
                this.handlerBtnInfoEsp32(esp32) 
                });
            });
        }
    }

    getEsp32ConnectionElementHtml(list) {
        let html =``;
        this.arrayConnections = [];
        list.forEach((esp32, count) => {
            let nameView = this.nameView;
            let idCheck = "checkEspConnection-" + count;
            let idIcon = "btnEspConnection-" + count;
            let statusIndicator = "statusIndicator-"+count;
            let deviceName = esp32.infoConnection.deviceName;
            let refCountEsp32 = count;
            let statusConnection = esp32.connectionState;
            
            html+= `
                <div class="esp32PanelConnection d-flex justify-content-between align-items-center esp32PanelConnection" style="">
                    
                    <input id="${idCheck}_${nameView}" class="elementEsp32" type="checkbox" style="width: 25px;height: 25px;" />

                    <span id="${statusIndicator}_${nameView}" class="ms-2 elementEsp32 indicator-${statusConnection}" style="width: 15px; height: 15px; border-radius: 50%; margin-right: 5px;"></span>
                    
                    <span class="text-start" style="width: 140px;">${deviceName}</span>

                    <svg id="${idIcon}_${nameView}" class="bi bi-tools border rounded-circle" xmlns="http://www.w3.org/2000/svg" width="1em" height="1em" fill="currentColor" viewBox="0 0 16 16" style="width: 25px;height: 25px;">
                        <path d="M1 0 0 1l2.2 3.081a1 1 0 0 0 .815.419h.07a1 1 0 0 1 .708.293l2.675 2.675-2.617 2.654A3.003 3.003 0 0 0 0 13a3 3 0 1 0 5.878-.851l2.654-2.617.968.968-.305.914a1 1 0 0 0 .242 1.023l3.27 3.27a.997.997 0 0 0 1.414 0l1.586-1.586a.997.997 0 0 0 0-1.414l-3.27-3.27a1 1 0 0 0-1.023-.242L10.5 9.5l-.96-.96 2.68-2.643A3.005 3.005 0 0 0 16 3c0-.269-.035-.53-.102-.777l-2.14 2.141L12 4l-.364-1.757L13.777.102a3 3 0 0 0-3.675 3.68L7.462 6.46 4.793 3.793a1 1 0 0 1-.293-.707v-.071a1 1 0 0 0-.419-.814zm9.646 10.646a.5.5 0 0 1 .708 0l2.914 2.915a.5.5 0 0 1-.707.707l-2.915-2.914a.5.5 0 0 1 0-.708M3 11l.471.242.529.026.287.445.445.287.026.529L5 13l-.242.471-.026.529-.445.287-.287.445-.529.026L3 15l-.471-.242L2 14.732l-.287-.445L1.268 14l-.026-.529L1 13l.242-.471.026-.529.445-.287.287-.445.529-.026z"></path>
                    </svg>
                    
                </div>

            `;
            this.arrayConnections.push({"refCountEsp32": refCountEsp32, "idBtnElement": '#'+idIcon+'_'+nameView, "idHtmlElement": '#'+idCheck+'_'+nameView, "espConnection": esp32});
        });

        return html;
    }

    getActiveConnections(){
        let array = [];
        this.arrayConnections.forEach( (el) => {
            let htmlElement = document.querySelector(el.idHtmlElement);
            if(htmlElement.checked){
                array.push(el);
            }
        });
        return array;
    }

    hideFieldIp() {
        this.fieldIp.style.display = 'none';
    }

    showFieldIp() {
        this.fieldIp.style.display = 'inline';
    }

    setLabelIp(value) {
        this.labelIp.textContent = value;
    }

    getLabelIp() {
        return this.labelIp.textContent;
    }

        // if (ledMainModel.aPConnection) {
        //     this.hideFieldIp();
        //     document.querySelector('.fieldIp').value = DefaultConstants.debugApp;
        // }

}