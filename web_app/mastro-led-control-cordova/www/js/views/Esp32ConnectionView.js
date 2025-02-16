import { ConnectionInfo } from "../models/Esp32Model.js";
import DefaultConstants from "../constants/DefaultConstants.js";

export default class Esp32ConnectionView {
    constructor(rootElement, nameView="") {
        if (!(rootElement instanceof HTMLElement)) {
            console.error('rootElement non è un elemento DOM valido', rootElement);
        }

        this.advancedSearchMode = false;

        this.ipForAdvancedSearch = null;

        this.rootElement = rootElement;
        
        this.nameView = nameView != "" ? nameView : "connectionGeneric_"+Date.now();

        this.panelAdvancedSearch = {};

        this.buttonSearchEsp32 = {};

        this.buttonAdvancedSearchEsp32 = {};
        
        this.buttonAdvancedSearchEsp32Disabled = {};
        
        this.buttonUpdateStatusEsp32 = {};

        this.buttonInfoEsp32List = [];

        this.handlerBtnUpdateStatusEsp32 = {};

        this.handlerBtnSearchEsp32 = {};

        this.handlerBtnAdvancedSearchEsp32 = {};

        this.handlerFieldIpInput = this.changeIp.bind(this);

        this.arrayConnections = [];

        this.render();
    }

