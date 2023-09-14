#include <WiFiClientSecure.h>
//#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

# define DHTPIN 4 //DHT11 4번핀에 연결
# define DHTTYPE DHT11

// Replace the next variables with your SSID/Password combination
const char* ssid = "YSO";
const char* password = "";

const char* mqtt_server = "a1k6sujl27ed0y-ats.iot.us-east-2.amazonaws.com";

WiFiClientSecure espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

DHT dht(DHTPIN, DHTTYPE);

// 사물 인증서 (파일 이름: xxxxxxxxxx-certificate.pem.crt)
const char cert_str[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAMxGRxbBfB4MnMSjHFTEWXGvYbzIMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMDA2MjgxNjUz
MThaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDVl6njkaKEEZBc0ZlN
Tcm5eeqEC5tCkbiyMEmx0wreNOAkdgTBhiiMiEqgcYN8EU9kkqn4n5Y+/CbLwJxs
muxBf8Bp9Vaeuexvouet5PkuDV3ZiUkFfbCWM+rQ6j78tNoDmsOxsxxzqQKc5UkS
a7sFnSMqyyyF3cX/xR1zu2PqPdFMez37+FT1SaXNPaAVIR0g8rdlYnE9UnWU6x7N
p7zI85DK9D98GvYZ1zNvkX5OGDFxrNcujeNHa4+b9ekXD74yzs4yyRS3+aN2p1yn
X5bZEAlXzPWZi/znSMzP2SJ3VWq7wx/RavCLupTwDQpdrt1rS6UF1dQB7PNTk39B
zXmbAgMBAAGjYDBeMB8GA1UdIwQYMBaAFDLFxKMg0hwNvOaCY+TLhw/X7tiGMB0G
A1UdDgQWBBT5jWJQOn54uo1gw6eq072aFn3bCTAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAM1Zn6x2QVivHsQRl/NPlV36b
Z1CIhQgeBr8zepktok38Ra7R6vFtD0RZPaxJpMtwq+RKmtp93Vx9EM8WDtZMRek5
YkMkm372m8s9mwTEmb9ye8y2dtdLNLlZj6bsqy32Xgebv4jirSRYg0tmwGSI1Pqo
eKg4DxTxG2z1jJOANApMICt3/0a8wTO0d5LTzhiS061RPJtSWK5H7BTr4fIFnMj6
tprOQ674fJ8+Rt1ITlF4XnGAuchNasJjOHTu97QV2jmSGxCDhXU3Oxzg/PKBVKYL
vI/3oANxAo11TtyjdNGi6aHzoRN7b1EdV3NpKB0duAJ9olVzMjcyAxEnMXIgiQ==
-----END CERTIFICATE-----
)EOF";
// 사물 인증서 프라이빗 키 (파일 이름: xxxxxxxxxx-private.pem.key)
const char key_str[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEA1Zep45GihBGQXNGZTU3JuXnqhAubQpG4sjBJsdMK3jTgJHYE
wYYojIhKoHGDfBFPZJKp+J+WPvwmy8CcbJrsQX/AafVWnrnsb6LnreT5Lg1d2YlJ
BX2wljPq0Oo+/LTaA5rDsbMcc6kCnOVJEmu7BZ0jKssshd3F/8Udc7tj6j3RTHs9
+/hU9UmlzT2gFSEdIPK3ZWJxPVJ1lOsezae8yPOQyvQ/fBr2Gdczb5F+ThgxcazX
Lo3jR2uPm/XpFw++Ms7OMskUt/mjdqdcp1+W2RAJV8z1mYv850jMz9kid1Vqu8Mf
0Wrwi7qU8A0KXa7da0ulBdXUAezzU5N/Qc15mwIDAQABAoIBAQDLrnUzrg1sI9Om
r7mhHizf0oLwqKDGOnAGfEXKbZZPj9yu6Fxgr667VLzVg2n/RgKPxcyQap68MXhN
lL+WUaY6T7kNUgfoFSSC0LZd/Zwhbl2wtFoOAm4Czj9+OeMW2N+N0GgNbOAU63ay
Z3PnPRR7pf2DMXyjq3jFDxOm6Wu+UrW7nah3G+UrsTIL7HTvcsrQkDu5CNHe2mYz
eBXTC/MT+jWXE8n/U0Grdwv1uOTSdCrtFA6idTsXFdp8X2vgkRydXrf+RihZphm5
3XuIniJ6MPaAjcCrw/FiHaKPX+s4H4wLMOywBX9KEEz0Wsejov2aQRsThmHCU5WI
WPc3rl4BAoGBAPt4u1tm79Qz7JsL5DraJzX830UQaHNP9G1cgeBRc7Y9jI0uV/O3
QhvC+F9daKuoqWB8E4AUGNDqPuosMpmgJpMwGaA/N/PQBDWk9ll91+m1VK3t1pfY
3amm2jD0wivBo1Yt4ifkadXlcb0fi1E5YKtMQ2/+Kkfyy/f87RHNv0Y5AoGBANlw
T6ne0EjiTdiQMz9day4Kqqm9sO96GO6aVc5dp3t38rTkiQXTJWZWo1pgQ3RaWj9S
7rDuqR7lsNWZeLEekzDZkU6aTwlxiK8Y53RCdhDsVhg6n/qutDxJrWCN34nyFF+e
idVRE0L1ZKs6Y1Q3Zk87e1blMKM5gu6j7Zsh2F5zAoGAVw9QRe+YPi4PzwnJ5Pob
sxwtq/pnPuzCYY4yRJfOaJxcaZBO0Mh0MapJVLLBBxToNsgd9CyA/urqByZwCjGg
yTpQ1OKuG1G781p+EzN3KakmfOSnc4CYFSD3zxUdmAp84twVvNveLDg4LVKbZ+5m
YkeP3VjwCGKtbNh6trw1aTkCgYA2PiNRcCkrlXDwDoEKVKaxNK4AbDbgdugTVeMD
+S8XyQN+Buj70AGrdv+HQy5wrz2GQI0VDGSaNZiYoJGQNd1oOU5benkvFOiRLbhH
s3C6pgqQPse8BKsnwwWCacITKpXUiLs/wNr6QDdd09IFp1BZgbAyLDim72jvWWuj
4JyWawKBgQDrfEg4fZGOHf4j3oCbVUPfxfqfuAsEICG/l/JmICA2tfHMbF9ENju6
TwmyzWUjddsBh3/07MAwE+LdAoTuyw9lbuujdLzBj8Xt85Lf0euKCvWX+yJXbEEx
IWKjASMoREaDTpjf23dzNrFuMc7IMqLt5jVaYeiJ2WJXlnsJwVXThg==
-----END RSA PRIVATE KEY-----
)EOF";
// Amazon Trust Services(ATS) 엔드포인트 CA 인증서 (서버인증 > "RSA 2048비트 키: Amazon Root CA 1" 다운로드)
const char ca_str[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

void Wificonnect() {  // wifi연결
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("WiFi connecting!");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected!");
}

void reconnect() {  // 브로커에 연결하고 특정 토픽을 구독하는 함수로 loop함수에서 주기적으로 호출됨
  while(!client.connected()) {
    Serial.println("MQTT connecting...");
    if(client.connect("MQTT")){
      Serial.println("MQTT Connected!");
    }
    else {
      Serial.print("failed, rc = ");
      Serial.println(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  Wificonnect();
  espClient.setCACert(ca_str);  // 이것부터 밑에 두개까지는 인증서를 연결해줌
  espClient.setCertificate(cert_str);
  espClient.setPrivateKey(key_str);
  client.setServer(mqtt_server, 8883);  // 브로커의 ip와 포트를 통해 서버를 설정함
  //client.setCallback(callback);  // callback함수 설정
  
}

void loop() {
  if (! client.connected()) 
  { 
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    char tempString[8];
    dtostrf(t, 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);
    client.publish("Jewon/Temp001", tempString);

    char humString[8];
    dtostrf(h, 1, 2, humString);
    Serial.print("Humidity: ");
    Serial.println(humString);
    client.publish("Jewon/Humi001", humString);
  }
}
