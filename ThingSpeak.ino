#include <WiFi.h>
#include <ThingSpeak.h>
#include "DHT.h"
const char* ssid = "";
const char* password = "";
WiFiClient client;
unsigned long DHT_CHID = 1071235;
const char* DHT_WriteKey = "J43NNU227KKDISB0";

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(115200);
  initWiFi();
  dht.begin();
  ThingSpeak.begin(client);
}
void initWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
}

void loop()
{
  float temp, humi;
  temp = dht.readTemperature();
  humi = dht.readHumidity();

  ThingSpeak.writeField(DHT_CHID, 1, temp, DHT_WriteKey);

  ThingSpeak.writeField(DHT_CHID, 2, humi, DHT_WriteKey);

  delay(15000);
}
