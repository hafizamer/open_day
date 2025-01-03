#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "FIRSTCITY";
const char* password = "fcuc1234";

// OpenWeatherMap API
const char* openWeatherMapApiKey = "f804c07cdd20afb0eeb75ad6f495dbc3";
const char* city = "TOKYO";
const char* country = "JP";
String openWeatherUrl = "http://api.openweathermap.org/data/2.5/weather?q=" + String(city) + "," + String(country) + "&appid=" + String(openWeatherMapApiKey) + "&units=metric";

// ThingSpeak API
const char* thingSpeakApiKey = "Y92K4DSGTF5RP311";
String thingSpeakUrl = "http://api.thingspeak.com/update";

// RGB LED Pins
int greenPin = D5;  // First LED (Green)
int redPin = D6;    // Second LED (Red)

WiFiClient client;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Fetch weather data
    http.begin(client, openWeatherUrl); // Use WiFiClient object
    int httpResponseCode = http.GET();

    if (httpResponseCode == 200) {
      String payload = http.getString();
      Serial.println(payload);

      // Parse temperature from JSON response
      StaticJsonDocument<1024> doc;
      deserializeJson(doc, payload);

      float temperature = doc["main"]["temp"];
      Serial.print("Temperature: ");
      Serial.println(temperature);

      // Control LEDs based on temperature
      if (temperature > 20) {
        // Turn on red LED, turn off green LED
        digitalWrite(redPin, HIGH);  
        digitalWrite(greenPin, LOW);
      } else {
        // Turn on green LED, turn off red LED
        digitalWrite(greenPin, HIGH); 
        digitalWrite(redPin, LOW);
      }

      // Send temperature to ThingSpeak
      sendToThingSpeak(temperature);

    } else {
      Serial.println("Error fetching weather data");
    }
    http.end();
  }
  delay(60000); // Update every minute
}

void sendToThingSpeak(float temperature) {
  HTTPClient http;
  String url = thingSpeakUrl + "?api_key=" + String(thingSpeakApiKey) + "&field1=" + String(temperature);
  http.begin(client, url); // Use WiFiClient object
  int httpResponseCode = http.GET();
  Serial.print("ThingSpeak Response: ");
  Serial.println(httpResponseCode);
  http.end();
}
