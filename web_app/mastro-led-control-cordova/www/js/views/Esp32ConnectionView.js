export default class Esp32ConnectionView {
    constructor(rootElement) {
        if (!(rootElement instanceof HTMLElement)) {
            console.error('rootElement non è un elemento DOM valido', rootElement);
        }
        this.rootElement = rootElement;
        
        this.buttonSearchEsp32 = {};

        this.handlerButtonSearchEsp32 = {};

        this.render();
    }

    // Metodo render per creare la struttura del pannello delle connession (WIP DA FARE)
    render(listOfConnection = []) { //todo
        let htmlList = "";
        if (Array.isArray(listOfConnection)) {
            if (listOfConnection.length < 1) {
                htmlList = `<label class="form-label">Nessun Dispositivo disponibile.<br/>Cerca dispositivi cliccando sulla lente di ingrandimento</label>`;
            }else{
                htmlList = this.getEsp32ConnectionElementHtml(listOfConnection);
            }
        } else {
            htmlList = "Errore: Nessun Dispositivo disponibile";
        }

        // let button = <button class='searchEsp32'> Cerca dispositivi </button>
        // this.rootElement.innerHTML = button + htmlList;
        this.rootElement.innerHTML = (`

            <div class="container containerLabel" style="height: auto;max-height: 25px;">
                <label class="form-label form-label-title" style="padding-top: 0px;padding-right: 0px;padding-bottom: 0px;padding-left: 0px;">Connessioni Esp32</label>
                <svg class="bi bi-search fs-4 border rounded-circle buttonSearchEsp32 btn btn-dark" xmlns="http://www.w3.org/2000/svg" width="1em" height="1em" fill="currentColor" viewBox="0 0 16 16" data-bs-toggle="tooltip" data-bss-tooltip style="padding-top: 0px;padding-right: 0px;padding-bottom: 0px;padding-left: 0px;" title="Cerca dispositivi">
                    <path d="M11.742 10.344a6.5 6.5 0 1 0-1.397 1.398h-.001c.03.04.062.078.098.115l3.85 3.85a1 1 0 0 0 1.415-1.414l-3.85-3.85a1.007 1.007 0 0 0-.115-.1zM12 6.5a5.5 5.5 0 1 1-11 0 5.5 5.5 0 0 1 11 0"></path>
                </svg>
            </div>

            <div class="border rounded-pill d-flex d-md-flex d-xxl-flex flex-column justify-content-center align-items-center align-content-center justify-content-md-center justify-content-xxl-center componentForm rangeForm dark" value="timing" name="timing" min="0" max="1000" step="1" title="Value" data-bs-theme="dark" style="padding: 0px;padding-right: 12px;padding-left: 12px;padding-top: 10px;width: auto;--bs-body-bg: #2b3035;background: var(--bs-body-bg);">
                <div id="whiteTextInside" class="d-lg-flex flex-column justify-content-lg-center align-items-lg-center">
                    ${htmlList}
                </div>
            </div>


        `);

        // <div id=${"ConnectionDevices"} class="d-lg-flex flex-column justify-content-lg-center align-items-lg-center" style="margin-top: 5px;margin-bottom: 5px;">
        //     <!-- esp32Connections -->
        //     ${htmlList}
        // </div>

        this.buttonSearchEsp32 = document.querySelector('.buttonSearchEsp32');

    }

    reassignHandler() {
        if (typeof this.handlerButtonSearchEsp32 === 'function') {
            this.bindButtonSearchEsp32(this.handlerButtonSearchEsp32);
        }
    }

    bindButtonSearchEsp32(handler){
        this.handlerBtnSearchEsp32 = handler;
        this.buttonSearchEsp32.addEventListener('click', this.handlerBtnSearchEsp32);
    }




    getEsp32ConnectionElementHtml(list) {
        let html =``;
        list.forEach((esp32, count) => {
            let idCheck = "espConnection-" + count;
            let idInfo = "infoEspConnection-" + count;
            let deviceName = esp32.deviceName;
            html+= `
                <div class="d-flex d-print-flex d-sm-flex d-md-flex d-lg-flex d-xl-flex d-xxl-flex flex-row justify-content-center align-items-center justify-content-xl-center align-items-xl-center">
                
                    <input id="${idCheck}" class="elementEsp32" type="checkbox" style="width: 25px;height: 25px;" />
                    
                    <span class="fs-5 elementEsp32" style="min-width: 150px;">${deviceName}</span>
                    
                    <svg id="${idInfo}" class="bi bi-info-circle elementEsp32" xmlns="http://www.w3.org/2000/svg" width="1em" height="1em" fill="currentColor" viewBox="0 0 16 16" style="width: 25px;height: 25px;">
                        <path d="M8 15A7 7 0 1 1 8 1a7 7 0 0 1 0 14m0 1A8 8 0 1 0 8 0a8 8 0 0 0 0 16"></path>
                        <path d="m8.93 6.588-2.29.287-.082.38.45.083c.294.07.352.176.288.469l-.738 3.468c-.194.897.105 1.319.808 1.319.545 0 1.178-.252 1.465-.598l.088-.416c-.2.176-.492.246-.686.246-.275 0-.375-.193-.304-.533zM9 4.5a1 1 0 1 1-2 0 1 1 0 0 1 2 0"></path>
                    </svg>

                </div>

            `;
        });
        return html;
    }

    createEsp32IntoPanel(root, list) {
        list.forEach((esp32, count) => {
            let id = "espConnection-" + count;
            this.appendSingleEsp32(root, id, esp32);//todo insert into object esp32ListInView => this.esp32ListInView = this.appendSingleEsp32(...)
        });
    }

    appendSingleEsp32(root, id, esp32) {

        // Crea un elemento <input>
        let inputElement = document.createElement('<p>' + esp32.ip + '</p>');

        // Imposta gli attributi dell'elemento
        inputElement.id = id;
        inputElement.className = "d-lg-flex m-auto justify-content-lg-center align-items-lg-center";

        // Aggiungi l'elemento creato al nodo root
        root.appendChild(inputElement);
        return inputElement;
    }
}