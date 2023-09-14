#include"BluetoothSerial.h"
BluetoothSerial SerialBT;

int LED = 22;

void setup()
{
  Serial.begin(115200);
  SerialBT.begin("YJW");
  Serial.println("BT ON!");
  pinMode(LED, OUTPUT);
}

void loop()
{
  if (SerialBT.available())
  {
    char InD = SerialBT.read();
    if (InD == 'n')
    {
      digitalWrite(LED, HIGH);
    }
    else if (InD == 'f')
    {
      digitalWrite(LED, LOW);
    }
  }

}
