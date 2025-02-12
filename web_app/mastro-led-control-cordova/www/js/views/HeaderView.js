export default class HeaderView {
    constructor(rootElement) {
        if (!(rootElement instanceof HTMLElement)) {
            console.error('rootElement non è un elemento DOM valido', rootElement);
        }
        this.rootElement = rootElement;
        this.settingsBtn = {};
        this.render();
    }

    // Metodo render per creare la struttura del modale e iniettarla nel DOM
    render() {
        this.rootElement.insertAdjacentHTML('beforeend',`
            <nav class="navbar navbar-expand-md bg-body d-md-flex justify-content-md-center align-items-md-center py-3">
                <div class="container"><a class="navbar-brand d-flex align-items-center" href="#"><span class="bs-icon-sm bs-icon-rounded bs-icon-primary d-flex justify-content-center align-items-center me-2 bs-icon"><svg class="bi bi-bezier" xmlns="http://www.w3.org/2000/svg" width="1em" height="1em" fill="currentColor" viewBox="0 0 16 16">
                                <path fill-rule="evenodd" d="M0 10.5A1.5 1.5 0 0 1 1.5 9h1A1.5 1.5 0 0 1 4 10.5v1A1.5 1.5 0 0 1 2.5 13h-1A1.5 1.5 0 0 1 0 11.5zm1.5-.5a.5.5 0 0 0-.5.5v1a.5.5 0 0 0 .5.5h1a.5.5 0 0 0 .5-.5v-1a.5.5 0 0 0-.5-.5zm10.5.5A1.5 1.5 0 0 1 13.5 9h1a1.5 1.5 0 0 1 1.5 1.5v1a1.5 1.5 0 0 1-1.5 1.5h-1a1.5 1.5 0 0 1-1.5-1.5zm1.5-.5a.5.5 0 0 0-.5.5v1a.5.5 0 0 0 .5.5h1a.5.5 0 0 0 .5-.5v-1a.5.5 0 0 0-.5-.5zM6 4.5A1.5 1.5 0 0 1 7.5 3h1A1.5 1.5 0 0 1 10 4.5v1A1.5 1.5 0 0 1 8.5 7h-1A1.5 1.5 0 0 1 6 5.5zM7.5 4a.5.5 0 0 0-.5.5v1a.5.5 0 0 0 .5.5h1a.5.5 0 0 0 .5-.5v-1a.5.5 0 0 0-.5-.5z"></path>
                                <path d="M6 4.5H1.866a1 1 0 1 0 0 1h2.668A6.517 6.517 0 0 0 1.814 9H2.5c.123 0 .244.015.358.043a5.517 5.517 0 0 1 3.185-3.185A1.503 1.503 0 0 1 6 5.5zm3.957 1.358A1.5 1.5 0 0 0 10 5.5v-1h4.134a1 1 0 1 1 0 1h-2.668a6.517 6.517 0 0 1 2.72 3.5H13.5c-.123 0-.243.015-.358.043a5.517 5.517 0 0 0-3.185-3.185z"></path>
                            </svg></span><span>Mastro Connecting</span></a><button class="navbar-toggler" data-bs-toggle="collapse" data-bs-target="#navcol-3"><span class="visually-hidden">Toggle navigation</span><span class="navbar-toggler-icon"></span></button>
                    <div id="navcol-3" class="collapse navbar-collapse">
                        <ul class="navbar-nav me-auto">
                            <li class="nav-item"><a class="btn" role="button" href hrefcommented="/webConsole">Seriale</a></li>
                            <li class="nav-item"><a class="btn" role="button" href hrefcommented="updateLogin">Aggiorna Esp32</a></li>
                        </ul> <a id=settingsBtnHeader class="btn btn-primary settingsBtn hidden" role="button">Impostazioni</a>
                    </div>
                </div>
            </nav>
        `);
        this.settingsBtn = document.getElementById('settingsBtnHeader');
    }

    bindBtnSettings(handler){
        this.settingsBtn.addEventListener('click', handler);
    }
}