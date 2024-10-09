/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

// Wait for the deviceready event before using any of Cordova's device APIs.
// See https://cordova.apache.org/docs/en/latest/cordova/events/events.html#deviceready
var host = "";
var apHost = "192.168.4.1";
var connected = false;
var debug = true;

$('.fieldIp').hide();
$('.changeIpBtn').hide();

const app = {

    // Application Constructor
    initialize() {
        this.bindEvents();
        host = window.location.host;
        $('.label-ip').text("Indirizzo attuale: " + host);
    },

    // Bind Event Listeners
    bindEvents() {
        document.addEventListener('deviceready', this.onDeviceReady.bind(this), false);
        $('.fieldIp').on('input', this.changeIp);
        $('.changeIpBtn').on('click', this.changeIp);
        $('.buttonWs2811SetEffect').on('click', this.sendStartEffect.bind(this));
        $('.buttonWs2811StopEffect').on('click', this.sendStopEffect.bind(this));
        $('#APConnection').on('click', this.switchConnection);
        $('.buttonMemorizeInitialEffect').on('click', this.sendMemorizedInitialEffect);
    },

    // deviceready Event Handler
    onDeviceReady() {
        // Cordova is now initialized. Have fun!
        console.log('Running cordova-' + cordova.platformId + '@' + cordova.version);
        $('.buttonMemorizeInitialEffect').hide(); //todo show when the features is done
        $('.fieldIp').show();
        hideIp = $('#APConnection')[0].checked;
        if (hideIp) {
            $('.fieldIp').hide();
            $('.changeIpBtn').hide();
            host = apHost;
            $('.label-ip').text("Indirizzo attuale: " + host);
        }
    },

    switchConnection() {
        let accessPointMode = $('#APConnection')[0].checked;
        if (accessPointMode) {
            $('.fieldIp').hide();
            host = apHost;
            $('.label-ip').text("Indirizzo attuale: " + host);
        } else {
            $('.fieldIp').show();
            changeIp();
        }
    },

    changeIp() {
        let test = /^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/.test($('.fieldIp')[0].value);
        if (!$('.fieldIp')[0].value) {
            $('.label-ip').text("Inserisci un indirizzo ip");
        }
        else if (test) {
            host = $('.fieldIp')[0].value;
            $('.label-ip').text("Indirizzo attuale: " + host);
        } else {
            $('.label-ip').text("Inserisci un ip valido");
        }
    },

    showSettings() {
        $('#Settings-Modal').modal('show');
        //$('#Settings-Modal').modal('hide');
    },

    // METHOD TO API

    sendStartEffect() {
        let effect = $(".effectInput")[0].value; //CONSTANTS_UNIQUE_COLOR;
        let timing = $(".timingInput")[0].value; //100;
        let color = $(".colorInput")[0].value;
        let r = hexToRgb(color).r;
        let g = hexToRgb(color).g;
        let b = hexToRgb(color).b;
        let rgbAction = $("#rgbCheck")[0].checked;
        let ws2811Action = $("#ws2811Check")[0].checked;

        var url = "/api/ws2811SetEffect";
        var queryParam =
            "effect=" + encodeURIComponent(effect) +
            "&timing=" + encodeURIComponent(timing) +
            "&color=" + encodeURIComponent(color) +
            "&r=" + encodeURIComponent(r) +
            "&g=" + encodeURIComponent(g) +
            "&b=" + encodeURIComponent(b) +
            "&rgbAction=" + encodeURIComponent(rgbAction) +
            "&ws2811Action=" + encodeURIComponent(ws2811Action);

        postCustom(url, queryParam, {}, (response) => this.genericSuccess(response), (err) => this.genericFailure(err));
    },

    sendStopEffect() {
        let effect = $(".effectInput")[0].value; //CONSTANTS_UNIQUE_COLOR;
        let timing = $(".timingInput")[0].value; //100;
        let color = $(".colorInput")[0].value;
        let r = hexToRgb(color).r;
        let g = hexToRgb(color).g;
        let b = hexToRgb(color).b;
        let rgbAction = $("#rgbCheck")[0].checked;
        let ws2811Action = $("#ws2811Check")[0].checked;

        var url = "/api/ws2811StopEffect";
        var queryParam =
            "effect=" + encodeURIComponent(effect) +
            "&timing=" + encodeURIComponent(timing) +
            "&color=" + encodeURIComponent(color) +
            "&r=" + encodeURIComponent(r) +
            "&g=" + encodeURIComponent(g) +
            "&b=" + encodeURIComponent(b) +
            "&rgbAction=" + encodeURIComponent(rgbAction) +
            "&ws2811Action=" + encodeURIComponent(ws2811Action);

        url += (queryParam != "") ? ("?" + queryParam) : ("");
        postCustom(url, queryParam, {}, (response) => this.genericSuccess(response), (err) => this.genericFailure(err));
    },

    sendMemorizedInitialEffect() {
        console.log("WIP");
    },

    login(deviceName, devicePassword) {
        console.log("WIP");
    },

    // SUCCESS AND FAILURE METHOD
    genericSuccess(param) {
        console.log("GenericSuccess:\n" + param);
        alert("Operazione effettuata");
    },

    genericFailure(param) {
        console.log("GenericFailure:\n" + param);
        alert("Operazione fallita");
    },


}

