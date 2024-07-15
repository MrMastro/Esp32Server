var host ="";
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
    if(test){
        host = $('.fieldIp')[0].value;
        $('.label-ip').text("Indirizzo attuale: "+ host);
    }else{
        $('.label-ip').text("Errore: Inserisci un ip valido");
    }
}


$(document).ready(function () {
    host = window.location.host;
    $('.label-ip').text("Indirizzo attuale: "+ host);
    if(hideIp){
        $('.fieldIp').hide();
        $('.changeIpBtn').hide();
    }
    $('.fieldIp').on('input',changeIp);
    $('.changeIpBtn').on('click',changeIp);
    
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
});