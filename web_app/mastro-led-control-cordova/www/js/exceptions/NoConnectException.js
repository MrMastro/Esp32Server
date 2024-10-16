// NoConnectException.js

class NoConnectException extends Error {
    constructor(message = 'Unable to connect') {
        // Passa il messaggio al costruttore della classe `Error`
        super(message);
        // Imposta il nome dell'errore per distinguerlo da altri
        this.name = 'NoConnectException';
        // Mantieni lo stack trace
        if (Error.captureStackTrace) {
            Error.captureStackTrace(this, NoConnectException);
        }
    }
}

export default NoConnectException;