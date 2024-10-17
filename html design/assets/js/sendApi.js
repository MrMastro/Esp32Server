var host ="";
var apHost = "192.168.4.1";
var connected = true;

var hideIp = true;

function hexToRgb(hex) {
  // Expand shorthand form (e.g. "03F") to full form (e.g. "0033FF")
  var shorthandRegex = /^#?([a-f\d])([a-f\d])([a-f\d])$/i;
  hex = hex.replace(shorthandRegex, function(m, r, g, b) {
    return r + r + g + g + b + b;
  });

  var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
  return result ? {
    r: parseInt(result[1], 16),
    g: parseInt(result[2], 16),
    b: parseInt(result[3], 16)
  } : null;
};

function changeIp(){
    let test = /^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/.test($('.fieldIp')[0].value);
    if(!$('.fieldIp')[0].value){
        $('.label-ip').text("Inserisci un indirizzo ip");
    }
    else if(test){
        host = $('.fieldIp')[0].value;
        $('.label-ip').text("Indirizzo attuale: "+ host);
    }else{
        $('.label-ip').text("Inserisci un ip valido");
    }
}

function showSettings(){
    $('#Settings-Modal').modal('show');
    //$('#Settings-Modal').modal('hide');
}


$(document).ready(function () {
    host = window.location.host;
    $('.label-ip').text("Indirizzo attuale: "+ host);
    hideIp = $('#APConnection')[0].checked;
    if(hideIp){
        $('.fieldIp').hide();
        $('.changeIpBtn').hide();
        host = apHost;
        $('.label-ip').text("Indirizzo attuale: "+ host);
    }
    
    $('.fieldIp').on('input',changeIp);
    
    $('.buttonWs2811SetEffect').on('click', function () {
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

    url += (queryParam != "") ? ("?" + queryParam) : ("");

    $.ajax({
      type: "POST",
      url: url,
      data: null, // No data in the request body
      success: function (response) {
        // Handle the success response
        console.log("Request successful:", response);
      },
      error: function (error) {
        // Handle errors
        console.error("Error:", error);
        console.log("Status code:" + error.status);
      }
    });
  });
    
    $('.buttonWs2811StopEffect').on('click', function () {
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

    $.ajax({
      type: "POST",
      url: url,
      data: null, // No data in the request body
      success: function (response) {
        // Handle the success response
        console.log("Request successful:", response);
      },
      error: function (error) {
        // Handle errors
        console.error("Error:", error);
        console.log("Status code:" + error.status);
      }
    });
  });
    
    $('.buttonMemorizeInitialEffect').on('click', function () {
        console.log("WIP");
    });
    
    $('#APConnection').on('click',function () {
        let accessPointMode = $('#APConnection')[0].checked;
        if(accessPointMode){
            $('.fieldIp').hide();
            $('.changeIpBtn').hide();
            host = "192.168.4.1";
            $('.label-ip').text("Indirizzo attuale: "+ host);
        }else{
            $('.fieldIp').show();
            $('.changeIpBtn').show();
            changeIp();
        }
    });
    
    $('.settingsBtn').on('click',function(){
        showSettings();
    });
    
    $('.buttonMemorizeInitialEffect').hide();
    
});