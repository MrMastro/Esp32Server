
class GenericErrorExceptions extends Error {
    constructor(message = 'An error occurred') {
        // Passa il messaggio al costruttore della classe `Error`
        super(message);
        // Imposta il nome dell'errore per distinguerlo da altri
        this.name = 'GenericError';
        // Mantieni lo stack trace
        if (Error.captureStackTrace) {
            Error.captureStackTrace(this, GenericError);
        }
    }
}

export default GenericErrorExceptions;