    // Metodo render per creare la struttura del pannello delle connession (WIP DA FARE)
    render(listOfConnection = [], message = "") { //todo
        this.advancedSearchMode = false;
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

        groupSearchIp = this.getEsp32AdvancedSearch();

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
            if(htmlElement != null){ //fix di controllo 
                htmlElement.checked = el.espConnection.active;
                if(el.espConnection.connectionState == ConnectionInfo.OFFLINE){
                    htmlElement.checked = false;
                    htmlElement.disabled = true;
                }
                this.buttonInfoEsp32List.push(document.querySelector(el.idBtnElement));
            }else{
                this.arrayConnections = [];
            }
        });

        this.buttonSearchEsp32 = document.querySelector('.buttonSearchEsp32');
        this.buttonUpdateStatusEsp32 = document.querySelector('.updateSatusEsp32');

        this.fieldIp = document.querySelector('.fieldIp');
        this.labelIp = document.querySelector('.label-ip');
        this.fieldIp = document.querySelector('.fieldIp');

        this.panelAdvancedSearch = document.querySelector('.panelAdvancedSearch');
        this.buttonAdvancedSearchEsp32 = document.querySelector('.searchAdvanceEsp32');
        this.buttonAdvancedSearchEsp32Disabled = document.querySelector('.searchAdvanceEsp32_Disabled');

        if(!this.advancedSearchMode){
            this.panelAdvancedSearch.classList.add("hidden");
        }

        this.reassignHandler();
    }

    getEsp32AdvancedSearch() {
        let html =``;
        let nameView = this.nameView;
        let idIconSearch = "btnAdvancedSearch-"+nameView;
        
        html = `
                <div class="panelAdvancedSearch d-flex flex-column justify-content-center align-items-center">
                    <div class="border rounded d-flex justify-content-between align-items-center esp32PanelConnection" style="margin: 10px 10px 0px 0px;">
                    
                        <input id="inputIp_${nameView}" class="fieldIp" type="text" minlength="7" maxlength="15" size="15" pattern="^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$" />
                        
                        <svg id="${idIconSearch}" class="bi bi-search btn searchAdvanceEsp32 btn_svg_icon hidden" xmlns="http://www.w3.org/2000/svg" fill="currentColor" viewBox="0 0 16 16" style="width: 25px;height: 25px;">
                            <path d="M11.742 10.344a6.5 6.5 0 1 0-1.397 1.398h-.001c.03.04.062.078.098.115l3.85 3.85a1 1 0 0 0 1.415-1.414l-3.85-3.85a1.007 1.007 0 0 0-.115-.1zM12 6.5a5.5 5.5 0 1 1-11 0 5.5 5.5 0 0 1 11 0"></path>
                        </svg>

                        <svg id="${idIconSearch}_disabled" class="bi bi-search btn disabled searchAdvanceEsp32_Disabled btn_svg_icon" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16" fill="currentColor" style="width: 25px;height: 25px;">
                            <path d="M11.742 10.344a6.5 6.5 0 1 0-1.397 1.398h-.001c.03.04.062.078.098.115l3.85 3.85a1 1 0 0 0 1.415-1.414l-3.85-3.85a1.007 1.007 0 0 0-.115-.1zM12 6.5a5.5 5.5 0 1 1-11 0 5.5 5.5 0 0 1 11 0"/>
                            <line x1="2" y1="2" x2="11" y2="11" stroke="white" stroke-width="1"/>
                            <line x1="2" y1="11" x2="11" y2="2" stroke="white" stroke-width="1"/>
                        </svg>

                    </div>
                    <small class="label-ip labelIpNeutral">Inserisci un indirizzo ip della rete wifi</small>
                </div>
                `;

        return html;
    }

    reassignHandler() {
        if (typeof this.handlerBtnSearchEsp32 === 'function') {
            this.bindButtonSearchEsp32(this.handlerBtnSearchEsp32);
        }

        if (typeof this.handlerBtnAdvancedSearchEsp32 === 'function') {
            this.bindButtonAdvancedSearchEsp32(this.handlerBtnAdvancedSearchEsp32);
        }

        if (typeof this.handlerBtnUpdateStatusEsp32=== 'function') {
            this.bindButtonUpdateStatusEsp32(this.handlerBtnUpdateStatusEsp32);
        }

        this.bindLongButtonSearchEsp32();
        
        this.bindButtonInfoEsp32(this.handlerBtnInfoEsp32);

        this.bindFieldIpInput(this.handlerFieldIpInput);
    }

    bindButtonUpdateStatusEsp32(handler){
        this.handlerBtnUpdateStatusEsp32 = handler;
        this.buttonUpdateStatusEsp32.addEventListener('click', this.handlerBtnUpdateStatusEsp32);
    }

    bindButtonSearchEsp32(handler){
        this.handlerBtnSearchEsp32 = handler;
        this.buttonSearchEsp32.addEventListener('click', this.handlerBtnSearchEsp32);
    }

    bindLongButtonSearchEsp32(){
        this.pressTimer = {};
        this.buttonSearchEsp32.addEventListener('pointerdown', (event) => {
            this.isLongPress = false;  // Resetta il flag per ogni nuovo click
            this.pressTimer = setTimeout(() => {
                this.isLongPress = true;
                this.panelAdvancedSearch.classList.remove("hidden");
            }, 1000); // 1000ms = 1 secondo
        });
        
        this.buttonSearchEsp32.addEventListener('pointerup', (event) => {
            clearTimeout(this.pressTimer);  // Cancella il timer se il click viene rilasciato prima di un secondo
            if (!this.isLongPress) {
                this.advancedSearchMode = false;
                this.panelAdvancedSearch.classList.add("hidden");
            }
        });
        
        this.buttonSearchEsp32.addEventListener('pointerleave', (event) => {
            clearTimeout(this.pressTimer); // Cancella il timer se il mouse esce dal bottone prima di un secondo
        });
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

    bindButtonAdvancedSearchEsp32(handler){
        if(typeof handler === 'function'){
            this.handlerBtnAdvancedSearchEsp32 = handler;
            if(this.buttonAdvancedSearchEsp32 != null){
                this.buttonAdvancedSearchEsp32.addEventListener('click', this.handlerBtnAdvancedSearchEsp32);
            }
        }
    }

    bindFieldIpInput(handler) {
        this.handlerFieldIpInput = handler;
        if(this.fieldIp != null){
            this.fieldIp.addEventListener('input', this.handlerFieldIpInput);
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

    getIpForAdvancedSearch(){ 
        return this.ipForAdvancedSearch;
    }

    getFieldIp(){
        return document.querySelector('.fieldIp').value;
    }

    changeIp() {
        let test = /^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/.test(this.getFieldIp());
        if (!this.getFieldIp()) {
            this.setLabelIp("Inserisci un indirizzo ip della rete wifi");
            this.ipForAdvancedSearch = null;
            this.buttonAdvancedSearchEsp32Disabled.classList.remove("hidden");
            this.buttonAdvancedSearchEsp32.classList.add("hidden");
            this.labelIp.classList.remove("labelIpWrong");
            this.labelIp.classList.remove("labelIpRight");
            this.labelIp.classList.add("labelIpNeutral");
        }
        else if (test) {
            this.ipForAdvancedSearch = this.getFieldIp();
            this.setLabelIp("Clicca sulla lente per iniziare la ricerca");
            this.buttonAdvancedSearchEsp32Disabled.classList.add("hidden");
            this.buttonAdvancedSearchEsp32.classList.remove("hidden");
            this.labelIp.classList.add("labelIpRight");
            this.labelIp.classList.remove("labelIpWrong");
            this.labelIp.classList.remove("labelIpNeutral");
            
        } else {
            this.setLabelIp("Inserisci un indirizzo ip della rete wifi valido");
            this.buttonAdvancedSearchEsp32Disabled.classList.remove("hidden");
            this.buttonAdvancedSearchEsp32.classList.add("hidden");
            this.labelIp.classList.remove("labelIpRight");
            this.labelIp.classList.remove("labelIpNeutral");
            this.labelIp.classList.add("labelIpWrong");
        }
    }
    
        // if (ledMainModel.aPConnection) {
        //     this.hideFieldIp();
        //     document.querySelector('.fieldIp').value = DefaultConstants.debugApp;
        // }

}