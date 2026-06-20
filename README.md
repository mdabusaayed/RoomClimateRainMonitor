# RoomClimateRainMonitor
IoT-Based Room Climate and Rain Monitoring System using ESP8266, DHT11, BMP280, MQTT, OLED Display, LEDs and Buzzer.

## Overview

This project monitors:

* Temperature (DHT11)
* Humidity (DHT11)
* Air Pressure (BMP280)
* Rain Status (Rain Sensor)

Data is displayed locally on the OLED display and published to HiveMQ using MQTT.

---

## Hardware

* AALeC V3 (ESP8266)
* DHT11 Sensor
* BMP280 Sensor
* Rain Sensor Module
* OLED Display
* NeoPixel LEDs
* Buzzer

---

## Wiring

| Device              | Pin    |
| ------------------- | ------ |
| DHT11               | D5     |
| Rain Sensor Digital | D6     |
| Rain Sensor Analog  | A0     |
| Buzzer              | GPIO15 |
| NeoPixel LEDs       | GPIO2  |
| OLED                | I²C    |

---

## MQTT Topics

* home/dht11/temperature
* home/dht11/humidity
* home/bmp280/pressure
* home/rain/status
* home/4sensors/datatogether

---

## Required Libraries

* ESP8266WiFi
* PubSubClient
* DHT Sensor Library
* Adafruit GFX
* Adafruit SH110X
* Adafruit NeoPixel
* Adafruit BMP280

---

## Upload Procedure

1. Connect the AALeC V3 board.
2. Install required libraries.
3. Set Wi-Fi credentials.
4. Upload the sketch.
5. Open Serial Monitor.
6. Subscribe to MQTT topics in HiveMQ.
