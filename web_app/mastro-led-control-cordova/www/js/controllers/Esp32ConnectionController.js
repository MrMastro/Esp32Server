import Esp32ConnectionService from '../services/Esp32ConnectionService.js';
import LocalStorageService from '../services/LocalStorageService.js';
import Esp32ConnectionView from '../views/Esp32ConnectionView.js';


export default class Esp32ConnectionController {
    constructor(context, headerView) {

        this.context = context;

        this.esp32ConnectionView = context.espConnectionView; 
        this.localStorageService = new LocalStorageService();
        this.esp32ConnectionService = new Esp32ConnectionService();
        this.esp32ConnectionView.render(this.localStorageService.getEsp32InfoDeviceMem());
        this.init();
    }

    async init() {
        this.bindEvents();
    }

    async bindEvents() {
        this.esp32ConnectionView.bindButtonSearchEsp32(this.runLinkedDeviceSearch.bind(this));
        requestAnimationFrame(() => {
            console.log("WIP");
        });
    }

    async runLinkedDeviceSearch(){
        this.esp32ConnectionService.setLinkedDeviceSearch();
        this.esp32ConnectionView.render(this.localStorageService.getEsp32InfoDeviceMem())
    }

}