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
        $('.buttonWs2811SetEffect').on('click', this.sendStartEffect);
        $('.buttonWs2811StopEffect').on('click', this.sendStopEffect);
    },

    // deviceready Event Handler
    onDeviceReady() {
        // Cordova is now initialized. Have fun!
        console.log('Running cordova-' + cordova.platformId + '@' + cordova.version);
        $('.fieldIp').show();
        $('.changeIpBtn').show();
    },

    changeIp() {
        let test = /^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/.test($('.fieldIp')[0].value);
        if (test) {
            host = $('.fieldIp')[0].value;
            $('.label-ip').text("Indirizzo attuale: " + host);
        } else {
            $('.label-ip').text("Errore: Inserisci un ip valido");
        }
    },

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
    
        postCustom(url, queryParam, {});
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
        postCustom(url, queryParam,{});
    }


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

function postCustom(url, queryParam, postData) {
    url = host+url;
    url += (queryParam != "") ? ("?" + queryParam) : ("");
    cordova.plugin.http.sendRequest(url, {
        method: 'post',
        data: postData,
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded'
        }
    }, response => {
        console.log(response.status);
        console.log(response.data);
        alert("Success");
        $('#response').text('Response: ' + response.data);
    }, response => {
        console.error(response.error);
        alert('Error: ' + response.error);
    });

    // $.ajax({
    //     type: "POST",
    //     url: url,
    //     data: null, // No data in the request body
    //     success: function (response) {
    //         // Handle the success response
    //         console.log("Request successful:", response);
    //     },
    //     error: function (error) {
    //         // Handle errors
    //         console.error("Error:", error);
    //         console.log("Status code:" + error.status);
    //     }
    // });
}

