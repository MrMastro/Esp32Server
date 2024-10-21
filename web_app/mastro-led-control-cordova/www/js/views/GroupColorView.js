import LedColorRequest from "../models/request/LedColorRequest.js";
import ColorUtils from "../utils/ColorUtils.js";

export default class GroupColorView {
    constructor(rootElement, nameGroup, onlyRead) { //empty cause modify html yet written
        if (!(rootElement instanceof HTMLElement)) {
            console.error('rootElement non è un elemento DOM valido', rootElement);
        }
        this.nameGroup = nameGroup;
        this.rootElement = rootElement;
        this.colorsElement = new Array();
        this.render([{r:0,g:0,b:0}]);
        this.onlyRead = onlyRead;
    }

    render(colors=[]) {
        // Imposta i valori RGB iniziali
        //ColorUtils.setRgbInput(settings.initialR, settings.initialG, settings.initialB);
        this.colorsElement = new Array();
        this.rootElement.innerHTML = ''; // Pulisci il contenitore
        colors.forEach((element, index) => {
            let colorId = "Color-"+this.nameGroup+"-"+index;
            this.colorsElement.push(this.appendSingleColor(this.rootElement, colorId, element.r, element.g, element.b));
        });
    }

    appendSingleColor(root, idButton, r, g, b) {
        let colorHex = ColorUtils.rgbToHex(r, g, b);
        
        // Crea un elemento <input>
        let inputElement = document.createElement('input');
        
        // Imposta gli attributi dell'elemento
        inputElement.id = idButton;
        inputElement.className = "d-lg-flex m-auto justify-content-lg-center align-items-lg-center colorInput form-control";
        inputElement.type = "color";
        inputElement.value = colorHex;  // Imposta il colore
        inputElement.style.width = "50px";
        inputElement.style.height = "40px";
        if(this.onlyRead){
            inputElement.disabled = true;   // Disabilita l'input
        }
        
        // Aggiungi l'elemento creato al nodo root
        root.appendChild(inputElement);
        return inputElement;
    }

    getColors(){
        let colors = [];
        this.colorsElement.forEach(element => {
            let c = ColorUtils.hexToRgb(element.value);
            let request = new LedColorRequest(c.r,c.g,c.b);
            colors.push(request);
        });
        return colors;
    }

}