# ESP 32 WS LED control

This repository integrates the ESP32 with an interface to control LEDs via web service.

# Setup

1. Download and import [AsyncTCP](https://github.com/me-no-dev/AsyncTCP) [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) libraries on Arduino IDE.
2. Set wifi credentials.
3. Deploy
4. Access ESP's IP.
5. Be fun!

# Circuit

| RGB LED | ESP32 |
|---|---|
| Red | 19 * |
| Green | 23 * |
| Blue | 18 * |
| GND | GND |

> * I am using a 330 ohms resistor.

| I2C LCD Display | ESP32 |
|---|---|
| GND | GND |
| VCC | Vin |
| SDA | 22 |
| SCL | 21 |