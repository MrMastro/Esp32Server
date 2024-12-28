export default class Esp32ConnectionView {
    constructor(rootElement) {
        if (!(rootElement instanceof HTMLElement)) {
            console.error('rootElement non è un elemento DOM valido', rootElement);
        }
        this.rootElement = rootElement;
        this.render();
    }

    // Metodo render per creare la struttura del pannello delle connession (WIP DA FARE)
    render(listOfConnection) { //todo
        this.rootElement.innerHTML = (`
                <div id=${"ConnectionDevices"} class="d-lg-flex flex-column justify-content-lg-center align-items-lg-center" style="margin-top: 5px;margin-bottom: 5px;">
                    <!-- colorInputs -->
                </div>
            `);
        let htmlList = "";
        if (Array.isArray(listOfConnection)) {
            htmlList = "Nessun Dispositivo disponibile";
        } else if (listOfConnection.length < 1) {
            htmlList = "Nessun Dispositivo disponibile, esegui la ricerca automatica";
        } else {
            htmlList = getEsp32ConnectionElementHtml(listOfConnection);
        }

        let button = <button class='searchEsp32'> Cerca dispositivi </button>
        this.rootElement.innerHTML = button + htmlList;
    }

    bindBtnSettings(handler) {
        this.settingsBtn.addEventListener('click', handler);
    }

    // getEsp32ConnectionElementHtml(list) {
    //     let html;
    //     list.forEach(element => {
    //         if (element.style.display != 'none') {
    //             let c = ColorUtils.hexToRgb(element.value);
    //             let request = new LedColorRequest(c.r, c.g, c.b);
    //             colors.push(request);
    //         }
    //     });
    //     return colors;
    // }

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