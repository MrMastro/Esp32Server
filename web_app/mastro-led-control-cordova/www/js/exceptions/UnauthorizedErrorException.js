class UnauthorizedErrorException extends Error {
    constructor(message = 'Unauthorized') {
        // Passa il messaggio al costruttore della classe `Error`
        super(message);
        // Imposta il nome dell'errore per distinguerlo da altri
        this.name = 'UnauthorizedError';
        // Mantieni lo stack trace
        if (Error.captureStackTrace) {
            Error.captureStackTrace(this, UnauthorizedError);
        }
    }
}

export default UnauthorizedErrorException;