
#include <pgmspace.h>
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "DHT.h"

#define DHTPIN 4  //DHT11을 D4번핀에 연결한다.

#define DHTTYPE DHT11

#define SECRET
#define THINGNAME "Mtest"
// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC   "outTopic"
#define AWS_IOT_SUBSCRIBE_TOPIC "inTopic"

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

const char WIFI_SSID[] = "YSO";
const char WIFI_PASSWORD[] = "";
const char AWS_IOT_ENDPOINT[] = "a1k6sujl27ed0y-ats.iot.us-east-2.amazonaws.com";
long lastMsg = 0;
char msg[50];
String packet;
float Humi, Temp;

DHT dht(DHTPIN, DHTTYPE);


void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Create a message handler
  client.onMessage(messageHandler);

  Serial.print("Connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["time"] = millis();
  doc["sensor_a0"] = analogRead(0);
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void messageHandler(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  //  StaticJsonDocument<200> doc;
  //  deserializeJson(doc, payload);
  //  const char* message = doc["message"];
}

float getHumi() { //DHT11 습도를 받아오는 함수
  float h = dht.readHumidity();

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  return (h);
}

float getTemp() {//DHT11 온도를 받아오는 함수

  float t = dht.readTemperature();

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");

  return (t);
}
void mqtt_publish(float Humi, float Temp) {
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;

    packet = "Humidity : " + String(Humi) + "% " + "Temperature : " + String(Temp) + "*C" ;
    packet.toCharArray(msg, 50);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("DHT_data", msg);
    client.publish("Sensor/Humi_Temp", msg);
  }
  delay(5000);
}

void setup() {
  dht.begin();
  Serial.begin(9600);
  connectAWS();
}

void loop() {
  publishMessage();
  Humi = getHumi();
  Temp = getTemp(); 
  //mqtt_publish(Humi,Temp);
  client.loop();
  delay(1000);
}
