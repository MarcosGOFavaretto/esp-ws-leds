/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-web-server-slider-pwm/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/
// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "marcos";
const char* password = "10092722";

const int output = 2;

String sliderValue = "0";

// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

const char* PARAM_INPUT = "value";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="pt-BR">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Controle de LED</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            height: 100vh;
            background-color: #f0f0f0;
            color: #333;
        }

        h1 {
            font-size: 1.8rem;
            margin-bottom: 20px;
        }

        .switch {
            position: relative;
            display: inline-block;
            width: 60px;
            height: 34px;
            margin-bottom: 20px;
        }

        .switch input {
            opacity: 0;
            width: 0;
            height: 0;
        }

        .slider {
            position: absolute;
            cursor: pointer;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background-color: #ccc;
            transition: .4s;
            border-radius: 34px;
        }

        .slider:before {
            position: absolute;
            content: "";
            height: 26px;
            width: 26px;
            border-radius: 50%;
            left: 4px;
            bottom: 4px;
            background-color: white;
            transition: .4s;
        }

        input:checked+.slider {
            background-color: #2196F3;
        }

        input:checked+.slider:before {
            transform: translateX(26px);
        }

        .button {
            padding: 10px 20px;
            font-size: 1rem;
            color: #fff;
            background-color: #4CAF50;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            margin-top: 10px;
            opacity: 0.5;
            pointer-events: none;
        }

        .button.active {
            opacity: 1;
            pointer-events: auto;
        }

        .button:disabled {
            background-color: #a5a5a5;
        }
    </style>
</head>

<body>
    <h1>Controle o LED</h1>

    <p>Ligar</p>
    <label class="switch">
        <input type="checkbox" id="powerSwitch" onchange="toggleButton()">
        <span class="slider"></span>
    </label>

    <button id="blinkButton" class="button" disabled onclick="blinkLED()">Piscar</button>

    <script>
        function toggleButton() {
            var powerSwitch = document.getElementById('powerSwitch');
            var blinkButton = document.getElementById('blinkButton');
            if (powerSwitch.checked) {
                blinkButton.classList.add('active');
                blinkButton.disabled = false;

                var xhr = new XMLHttpRequest();
                xhr.open("GET", "/on", true);
                xhr.send();
            } else {
                blinkButton.classList.remove('active');
                blinkButton.disabled = true;

                var xhr = new XMLHttpRequest();
                xhr.open("GET", "/off", true);
                xhr.send();
            }
        }

        function blinkLED() {
            console.log('LED está piscando');
            // Enviar solicitação para piscar LED
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/blink", true);
            xhr.send();
        }
    </script>
</body>

</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var) {
  //Serial.println(var);
  if (var == "SLIDERVALUE") {
    return sliderValue;
  }
  return String();
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  // configure LED PWM functionalitites
  ledcAttachChannel(output, freq, resolution, ledChannel);

  ledcWrite(output, sliderValue.toInt());

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/on", HTTP_GET, [](AsyncWebServerRequest* request) {

    ledcWrite(LED_BUILTIN, HIGH);

    Serial.println("Led on!");
    request->send(200, "text/plain", "OK");
  });

  server.on("/off", HTTP_GET, [](AsyncWebServerRequest* request) {

    ledcWrite(LED_BUILTIN, LOW);

    Serial.println("Led off!");
    request->send(200, "text/plain", "OK");
  });

  // Start server
  server.begin();
}

void loop() {
}