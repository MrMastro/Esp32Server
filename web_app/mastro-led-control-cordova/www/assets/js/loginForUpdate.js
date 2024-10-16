// $(document).ready(function () {
//   $('#submitLoginForUpdate').on('submit', function () {
//     let deviceName = $(".deviceNameInput")[0].value;
//     let password = $(".passwordInput")[0].value;

//     // Base64 encode the credentials
//     let credentials = btoa(deviceName + ':' + password);
//     var url = "/update";

//     $.ajax({
//         type: "GET",
//         url: url,
//         username: $(".deviceNameInput").val(),
//         password: $(".passwordInput").val(),
//         timeout: 0,
//         success: function (response) {
//             // Handle the success response
//             window.location.replace(url);
//         },
//         error: function (error) {
//             // Handle errors
//             console.error("Error:", error);
//             console.log("Status code:" + error.status);
//             $(".errorLogin").modal('show');
//         }
//     });
//   });
// });