#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C

// Wi-Fi credentials
const char* ssid = "";
const char* password = "";

// Server endpoint
const String serverUrl = "http://ec2-16-171-28-128.eu-north-1.compute.amazonaws.com:8080/{1234}/report/fermentation";

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println(F("BME680 test"));

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

void loop() {
  if (!bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }

  // Read the temperature
  float temperature = bme.temperature;

  // Get the current time
  unsigned long currentTime = millis();

  // Print data to serial monitor
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" *C");

  // Make HTTP POST request
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);

    // Specify content-type header
    http.addHeader("Content-Type", "application/json");

    // Create JSON payload
    String jsonPayload = "{\"temperature\":";
    jsonPayload += String(temperature);
    jsonPayload += ", \"time\":";
    jsonPayload += String(currentTime);
    jsonPayload += "}";

    // Send HTTP POST request
    int httpResponseCode = http.POST(jsonPayload);

    // Check response
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    // Free resources
    http.end();
  }

  delay(2000);
}
