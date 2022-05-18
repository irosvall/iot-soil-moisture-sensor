#include "ESP8266WiFi.h" // Enables the ESP8266 to connect to the local network (via WiFi)

#define SensorPin A0  // used for Arduino and ESP8266

// WiFi
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

// Gathered from WifiClient example setup for ESP8266
void connect_WIFI() {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() { 
  Serial.begin(115200);
  connect_WIFI();
}

void loop() {
  // Read sensor value
  float sensorValue = analogRead(SensorPin);
  Serial.println(sensorValue);

  delay(10000);
}
