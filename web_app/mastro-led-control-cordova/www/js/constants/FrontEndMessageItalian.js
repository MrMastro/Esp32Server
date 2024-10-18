const FrontEndMessage = (() => {

    //titlte of alert for front end
    const titleError = "ERRORE";
    const titleSuccess = "SUCCESSO";

    //Message for front end
    const genericSuccessOperation = "Operazione effettuata con successo";
    const updateEffectListSuccess = "Hai aggiornato la lista degli effetti con successo";
    const saveSettingsSuccess = "Impostazioni salvate, il dispositivo sarà riavviato";
    const noConnect = "Impossibile stabilire la connessione con il dispositivo, verifica connessione";
    const unauthorized = "Accesso non autorizzato";
    const unauthorizedWrongCredential = "Impossibile accedere, credenziali errate";
    const unauthorizedRelogin = "Impossibile accedere, prova a rieseguire l'accesso";
    const genericError = "Errore Generico";
    
    return {
        titleSuccess,
        titleError,
        genericSuccessOperation,
        updateEffectListSuccess,
        saveSettingsSuccess,
        noConnect,
        unauthorized,
        unauthorizedWrongCredential,
        unauthorizedRelogin,
        genericError
    };
})();

export default FrontEndMessage;