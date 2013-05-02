#include <Arduino.h>
#include "../../../interfaces/interfaces.h"

extern HardwareSerial Serial;
int i=0;
int led=13;

void setup()
{
    pinMode(led, OUTPUT);
    digitalWrite(led, HIGH);
    Serial.begin(9600);
    digitalWrite(led, LOW);
}

void loop()
{
    Serial.println(i++);
    delay(500);
    digitalWrite(led, HIGH);
}
