import Esp32ConnectionService from '../services/Esp32ConnectionService.js';
import LocalStorageService from '../services/LocalStorageService.js';
import AlertMessageView from '../views/AlertMessageView.js';
import WaitView from '../views/WaitView.js';


export default class Esp32ConnectionController {
    constructor(context) {

        this.context = context;

        this.esp32ConnectionView = context.espConnectionView; 
        this.waitView = new WaitView(document.getElementById('WaitViewContainer'));
        this.alertMessageView = new AlertMessageView(document.getElementById('AlertMessageViewContainer'));

        this.localStorageService = new LocalStorageService();
        this.esp32ConnectionService = context.espConnectionService;
        this.esp32ConnectionView.render(this.localStorageService.getEsp32InfoDeviceMem());
        this.init();
    }

    async init() {
        this.waitView.render();
        this.bindEvents();
    }

    async bindEvents() {
        this.esp32ConnectionView.bindButtonSearchEsp32(this.runLinkedDeviceSearch.bind(this));
        this.esp32ConnectionView.bindButtonUpdateStatusEsp32(this.updateStatusEsp32.bind(this));
        requestAnimationFrame(() => {
            console.log("WIP");
        });
    }

    async runLinkedDeviceSearch(){
        this.waitView.show();
        await this.esp32ConnectionService.setLocalIP();
        if(this.esp32ConnectionService.getIP() == null){
            this.alertMessageView.alert("Non connesso","Sembra tu non sia connesso ad una rete wireless o il dispositivo non sia collegato in modalità hot spot.");
        }
        await this.esp32ConnectionService.setLinkedDeviceSearch();
        this.esp32ConnectionView.render(this.localStorageService.getEsp32InfoDeviceMem());
        this.waitView.hide();
    }

    async updateStatusEsp32(){
        this.waitView.show();
        await this.esp32ConnectionService.setLocalIP();
        if(this.esp32ConnectionService.getIP() == null){
            this.alertMessageView.alert("Non connesso","Sembra tu non sia connesso ad una rete wireless o il dispositivo non sia collegato in modalità hot spot.");
        }
        await this.esp32ConnectionService.updateStatusDevices();
        this.esp32ConnectionView.render(this.localStorageService.getEsp32InfoDeviceMem());
        this.waitView.hide();
    }

}