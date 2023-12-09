
const char* htmlPage = R"(
<!DOCTYPE html>
<html>
    <head>
    <title>HOME</title>
            <style>
            body {
                overflow: hidden;
            }

            /* Stile del toggle switch */
            .switch {
                position: relative;
                display: inline-block;
                width: 60px;
                height: 34px;
            }

            .switch input { 
                display: none;
            }

            .slider {
                position: absolute;
                cursor: pointer;
                top: 0;
                left: 0;
                right: 0;
                bottom: 0;
                background-color: #ccc;
                -webkit-transition: .4s;
                transition: .4s;
            }

            .slider:before {
                position: absolute;
                content: "";
                height: 26px;
                width: 26px;
                left: 4px;
                bottom: 4px;
                background-color: white;
                -webkit-transition: .4s;
                transition: .4s;
            }

            input:checked + .slider {
                background-color: #2196F3;
            }

            input:focus + .slider {
                box-shadow: 0 0 1px #2196F3;
            }

            input:checked + .slider:before {
                -webkit-transform: translateX(26px);
                -ms-transform: translateX(26px);
                transform: translateX(26px);
            }

            .slider.round {
                border-radius: 34px;
            }

            .slider.round:before {
                border-radius: 50%;
            }

            .character-container {
                position: relative;
                overflow: hidden;
                border: 2px solid black; 
                margin: 10 auto; 
                padding: 0;
            }

            .character {
                width: 99px;
                height: 100px;
                background-image: url('https://i.pinimg.com/originals/70/88/c8/7088c811b82defaa5fd8a2862751ed1c.gif');
                background-size: cover;
                position: relative;
                top: 50%;
                left: 0px;
                --X: -1;
                animation: walk 20s linear infinite;
            }

            @keyframes walk {
                0% {
                    left: 0;
                    transform: scaleX(calc(var(--X)));
                }
                50% {
                    left: calc(100% - 95px);
                    transform: scaleX(calc(var(--X)));
                }
                50.000001% {
                    transform: scaleX(calc(-1 * var(--X)));
                }

                99%{
                    left:0;
                    transform: scaleX(calc(-1 * var(--X)));
                
                100% {
                    left: 0;
                }
            }
        </style>
    </head>
    <body>
        <h1>%HOST_NAME%</h1>
        <div> 
            <!-- Toggle Switch -->
            <label class="switch">
                <input type="checkbox" id="toggleSwitch">
                <span class="slider round"></span>
            </label>
        <div>
        <p>Link utili:</p>
        <a href='/webserial'> Seriale </a>
        <p>     
            <div class="character-container">
            <div class="character"></div>
            </div>
        </p>

    
        <script>
        document.getElementById('toggleSwitch').addEventListener('click', function () {
        const toggleSwitch = document.getElementById('toggleSwitch');
        const isChecked = toggleSwitch.checked;

        const jsonData = {
        toggleValue: isChecked
        };

        // Invia il JSON al server locale
        fetch('/api/handleLed', {
        method: 'POST',
        headers: {
        'Content-Type': 'application/json'
        },
        body: JSON.stringify(jsonData)
        })
        .then(response => response.text())
        .then(data => {
        console.log('Risposta dal server:', data);
        let objectJson = JSON.parse(data);
        let sw = document.getElementById('toggleSwitch');
        console.log("switch: " + sw.checked);
        console.log("data: " + objectJson.lightLed);
        sw.checked = objectJson.lightLed;
        })
        .catch(error => {
        console.error('Errore:', error);
        });
        });
        </script>
    </body>
</html>
)";
