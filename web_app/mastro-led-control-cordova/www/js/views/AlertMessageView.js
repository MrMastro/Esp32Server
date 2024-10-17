export default class AlertMessageView {
    constructor(rootElement) {
        if (!(rootElement instanceof HTMLElement)) {
            console.error('rootElement non è un elemento DOM valido', rootElement);
        }
        this.rootElement = rootElement;
        this.modalElement = null;
        this.render("Prova", "ProvaMessage");
    }

    // Metodo render per creare la struttura del modale e iniettarla nel DOM
    render(title, message) {
        this.rootElement.innerHTML =`
            <div id="AlertModalContainer" class="modal fade" role="dialog" tabindex="-1">
                <div class="modal-dialog modal-dialog-centered" role="document">
                    <div class="modal-content">
                        <div class="modal-header">
                            <h4 class="modal-title">${title}</h4>
                            <button class="btn-close" type="button" aria-label="Close" data-bs-dismiss="modal"></button>
                        </div>
                        <div class="modal-body">
                            <p style="margin-bottom: 0px;">${message}</p>
                        </div>
                        <div class="modal-footer">
                            <button class="btn btn-light" type="button" data-bs-dismiss="modal">Close</button>
                        </div>
                    </div>
                </div>
            </div>
        `;
        this.modalElement = new bootstrap.Modal(document.getElementById('AlertModalContainer'));
    }

    alert(title, message) {
        if (this.modalElement) {
            this.render(title, message);
            this.modalElement.show();
        }
    }


    show() {
        if (this.modalElement) {
            this.modalElement.show();
        }
    }

    hide() {
        if (this.modalElement) {
            this.modalElement.hide();
        }
    }
}