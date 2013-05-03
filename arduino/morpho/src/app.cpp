#include "../../common/communication.h"
#include <Servo.h>

Servo servo;

int led=13;

void setup()
{
    pinMode(led, OUTPUT);
    digitalWrite(led, HIGH);
    init_ard(MORPHOLOGY);
    digitalWrite(led, LOW);
    servo.attach(52,800,2200);
}

void handleMessage(MSG_TYPE type, int idMsg, int expe, int data)
{
    switch(type) {
    case COMMAND:
        servo.writeMicroseconds(data);
        break;
    default:
        break;
    }
}

void loop()
{
}
