#include <Arduino.h>
//#include "../../../interfaces/interfaces.h"
#include "../../common/communication.h"

#define NBLED 6
#define LIMIT 400

extern HardwareSerial Serial;
int i=0;
int ledOn=4;

int leds[] = {12,11,10,9,8,7,6,5};
unsigned char grayValue;
unsigned char value;
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

unsigned char grayToBinary(unsigned char num)
{
    unsigned char mask;
    for (mask = num >> 1; mask != 0; mask = mask >> 1)
    {
        num = num ^ mask;
    }
    return num;
}

void loop()
{
    unsigned long us;
    int i;

    grayValue = 0;
    for(i=0; i < NBLED; i++) {
        int led = leds[i];
        pinMode(led, OUTPUT);
        digitalWrite(led, HIGH);
        delayMicroseconds(20);
        us = micros();
        pinMode(led, INPUT);
        while(digitalRead(led) == HIGH){};
        unsigned long delta = micros() - us;
        Serial.println(delta);
        if(delta > LIMIT) {
            bitSet(grayValue, NBLED-i-1);
        } else {
            bitClear(grayValue, NBLED-i-1);
        }
    }
    //float angle = (float)grayToBinary(grayValue) / (float)pow(2,NBLED);
    Serial.println(grayToBinary(grayValue));//,3);
    delay(100);
}

bool handleMessage(MSG_TYPE type, int idMsg, int expe, HardwareSerial serial)
{
    return true;
    //DBG("fio");
}
