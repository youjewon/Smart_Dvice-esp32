//Smart Dvice First study 

// ex esp32_Blink

int LED_BUILTIN_LED =2; // Default 

void setup()
{
  pinMode(LED_BUILTIN,OUTPUT);
}

void loop()
{
  digitalWrite(LED_BUILTIN,HIGH);
  delay(1000);                  // delay 1 second
  digitalWrite(LED_BUILTIN,LOW);
  delay(1000);                   // delay 1 second

}
