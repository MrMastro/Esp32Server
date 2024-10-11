
export default class LoginView {
    constructor(rootElement) {
        if (!(rootElement instanceof HTMLElement)) {
            console.error('rootElementModal non è un elemento DOM valido', rootElement);
        }
        this.rootElement = rootElement;
        this.modalElement = null;
    }

    render() {
        this.rootElement.insertAdjacentHTML('beforeend', `
            <div id="Login-Modal" class="modal fade" tabindex="-1" role="dialog" aria-labelledby="loginModalLabel" aria-hidden="true">
                <div class="modal-dialog" role="document">
                    <div class="modal-content">
                    <div class="modal-header">
                        <h5 class="modal-title" id="loginModalLabel">Login</h5>
                        <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
                    </div>
                    <div class="modal-body">
                        <form class="p-4 py-5 formLoginForUpdate" action="#" enctype="application/x-www-form-urlencoded">
                        <div class="mb-3">
                            <label class="form-label" for="deviceNameLogin">Device Name</label>
                            <input id="deviceNameLogin" class="form-control item" type="text" required minlength="1" />
                        </div>
                        <div class="mb-3">
                            <label class="form-label" for="passwordLogin">Password</label>
                            <input id="passwordLogin" class="form-control passwordInput" type="password" required minlength="1" />
                        </div>
                        <button id="submitLoginForUpdate" class="btn btn-primary">Log In</button>
                        </form>
                    </div>
                    <div class="modal-footer">
                        <button type="button" class="btn btn-secondary" data-bs-dismiss="modal">Close</button>
                    </div>
                    </div>
                </div>
            </div>
        `);
        // Inizializza la modale qui, dopo che è stata renderizzata
        this.modalElement = new bootstrap.Modal(document.getElementById('Login-Modal'));
    }

    bindBtnLogin(handler) {
        // Assicuriamoci che l'elemento esista prima di fare il binding
        const btnLogin =  document.getElementById('submitLoginForUpdate');
        if (btnLogin) {
            btnLogin.addEventListener('click', (event) => {
                event.preventDefault(); // Previene il comportamento predefinito del form
                handler(); // Chiama il metodo passato
            });
        } else {
            console.warn('submitLoginForUpdate button not found in DOM.');
        }
    }

    show() {
        this.modalElement.show();
    }

    hide() {
        this.modalElement.hide();
    }
}