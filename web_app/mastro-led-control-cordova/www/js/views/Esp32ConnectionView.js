export default class Esp32ConnectionView {
    constructor(rootElement, nameView) {
        if (!(rootElement instanceof HTMLElement)) {
            console.error('rootElement non è un elemento DOM valido', rootElement);
        }
        this.rootElement = rootElement;
        
        this.nameView = nameView;

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

            <div class="panelConnections d-flex dark" style="padding: 0px;padding-right: 12px;padding-left: 12px;padding-top: 10px;width: auto;--bs-body-bg: #2b3035;background: var(--bs-body-bg);min-width: auto;">
                <div class="d-flex d-lg-flex flex-column align-items-center justify-content-lg-center align-items-lg-center">
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
            let nameView = this.nameView;
            let idCheck = "checkEspConnection-" + count;
            let idIcon = "btnEspConnection-" + count;
            let deviceName = esp32.deviceName;
            html+= `
                <div class=" esp32PanelConnection d-flex justify-content-between align-items-center esp32PanelConnection" style="min-width: 250px;">
                    
                    <input id="${idCheck}_${nameView}" class="elementEsp32" type="checkbox" style="width: 25px;height: 25px;" />
                    
                    <span class="text-start" style="min-width: 175px;">${deviceName}</span>
                    
                    <svg id="${idIcon}_${nameView}" class="bi bi-gear" xmlns="http://www.w3.org/2000/svg" width="1em" height="1em" fill="currentColor" viewBox="0 0 16 16" style="width: 25px;height: 25px;">
                        <path d="M8 4.754a3.246 3.246 0 1 0 0 6.492 3.246 3.246 0 0 0 0-6.492zM5.754 8a2.246 2.246 0 1 1 4.492 0 2.246 2.246 0 0 1-4.492 0z"></path>
                        <path d="M9.796 1.343c-.527-1.79-3.065-1.79-3.592 0l-.094.319a.873.873 0 0 1-1.255.52l-.292-.16c-1.64-.892-3.433.902-2.54 2.541l.159.292a.873.873 0 0 1-.52 1.255l-.319.094c-1.79.527-1.79 3.065 0 3.592l.319.094a.873.873 0 0 1 .52 1.255l-.16.292c-.892 1.64.901 3.434 2.541 2.54l.292-.159a.873.873 0 0 1 1.255.52l.094.319c.527 1.79 3.065 1.79 3.592 0l.094-.319a.873.873 0 0 1 1.255-.52l.292.16c1.64.893 3.434-.902 2.54-2.541l-.159-.292a.873.873 0 0 1 .52-1.255l.319-.094c1.79-.527 1.79-3.065 0-3.592l-.319-.094a.873.873 0 0 1-.52-1.255l.16-.292c.893-1.64-.902-3.433-2.541-2.54l-.292.159a.873.873 0 0 1-1.255-.52l-.094-.319zm-2.633.283c.246-.835 1.428-.835 1.674 0l.094.319a1.873 1.873 0 0 0 2.693 1.115l.291-.16c.764-.415 1.6.42 1.184 1.185l-.159.292a1.873 1.873 0 0 0 1.116 2.692l.318.094c.835.246.835 1.428 0 1.674l-.319.094a1.873 1.873 0 0 0-1.115 2.693l.16.291c.415.764-.42 1.6-1.185 1.184l-.291-.159a1.873 1.873 0 0 0-2.693 1.116l-.094.318c-.246.835-1.428.835-1.674 0l-.094-.319a1.873 1.873 0 0 0-2.692-1.115l-.292.16c-.764.415-1.6-.42-1.184-1.185l.159-.291A1.873 1.873 0 0 0 1.945 8.93l-.319-.094c-.835-.246-.835-1.428 0-1.674l.319-.094A1.873 1.873 0 0 0 3.06 4.377l-.16-.292c-.415-.764.42-1.6 1.185-1.184l.292.159a1.873 1.873 0 0 0 2.692-1.115l.094-.319z"></path>
                    </svg>
                    
                </div>

            `;
        });
        return html;
    }

}