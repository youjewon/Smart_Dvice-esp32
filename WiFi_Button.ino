#include <WiFi.h>

const char* ssid     = "YSO";    // 연결할 SSID
const char* password = "";     // 연결할 SSID의 비밀번호
IPAddress ip;
int status = WL_IDLE_STATUS;
WiFiServer server(80);

const int LED = 23;
const int BUTTON = 15;
void setup()
{
  Serial.begin(115200);
  delay(10);
  WiFi.begin(ssid, password);
  pinMode(LED, OUTPUT);
  pinMode(BUTTON,INPUT);

  // 와이파이망에 연결
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Network connected.");
  server.begin();
  ip = WiFi.localIP();
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

// 연결 여부 로그 출력

void loop() {
  WiFiClient client = server.available();
  int bIN = digitalRead(BUTTON);
  if (client) {
    Serial.println("new client");
    String newLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (newLine) {
            Serial.println("Client Request Ended!");
            Serial.println("Web page transfer Start!");
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            client.println("<!DOCTYPE html>");
            client.println("<html>");
            client.println("<head >");
            client.println("<title >Network Info</title>");
            client.println("</head>");
            client.println("<body>");
            client.println("<h1 style=\"text-align: center;\">WiFi Connection Status</h1>");
            client.println("<p style=\"text-align: center;\" >IP Address : ");
            client.println(ip);
            client.println("</p>");     // ...  주소
            if(bIN == HIGH)
            {
              digitalWrite(LED,HIGH);
              client.println("<p style=\"text-align: center;\">button ON</p>");
            }
            else
            {
              digitalWrite(LED,LOW);
              client.println("<p style=\"text-align: center;\">button OFF</p>");
            }
            
            client.println("</body>");
            client.println("</html>");
            break;
          } else {
            newLine = "";
          }
        } else if (c != '\r') {
          newLine += c;
        }
      }
    }
    client.stop();
    Serial.println("client disonnected");
  }
}
