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
        this.divPanelColors = {};
        this.divPanelAddRemove = {};
        this.plusBtn = {};
        this.minusBtn = {};
        this.colorsMemorized = new Array();
        this.colorQtActive = 1;
        this.minColorShowed = 1;
        this.maxColorShowed = 1;
    }

    createColorsIntoPanel(root, n) {
        for (let i = 0; i < n; i++) {
            let colorId = "Color-" + this.nameGroup + "-" + i;
            this.colorsMemorized.push(this.appendSingleColor(root, colorId, 0, 0, 0));
            this.hideSingleColor(i);
        }
    }

    render(inputColors = [], useColorByInputColors, canAddOrRemoveColor) {
        // Imposta i valori RGB iniziali
        //ColorUtils.setRgbInput(settings.initialR, settings.initialG, settings.initialB);

        this.rootElement.innerHTML = (`
            <div class="d-lg-flex flex-column justify-content-lg-center align-items-lg-center">
                <div id=${"PanelColor-" + this.nameGroup} class="text-center d-flex d-lg-flex d-xl-flex flex-row justify-content-lg-center align-items-lg-center justify-content-xl-center align-items-xl-center containerColor" style="margin-top: 5px;margin-bottom: 5px;">
                    <!-- colorInputs -->
                </div>
                <div id=${"PanelAddRemove-" + this.nameGroup} style="margin-top: 5px;margin-bottom: 5px;">
                
                    <svg id=${"MinusBtn-" + this.nameGroup} class="bi bi-dash-circle fs-4 border rounded-circle btn btn-dark" xmlns="http://www.w3.org/2000/svg" width="1em" height="1em" fill="currentColor" viewBox="0 0 16 16" data-bs-toggle="tooltip" data-bss-tooltip style="padding-top: 0px;padding-right: 0px;padding-bottom: 0px;padding-left: 0px;margin-right: 5px;margin-left: 5px;">
                        <path d="M8 15A7 7 0 1 1 8 1a7 7 0 0 1 0 14m0 1A8 8 0 1 0 8 0a8 8 0 0 0 0 16"></path>
                        <path d="M4 8a.5.5 0 0 1 .5-.5h7a.5.5 0 0 1 0 1h-7A.5.5 0 0 1 4 8"></path>
                    </svg>
                    
                    <svg id=${"PlusBtn-" + this.nameGroup} class="bi bi-plus-circle fs-4 border rounded-circle btn btn-dark" xmlns="http://www.w3.org/2000/svg" width="1em" height="1em" fill="currentColor" viewBox="0 0 16 16" data-bs-toggle="tooltip" data-bss-tooltip style="padding-top: 0px;padding-right: 0px;padding-bottom: 0px;padding-left: 0px;margin-right: 5px;margin-left: 5px;">
                        <path d="M8 15A7 7 0 1 1 8 1a7 7 0 0 1 0 14m0 1A8 8 0 1 0 8 0a8 8 0 0 0 0 16"></path>
                        <path d="M8 4a.5.5 0 0 1 .5.5v3h3a.5.5 0 0 1 0 1h-3v3a.5.5 0 0 1-1 0v-3h-3a.5.5 0 0 1 0-1h3v-3A.5.5 0 0 1 8 4"></path>
                    </svg>

                </div>
            </div>
            `);

        this.divPanelColors = document.querySelector("#" + "PanelColor-" + this.nameGroup);
        this.divPanelAddRemove = document.querySelector("#" + "PanelAddRemove-" + this.nameGroup);

        this.plusBtn = document.querySelector("#" + "PlusBtn-" + this.nameGroup);
        this.minusBtn = document.querySelector("#" + "MinusBtn-" + this.nameGroup);

        this.bindPlusAndMinusBtn();

        if (!canAddOrRemoveColor) {
            this.hideDivPanelAddRemove();
        }

        this.createColorsIntoPanel(this.divPanelColors, 5);

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
        this.checkBoundAndDisablePlusMinus();
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

    hideDivPanelAddRemove() {
        this.divPanelAddRemove.style.display = 'none';
    }

    hideAllColor() {
        for (let index = 0; index < this.colorsMemorized.length; index++) {
            this.hideSingleColor(index);
        }
        this.colorQtActive = 0;
    }

    showColors(n) {
        for (let index = 0; index < n; index++) {
            this.showSingleColor(index);
        }
        this.colorQtActive = n;
        this.checkBoundAndDisablePlusMinus();
    }

    hideSingleColor(index) {
        this.colorsMemorized[index].style.display = 'none';
    }

    showSingleColor(index) {
        this.colorsMemorized[index].style.display = 'block';
    }

    usersAddColor() {
        if (this.colorQtActive < this.maxColorShowed) {
            this.colorQtActive++;
            this.showSingleColor(this.colorQtActive-1);
        }
        this.checkBoundAndDisablePlusMinus();
    }

    usersRemoveColor() {
        if (this.colorQtActive > this.minColorShowed) {
            this.colorQtActive--;
            this.hideSingleColor(this.colorQtActive-1);
        }
        this.checkBoundAndDisablePlusMinus();
    }

    checkBoundAndDisablePlusMinus() {
        if (this.colorQtActive == this.maxColorShowed) {
            this.disablePlus();
        } else {
            this.enablePlus();
        }

        if (this.colorQtActive == this.minColorShowed) {
            this.disableMinus();
        } else {
            this.enableMinus();
        }
    }

    disablePlus() {
        let svgPlus = this.plusBtn;
        svgPlus.style.pointerEvents = "none";   
        svgPlus.style.opacity = "0.5";          
        svgPlus.style.cursor = "not-allowed";   
    }

    enablePlus() {
        let svgPlus = this.plusBtn;
        svgPlus.style.pointerEvents = "auto";   
        svgPlus.style.opacity = "1";            
        svgPlus.style.cursor = "pointer";       
    }

    disableMinus() {
        let svgMinus = this.minusBtn;
        svgMinus.style.pointerEvents = "none";  
        svgMinus.style.opacity = "0.5";         
        svgMinus.style.cursor = "not-allowed";  
    }

    enableMinus() {
        let svgMinus = this.minusBtn;
        svgMinus.style.pointerEvents = "auto";  
        svgMinus.style.opacity = "1";           
        svgMinus.style.cursor = "pointer";      
    }

    setBound(min, max) {
        this.minColorShowed = min;
        this.maxColorShowed = max;
        this.checkBoundAndDisablePlusMinus();
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

    bindPlusAndMinusBtn() {
        this.plusBtn.addEventListener('click', this.usersAddColor.bind(this));
        this.minusBtn.addEventListener('click', this.usersRemoveColor.bind(this));
    }

}