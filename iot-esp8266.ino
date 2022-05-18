#include "ESP8266WiFi.h" // Enables the ESP8266 to connect to the local network (via WiFi)
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#define SensorPin A0  // used for Arduino and ESP8266

// WiFi
#define WIFI_SSID "Hemma"
#define WIFI_PASSWORD ""

// InfluxDB v2 server url
#define INFLUXDB_URL "https://eu-central-1-1.aws.cloud2.influxdata.com"
// InfluxDB v2 server or cloud API token
#define INFLUXDB_TOKEN "6MS-NflDiuvvndGjDoxgxAP2jFpZsMEnVvMgm5EyEhsUSQgIexaEVdb-QFB1H91Co0pG_frgLLwK-HH7OWi4Qw=="
// InfluxDB v2 organization id
#define INFLUXDB_ORG "idarosvall1@gmail.com"
// InfluxDB v2 bucket name
#define INFLUXDB_BUCKET "plant-data"
// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
#define TZ_INFO "CET-2CEST,M3.5.0,M10.5.0/3"

// InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Data point
Point sensor("plant");

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

void connect_influx() {
  // Add tags
  sensor.addTag("device", "ESP8266");

  // Accurate time is necessary for certificate validation and writing in batches
  // For the fastest time sync find NTP servers in your area: https://www.pool.ntp.org/zone/
  // Syncing progress and the time will be printed to Serial.
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

void setup() { 
  Serial.begin(115200);
  connect_WIFI();
  connect_influx();
}

void loop() {
  // Clear fields for reusing the point. Tags will remain untouched
  sensor.clearFields();

  // Read sensor value
  float sensorValue = analogRead(SensorPin);

  // Store measured value into point
  sensor.addField("moisture", sensorValue);

  // Print what is is written to influx
  Serial.print("Writing: ");
  Serial.println(sensor.toLineProtocol());

  // Write point to influx
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  
  delay(10000);
}
