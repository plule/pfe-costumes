#include <Arduino.h>
#include "../../../interfaces/interfaces.h"

extern HardwareSerial Serial;

void setup()
{
    Serial.begin(9600);
}

void sendMessage(int type, int id, int dest, int expe)
{
    Serial.print(type);
    Serial.print(" ");
    Serial.print(id);
    Serial.print(" ");
    Serial.print(dest);
    Serial.print(" ");
    Serial.print(expe);
    Serial.print("\n");
}

void serialEvent() {
    int type = Serial.parseInt();
    int id = Serial.parseInt();
    int dest = Serial.parseInt();
    int expe = Serial.parseInt();
    Serial.flush();

    Serial.print("Message :");
    Serial.print(type);
    Serial.print(id);
    Serial.print(dest);
    Serial.println(expe);
}

void loop()
{
}
