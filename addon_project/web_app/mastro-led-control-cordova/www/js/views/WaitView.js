export default class WaitView {
    constructor(rootElement) {
        if (!(rootElement instanceof HTMLElement)) {
            console.error('rootElement non è un elemento DOM valido', rootElement);
        }
        this.rootElement = rootElement;
        this.modalElement = null;
    }

    // Metodo render per creare la struttura del modale e iniettarla nel DOM
    render() {
        this.rootElement.insertAdjacentHTML('beforeend', `
            <div id="waitViewModal" class="modal fade" tabindex="-1" aria-labelledby="waitViewModalLabel" aria-hidden="true">
                <div class="modal-dialog modal-dialog-centered">
                    <div class="modal-content">
                        <div class="modal-body d-flex justify-content-center align-items-center">
                            <div class="spinner-border text-primary" role="status">
                                <span class="visually-hidden">Loading...</span>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        `);
        this.modalElement = new bootstrap.Modal(document.getElementById('waitViewModal'));
    }

    // Mostra il modale con lo spinner
    show() {
        if (this.modalElement) {
            this.modalElement.show();
        }
    }

    // Nasconde il modale con lo spinner
    hide() {
        if (this.modalElement) {
            setTimeout(() => {
                this.modalElement.hide(); // questo verrà eseguito dopo 5 secondi
            }, 1000); // 1000 millisecondi = 1 secondo
        }
    }
}