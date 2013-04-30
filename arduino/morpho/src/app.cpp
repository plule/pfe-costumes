#include <Arduino.h>
extern HardwareSerial Serial;
void setup()
{
    Serial.begin(9600);
}

void loop()
{
    Serial.println("Hello World");
    delay(1000);
}
