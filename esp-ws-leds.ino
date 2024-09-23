// Connection
#include <WiFi.h>

const char* ssid = "Favaretto - 2.4 GHz";
const char* password = "09102722";
//

// Web Server
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
//

// LCD
#include <LiquidCrystal_I2C.h>

#define LCD_COLS 16
#define LCD_LINES 2
#define LCD_ADDR 0x27

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_LINES);
//

// LED
#define LED_R 19
#define LED_G 23
#define LED_B 18
//

bool isLedBlinking = false;

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

        #messageInput {
            margin-top: 10px;
            padding: 10px;
            width: 200px;
            border-radius: 5px;
            border: 1px solid #ccc;
            outline: none;
        }

        #redButton {
            background-color: red;
            color: white;
        }

        #greenButton {
            background-color: #4CAF50;
            color: white;
        }

        #blueButton {
            background-color: blue;
            color: white;
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

    <button id="redButton" class="button" disabled onclick="red()">Vermelho</button>
    <button id="greenButton" class="button" disabled onclick="green()">Verde</button>
    <button id="blueButton" class="button" disabled onclick="blue()">Azul</button>
    <button id="blinkButton" class="button" disabled onclick="blinkLED()">Piscar</button>

    <input type="text" id="messageInput" placeholder="Digite sua mensagem" disabled>
    <button id="sendMessageButton" class="button" disabled onclick="sendMessage()">Enviar Mensagem</button>

    <script>
        function toggleButton() {
            var powerSwitch = document.getElementById('powerSwitch');
            var blinkButton = document.getElementById('blinkButton');
            var redButton = document.getElementById('redButton');
            var greenButton = document.getElementById('greenButton');
            var blueButton = document.getElementById('blueButton');
            var messageInput = document.getElementById('messageInput');
            var sendMessageButton = document.getElementById('sendMessageButton');

            if (powerSwitch.checked) {
                blinkButton.classList.add('active');
                blinkButton.disabled = false;
                redButton.classList.add('active');
                redButton.disabled = false;
                greenButton.classList.add('active');
                greenButton.disabled = false;
                blueButton.classList.add('active');
                blueButton.disabled = false;
                messageInput.disabled = false;
                sendMessageButton.classList.add('active');
                sendMessageButton.disabled = false;

                var xhr = new XMLHttpRequest();
                xhr.open("GET", "/on", true);
                xhr.send();
            } else {
                blinkButton.classList.remove('active');
                blinkButton.disabled = true;
                redButton.classList.remove('active');
                redButton.disabled = true;
                greenButton.classList.remove('active');
                greenButton.disabled = true;
                blueButton.classList.remove('active');
                blueButton.disabled = true;
                messageInput.disabled = true;
                sendMessageButton.classList.remove('active');
                sendMessageButton.disabled = true;

                var xhr = new XMLHttpRequest();
                xhr.open("GET", "/off", true);
                xhr.send();
            }
        }

        function blinkLED() {
            console.log('LED está piscando');
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/blink", true);
            xhr.send();
        }

        function red() {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/red", true);
            xhr.send();
        }

        function green() {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/green", true);
            xhr.send();
        }

        function blue() {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/blue", true);
            xhr.send();
        }

        function sendMessage() {
            var message = document.getElementById('messageInput').value;
            console.log('Mensagem enviada:', message);

            var xhr = new XMLHttpRequest();
            xhr.open("POST", "/message", true);
            xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
            var params = "message=" + encodeURIComponent(message);
            xhr.send(params);
        }
    </script>
</body>

</html>
)rawliteral";

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", index_html);
  });

  server.on("/on", HTTP_GET, [](AsyncWebServerRequest* request) {
    isLedBlinking = false;

    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, HIGH);

    lcd.backlight();

    Serial.println("Led on!");
    request->send(200, "text/plain", "OK");
  });

  server.on("/off", HTTP_GET, [](AsyncWebServerRequest* request) {
    isLedBlinking = false;

    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, LOW);

    lcd.noBacklight();

    Serial.println("Led off!");
    request->send(200, "text/plain", "OK");
  });

  server.on("/blink", HTTP_GET, [](AsyncWebServerRequest* request) {
    isLedBlinking = true;

    Serial.println("Led blinking!");
    request->send(200, "text/plain", "OK");
  });

  server.on("/red", HTTP_GET, [](AsyncWebServerRequest* request) {
    isLedBlinking = false;

    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, LOW);

    Serial.println("Red!");
    request->send(200, "text/plain", "OK");
  });

  server.on("/green", HTTP_GET, [](AsyncWebServerRequest* request) {
    isLedBlinking = false;

    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, LOW);

    Serial.println("Green!");
    request->send(200, "text/plain", "OK");
  });

  server.on("/blue", HTTP_GET, [](AsyncWebServerRequest* request) {
    isLedBlinking = false;

    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, HIGH);

    Serial.println("Blue!");
    request->send(200, "text/plain", "OK");
  });

  server.on("/message", HTTP_POST, [](AsyncWebServerRequest* request) {
    Serial.println("Message!");

    if (request->hasParam("message", true)) {
      String message = request->getParam("message", true)->value();
      Serial.println("Text: " + message);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(message);

      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "NOT OK");
    }
  });

  server.begin();

  // LCD
  lcd.init();
  //
}

void loop() {
  if (isLedBlinking) {
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, LOW);
    delay(1000);
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, HIGH);
    delay(1000);
  }
}