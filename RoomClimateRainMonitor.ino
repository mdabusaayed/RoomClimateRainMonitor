#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_BMP280.h>
#include "wifipassword.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

#define LED_PIN 2
#define LED_COUNT 5

#define BUZZER_PIN 15
const int rainAnalogPin = A0;
const int rainDigitalPin = D6;

#define DHTPIN D5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

Adafruit_BMP280 bmp;
bool bmpOK = false;

const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setAllPixels(uint32_t color) {
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP8266DHT-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT Connected");
    } else {
      Serial.print("MQTT Error: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void showDisplay(float temp, float hum, float pressure, bool isRaining, int analogValue) {
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);

  display.setTextSize(1);

  display.setCursor(0, 0);
  display.print("Temperature:");
  display.print(temp, 1);
  display.print((char)247);
  display.print("C");

  display.setCursor(0, 12);
  display.print("Humidity:");
  display.print((int)hum);
  display.print("%");

  display.setCursor(0, 24);
  display.print("RainAnalog:");
  display.print(analogValue);

  display.setCursor(0, 36);
  display.print("Pressure:");
  if (bmpOK) {
    display.print(pressure, 0);
    display.print("hPa");
  } else {
    display.print("--");
  }

  display.setTextSize(2);
  display.setCursor(12, 50);

  if (isRaining) {
    display.println("Raining!!");
  } else {
    display.println("No Rain");
  }

  display.display();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(rainDigitalPin, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  dht.begin();
  Wire.begin();

  if (!display.begin(OLED_ADDR, true)) {
    Serial.println("SH1106 init failed");
    while (true) delay(1000);
  }

  bmpOK = bmp.begin(0x76);

  strip.begin();
  strip.clear();
  strip.show();

  WiFi.begin(ssid, password);

  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");

  client.setServer(mqtt_server, 1883);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  int analogValue = analogRead(rainAnalogPin);
  int digitalValue = digitalRead(rainDigitalPin);
  bool isRaining = (digitalValue == LOW);

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  float pressure = bmpOK ? bmp.readPressure() / 100.0F : 0;

  if (!isnan(temp) && !isnan(hum)) {

    char t[10];
    char h[10];
    char p[10];

    dtostrf(temp, 4, 2, t);
    dtostrf(hum, 4, 2, h);
    dtostrf(pressure, 6, 2, p);

    const char* rainStatus = isRaining ? "Rain" : "No Rain";

    // Individual MQTT Topics
    client.publish("home/dht11/temperature", t);
    client.publish("home/dht11/humidity", h);

    if (bmpOK) {
      client.publish("home/bmp280/pressure", p);
    }

    client.publish("home/rain/status", rainStatus);

    // Combined MQTT Topic
    char allData[200];

snprintf(allData, sizeof(allData),
         "Temperature=%.1fC | Humidity=%d%%  ||  Pressure=%.1fhPa | %s",
         temp,
         (int)hum,
         pressure,
         rainStatus);

client.publish("home/4sensors/datatogether", allData);
client.publish("amiable", allData);

    // Serial Monitor Output
    Serial.print("Temperature: ");
    Serial.print(temp, 1);
    Serial.print(" °C");

    Serial.print(" | Humidity: ");
    Serial.print((int)hum);
    Serial.print("%");

    Serial.print(" | Air Pressure: ");

    if (bmpOK) {
      Serial.print(pressure, 1);
      Serial.print(" hPa");
    } else {
      Serial.print("BMP280 Error");
    }

    Serial.print(" | Rain Analog: ");
    Serial.print(analogValue);

    Serial.print(" | Digital State: ");
    Serial.print(isRaining ? "Raining Detected! 🌧️" : "Dry ☀️");

    Serial.print(" | Rain Status: ");
    Serial.print(rainStatus);

    if (isRaining) {
      Serial.print(" | Alarm ON 🌧️");

      tone(BUZZER_PIN, 880);
      setAllPixels(strip.Color(120, 0, 0));
    } else {
      Serial.print(" | Alarm OFF");

      noTone(BUZZER_PIN);
      setAllPixels(strip.Color(0, 120, 0));
    }

    Serial.println();
    Serial.println();

    showDisplay(temp, hum, pressure, isRaining, analogValue);

  } 
  
  else {
  Serial.println("DHT11 reading failed");
  Serial.println();

  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 10);
  display.println("Waiting for sensors..");

  display.display();
}

  delay(2000);
}