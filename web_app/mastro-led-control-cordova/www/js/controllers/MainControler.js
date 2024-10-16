import WaitView from '../views/WaitView.js';

export default class MainController {
    constructor(host) {
        this.waitView = new WaitView(document.getElementById('WaitViewContainer'));
        this.referenceHost = host;
        this.init();
    }

    async init() {
        this.waitView.render();
        this.bindEvents();
    }

    async bindEvents(){
        // requestAnimationFrame(() => {
        //     document.querySelector('.saveSettingsBtn').addEventListener('click', () => {
        //         this.saveSettings();
        //     });
        // });
    }

    async setReferenceHost(newHost){
        this.referenceHost = newHost;
    }

    async showWait(){
        this.waitView.show();
    }

    async hideWait(){
        this.waitView.hide();
    }

}