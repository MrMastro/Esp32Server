// HttpUtils.js

const HttpUtils = (() => {
    // Funzione principale per inviare una richiesta POST
    const postCustom = async (host, url, queryParam, postData) => {
        url = host + url; // Concatenare l'host con l'URL fornito

        const options = {
            method: "POST",
            data: postData,
            param: queryParam
        };

        if (cordova.platformId === 'browser') {
            throw new Error("Applicazione non disponibile per il browser");
        } else {
            return await postWithAndroid(url, options);
        }
    };

    const getCustom = async (host, url, queryParam, postData) => {
        url = host + url; // Concatenare l'host con l'URL fornito

        const options = {
            method: "GET",
            data: postData,
            param: queryParam
        };

        if (cordova.platformId === 'browser') {
            throw new Error("Applicazione non disponibile per il browser");
        } else {
            return await getWithAndroid(url, options);
        }
    };

    // Funzione per gestire la richiesta POST su Android
    const postWithAndroid = async (url, options) => {
        if (options.param && JSON.stringify(options.param) !== '{}') {
            url += `?${options.param}`; // Aggiungere i parametri all'URL
        }

        url = "http://" + url;

        return new Promise((resolve, reject) => {
            cordova.plugin.http.setDataSerializer('urlencoded');
            cordova.plugin.http.post(url, options.data, { "Accept": "application/json" },
                (response) => {
                    resolve(response); // Risolvere la promessa con la risposta
                },
                (err) => {
                    console.log(err);
                    resolve("ERROR");
                }
            );
        });
    };

    // Funzione per gestire la richiesta POST su Android
    const getWithAndroid = async (url, options) => {
        if (options.param && JSON.stringify(options.param) !== '{}') {
            url += `?${options.param}`; // Aggiungere i parametri all'URL
        }

        url = "http://" + url;

        return new Promise((resolve, reject) => {
            cordova.plugin.http.setDataSerializer('urlencoded');
            cordova.plugin.http.get(url, options.param, { "Accept": "application/json" },
                (response) => {
                    resolve(response); // Risolvere la promessa con la risposta
                },
                (err) => {
                    reject(err); // Rifiutare la promessa in caso di errore
                }
            );
        });
    };

    // Funzione per gestire la richiesta POST nel browser
    const postWithBrowser = async (url, options) => {
        if (options.param) {
            url += `?${encodeURIComponent(options.param)}`; // Aggiungere i parametri all'URL
        }

        return new Promise((resolve, reject) => {
            const xhr = new XMLHttpRequest();
            xhr.open('POST', url, true);
            xhr.setRequestHeader('Content-Type', 'application/json');

            xhr.onreadystatechange = function () {
                if (xhr.readyState === XMLHttpRequest.DONE) {
                    if (xhr.status === 200) {
                        const response = JSON.parse(xhr.responseText);
                        resolve(response); // Risolvere la promessa con la risposta
                    } else {
                        reject(`Error: ${xhr.status} - ${xhr.statusText}`); // Rifiutare la promessa in caso di errore
                    }
                }
            };
            xhr.send(JSON.stringify(options.data)); // Inviare i dati in formato JSON
        });
    };

    // Funzione per gestire la richiesta POST nel browser con supporto CORS
    const postWithBrowserCors = async (url, options) => {
        if (options.param) {
            url += `?${encodeURIComponent(options.param)}`; // Aggiungere i parametri all'URL
        }

        return new Promise((resolve, reject) => {
            const xhr = new XMLHttpRequest();
            xhr.open('POST', url, true);
            xhr.setRequestHeader('Content-Type', 'application/json');

            xhr.onload = function () {
                const accessControlAllowOrigin = xhr.getResponseHeader('Access-Control-Allow-Origin');
                if (accessControlAllowOrigin) {
                    console.log("CORS header present:", accessControlAllowOrigin);
                } else {
                    console.warn("No CORS header present in the response.");
                }

                if (xhr.status === 200) {
                    const response = JSON.parse(xhr.responseText);
                    resolve(response); // Risolvere la promessa con la risposta
                } else {
                    reject(`Error: ${xhr.status} - ${xhr.statusText}`); // Rifiutare la promessa in caso di errore
                }
            };

            xhr.onerror = function () {
                reject(`Network Error: Unable to reach the server.`); // Gestire errori di rete
            };

            xhr.send(JSON.stringify(options.data)); // Inviare i dati in formato JSON
        });
    };

    return {
        postCustom,
        getCustom
    };
})();

// Exporta il modulo
export default HttpUtils;