// Initialize the app
$(document).ready(() => {
    app.initialize();
});



// UTILS:

function hexToRgb(hex) {
    // Expand shorthand form (e.g. "03F") to full form (e.g. "0033FF")
    var shorthandRegex = /^#?([a-f\d])([a-f\d])([a-f\d])$/i;
    hex = hex.replace(shorthandRegex, function (m, r, g, b) {
        return r + r + g + g + b + b;
    });

    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
        r: parseInt(result[1], 16),
        g: parseInt(result[2], 16),
        b: parseInt(result[3], 16)
    } : null;
}

function postCustom(path, queryParam, postData, callBackSuccess, callBackFailure) {
    url = host + path;
    //let body = queryParam;
    let options = {
        method: "post",
        headers: { "Accept": "application/json" },
        data: postData,
        param: queryParam
    };

    if (cordova.platformId == 'browser') {
        callBackFailure("Applicazione non disponibile per il browser");
        postWithBrowserCors(url, options, callBackSuccess, callBackFailure);
    } else {
        postWithAndroid(url, options, callBackSuccess, callBackFailure);
    }
}

function postWithAndroid(url, options, callBackSuccess, callBackFailure) {
    if(options.param){
        url = "http://" + url + "?" + options.param;
    }else{
        url = "http://" + url;
    }
    cordova.plugin.http.sendRequest(url, options, (response) => {
        // success
        if (debug) {
            alert("DEBUG SUCCESS: \n" + response);
        }
        callBackSuccess(response);
    }, (err) => {
        // error
        if (debug) {
            alert("DEBUG ERROR: \n" + err);
        }
        callBackFailure(err);
        console.log(err.status);
        console.log(err.error);
    });
}

//Don't work
function postWithBrowser(url, options, callBackSuccess, callBackFailure) {
    if(options.param){
        url = "http://" + url + "?" + options.param;
    }else{
        url = "http://" + url;
    }
    const xhr = new XMLHttpRequest();

    // Initialize the XMLHttpRequest
    xhr.open('POST', url, true);
    xhr.setRequestHeader('Content-Type', 'application/json');
    // Define the callback for when the request state changes
    xhr.onreadystatechange = function () {
        if (xhr.readyState === XMLHttpRequest.DONE) {
            // Check if the request was successful
            if (xhr.status === 200) {
                // Parse the response if needed, then call the success callback
                const response = JSON.parse(xhr.responseText);
                callBackSuccess(response);
            } else {
                // Call the failure callback with an error message
                callBackFailure(`Error: ${xhr.status} - ${xhr.statusText}`);
            }
        }
    };
    // Convert the data object to JSON format and send the request
    xhr.send(JSON.stringify(options.data));
}

function postWithBrowserCors(url, options, callBackSuccess, callBackFailure) {
    if(options.param){
        url = "http://" + url + "?" + options.param;
    }else{
        url = "http://" + url;
    }
    const xhr = new XMLHttpRequest();

    xhr.open('POST', url, true);
    xhr.setRequestHeader('Content-Type', 'application/json');

    // Handle CORS
    xhr.onload = function () {
        // Check if the response has the CORS headers
        const accessControlAllowOrigin = xhr.getResponseHeader('Access-Control-Allow-Origin');
        if (accessControlAllowOrigin) {
            console.log("CORS header present:", accessControlAllowOrigin);
        } else {
            console.warn("No CORS header present in the response.");
        }

        if (xhr.status === 200) {
            const response = JSON.parse(xhr.responseText);
            callBackSuccess(response); // Call the success callback with the response
        } else {
            callBackFailure(`Error: ${xhr.status} - ${xhr.statusText}`); // Call the failure callback
        }
    };

    // Handle network errors
    xhr.onerror = function () {
        callBackFailure(`Network Error: Unable to reach the server.`);
    };

    // Send the request with the postData converted to JSON
    xhr.send(JSON.stringify(options.data));
}