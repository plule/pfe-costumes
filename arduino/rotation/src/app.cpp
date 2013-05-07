#include <Arduino.h>
//#include "../../../interfaces/interfaces.h"
//#include "../../common/communication.h"

extern HardwareSerial Serial;
int i=0;
int ledOn=5;

int ledTest=6;
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
    unsigned long delta;
    pinMode(ledTest, OUTPUT);
    digitalWrite(ledTest, HIGH);
    delayMicroseconds(20);
    us = micros();
    pinMode(ledTest, INPUT);
    while(digitalRead(ledTest) == HIGH){};
    delta = micros() - us;
    Serial.println(delta);
}
/*
void handleMessage(MSG_TYPE type, int idMsg, int expe, HardwareSerial serial)
{
    //DBG("fio");
}
*/
