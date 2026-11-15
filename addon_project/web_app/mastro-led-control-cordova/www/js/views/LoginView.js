
export default class LoginView {
    constructor(rootElement) {
        if (!(rootElement instanceof HTMLElement)) {
            console.error('rootElementModal non è un elemento DOM valido', rootElement);
        }
        this.modal = rootElement;
        this.ip = null;

        this.esp32Selected = null;

        this.handlerBtnLogin = {};

        this.passwordsData = [];
    }

    render(esp32Model) {
        let connection = "";
        let infoConnection = "";
        let deviceName = "";
        let password = "";
        let connectionState = "";
        this.ip = "";
    
        if (typeof esp32Model != 'undefined') {
            connection = esp32Model.espConnection;
            infoConnection = esp32Model.espConnection.infoConnection;
            deviceName = infoConnection.deviceName;
            connectionState = connection.connectionState;
            this.ip = infoConnection.ip;
            password = this.getLastPassword();
            this.esp32Selected = esp32Model;
            
        }
    
        this.modal.innerHTML = (`
            <div class="modal-dialog" role="document">
                <div class="modal-content">
                    <div id="Login-Modal" class="" tabindex="-1" role="dialog" aria-labelledby="loginModalLabel" aria-hidden="true">
                        <div class="modal-dialog" role="document">
                            <div class="modal-content">
                                <div class="modal-header">
                                    <h1 class="modal-title d-flex justify-content-center align-items-center">Info</h1>
                                    <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
                                </div>
    
                                <div class="modal-body">
                                    <form class="p-4 py-5 formLoginForUpdate" novalidate>
                                        <div class="mb-3">
                                            <label class="form-label" for="deviceNameLogin">Nome Dispositivo</label>
                                            <input id="deviceNameLogin" value="${deviceName}" class="form-control item" type="text" required minlength="1" readonly />
                                        </div>
    
                                        <div class="mb-3">
                                            <label class="form-label" for="ip">Indirizzo Ip</label>
                                            <input id="ip" value="${this.ip}" class="form-control item" type="text" required minlength="1" readonly />
                                        </div>
    
                                        <div class="mb-3">
                                            <label class="form-label"> Stato </label>
                                            <div class="groupStato d-flex align-items-center">
                                                <input id="stato" value="${connectionState}" class="form-control item" type="text" required minlength="1" readonly />
                                                <span id="status-icon" class="ms-2 item indicator-${connectionState}" style="width: 15px;height: 15px;border-radius: 50%;"></span>
                                            </div>
                                        </div>
    
                                        <div class="mb-3">
                                            <label class="form-label" for="passwordLogin">Password</label>
                                            <input id="passwordLogin" class="form-control passwordInput" type="password" required minlength="1" />
                                            <label class="labelWrong hidden" id="passwordLoginLabel"> Inserire una password valida per accedere alle impostazioni </label>
                                        </div>
                                        <button id="submitLoginForUpdate" class="btn btn-primary" type="submit">Impostazioni</button>
                                    </form>
                                </div>
    
                                <div class="modal-footer">
                                    <button type="button" class="btn btn-secondary" data-bs-dismiss="modal">Close</button>
                                </div>
    
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        `);
    
        // Seleziona il form appena creato
        const form = this.modal.querySelector(".formLoginForUpdate");

        if(password != ""){
            this.modal.querySelector("#passwordLogin").value = password;
        }
    
        // Previeni il comportamento predefinito
        form.addEventListener("submit", function(event) {
            event.preventDefault(); 
            form.classList.add("was-validated");
        });

        this.reassignHandler(esp32Model);

    }

    bindBtnLogin(handler) {
        this.handlerBtnLogin = handler;
        // Assicuriamoci che l'elemento esista prima di fare il binding
        const btnLogin =  document.getElementById('submitLoginForUpdate');
        if (btnLogin) {
            btnLogin.addEventListener('click', (event) => {
                const form = this.modal.querySelector(".formLoginForUpdate");
                if (!form.checkValidity()) {
                    event.preventDefault();
                    document.querySelector("#passwordLoginLabel").classList.remove("hidden");
                    event.stopPropagation();
                }else{
                    let macAddress = this.esp32Selected.espConnection.infoConnection.macAdress;
                    document.querySelector("#passwordLoginLabel").classList.add("hidden");
                    event.preventDefault(); // Previene il comportamento predefinito del form
                    this.addLastPassword(macAddress, this.getDevicePassword());
                    handler(this.esp32Selected); // Chiama il metodo passato
                }
            });
        }
    }

    reassignHandler(esp32Model) {
        if(typeof esp32Model != 'undefined'){
            if (typeof this.handlerBtnLogin === 'function') {
                this.bindBtnLogin(this.handlerBtnLogin);
            }
        }
    }

    show(esp32Model) {
        this.render(esp32Model);
        $(this.modal).modal('show');
    }

    hide() {
        $(this.modal).modal('hide');
    }

    getDeviceName(){
        return document.querySelector('#deviceNameLogin').value;
    }

    getDevicePassword(){
        return document.querySelector('#passwordLogin').value;
    }

    getLastPassword(){
        let el = this.esp32Selected;
        let myPass = this.passwordsData;
        let result = "";
        
        myPass.forEach( (i) => {
            let elMacAdress = el.espConnection.infoConnection.macAdress;
            if(i.macAddress == elMacAdress){
                result = i.password;
                return i.password;
            }
        });
        return result;
    }

    addLastPassword(macAddress, password){
        let index = this.passwordsData.findIndex(item => item.macAddress === macAddress);

        if (index !== -1) {
            this.passwordsData[index].password = this.getDevicePassword(); // Aggiorna la password
        } else {
            this.passwordsData.push({ macAddress: macAddress, password: password }); // Aggiunge un nuovo elemento
        }
    }
}