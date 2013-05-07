#include <Arduino.h>
//#include "../../../interfaces/interfaces.h"
//#include "../../common/communication.h"

#define NBLED 8
#define LIMIT 500

extern HardwareSerial Serial;
int i=0;
int ledOn=5;

int leds[] = {6,7,8,9,10,11,12,13};
int values[8];
//int led=13;

void setup()
{
    //pinMode(led, OUTPUT);
    Serial.begin(9600);
    pinMode(ledOn, OUTPUT);
    digitalWrite(ledOn, HIGH);
    //digitalWrite(led, HIGH);
    //init_ard(ROTATION);
    //digitalWrite(led, LOW);
}

void loop()
{
    unsigned long us;
    int i;
    for(i=0; i < NBLED; i++) {
        int led = leds[i];
        pinMode(led, OUTPUT);
        digitalWrite(led, HIGH);
        delayMicroseconds(20);
        us = micros();
        pinMode(led, INPUT);
        while(digitalRead(led) == HIGH){};
        values[i] = ((micros()-us) > LIMIT);
        Serial.print(values[i]);
        Serial.print(" ");
    }
    Serial.println();
    delay(10);
}
/*
void handleMessage(MSG_TYPE type, int idMsg, int expe, HardwareSerial serial)
{
    //DBG("fio");
}
*/
