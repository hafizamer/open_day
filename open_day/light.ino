#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Wi-Fi credentials
const char* ssid = "FIRSTCITY"; // Replace with your Wi-Fi SSID
const char* password = "fcuc1234"; // Replace with your Wi-Fi Password

// ThingSpeak API details
const char* server = "http://api.thingspeak.com/update";
const char* apiKey = "M5WCOTP5HID1PGA0"; // Replace with your API key

// Light Sensor Pin
#define LIGHT_SENSOR_PIN D0

WiFiClient client;

void setup() {
  Serial.begin(115200);

  // Initialize Light Sensor
  pinMode(LIGHT_SENSOR_PIN, INPUT);

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected.");
}

void loop() {
  int sensorRawState = digitalRead(LIGHT_SENSOR_PIN);
  int lightState = sensorRawState == HIGH ? 0 : 1; // Light = 1, Dark = 0

  Serial.print("Light Sensor State (Processed): ");
  Serial.println(lightState);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = String(server) + "?api_key=" + apiKey + "&field1=" + String(lightState);
    http.begin(client, url); // Use WiFiClient instance with the URL

    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.println("Data sent successfully.");
      Serial.println("HTTP Response code: " + String(httpCode));
    } else {
      Serial.println("Error in sending data.");
    }

    http.end();
  } else {
    Serial.println("Wi-Fi not connected.");
  }

  delay(5000); // Send data every 5 seconds
}
