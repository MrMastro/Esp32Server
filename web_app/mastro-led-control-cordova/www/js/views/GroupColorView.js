import LedColorRequest from "../models/request/LedColorRequest.js";
import ColorUtils from "../utils/ColorUtils.js";

export default class GroupColorView {
    constructor(rootElement, nameGroup, onlyRead) { //empty cause modify html yet written
        if (!(rootElement instanceof HTMLElement)) {
            console.error('rootElement non è un elemento DOM valido', rootElement);
        }
        this.nameGroup = nameGroup;
        this.rootElement = rootElement;
        this.onlyRead = onlyRead;
        this.colorsMemorized = new Array();
        this.initColorMemorized(5);
        this.colorQtActive = 0;
        this.minColorShowed = 1;
        this.maxColorShowed = 10;
    }

    initColorMemorized(n) {
        for (let i = 0; i < n; i++) {
            let colorId = "Color-" + this.nameGroup + "-" + i;
            this.colorsMemorized.push(this.appendSingleColor(this.rootElement, colorId, 0, 0, 0));
            this.hideSingleColor(i);
        }
    }

    render(inputColors = [], useColorByInputColors = false) {
        // Imposta i valori RGB iniziali
        //ColorUtils.setRgbInput(settings.initialR, settings.initialG, settings.initialB);

        for (let i = 0; i < this.colorsMemorized.length; i++) {
            this.hideSingleColor(i);
        }

        for (let i = 0; i < inputColors.length; i++) {
            let newColor = ColorUtils.rgbToHex(inputColors[i].r, inputColors[i].g, inputColors[i].b);
            this.showSingleColor(i);
            if (useColorByInputColors) {
                this.colorsMemorized[i].value = newColor;
            }
        }
        this.colorQtActive = inputColors.length;
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
        if (this.onlyRead) {
            inputElement.disabled = true;   // Disabilita l'input
        }

        // Aggiungi l'elemento creato al nodo root
        root.appendChild(inputElement);
        return inputElement;
    }

    hideAllColor(){
        for (let index = 0; index < this.colorsMemorized.length; index++) {
            this.hideSingleColor(index);
        }
        this.colorQtActive = 0;
    }

    showButton(n){
        for (let index = 0; index < n; index++) {
            this.showSingleColor(index);
        }
        this.colorQtActive = n;
    }

    hideSingleColor(index) {
        this.colorsMemorized[index].style.display = 'none';
    }

    showSingleColor(index) {
        this.colorsMemorized[index].style.display = 'block';
    }

    usersAddColor(){
        console.log("WIP");
    }

    usersRemoveColor(){
        console.log("WIP");
    }

    setBound(min,max){
        this.minColorShowed = min;
        this.maxColorShowed = max;
    }


    getColors() {
        let colors = [];
        this.colorsMemorized.forEach(element => {
            if (element.style.display != 'none') {
                let c = ColorUtils.hexToRgb(element.value);
                let request = new LedColorRequest(c.r, c.g, c.b);
                colors.push(request);
            }
        });
        return colors;
    }

}