const FrontEndMessage = (() => {

    //titlte of alert for front end
    const titleError = "ERRORE";
    const titleSuccess = "SUCCESSO";

    //Message for front end
    const genericSuccessOperation = "Operazione effettuata con successo"
    const noConnect = "Impossibile stabilire la connessione con il dispositivo, verifica connessione";
    const unauthorized = "Impossibile accedere, credenziali errate";
    const genericError = "Errore Generico";
    
    return {
        titleSuccess,
        titleError,
        genericSuccessOperation,
        noConnect,
        unauthorized,
        genericError
    };
})();

export default FrontEndMessage;