#include "../../common/communication.h"
#include <Servo.h>

//Servo servo;
Servo servos[MOTOR_NUMBER];


#define X(pin, define, string) pin,
int morpho_pins[] = {
    #include "../../../interfaces/morphology.h"
};
#undef X

int led=13;

void setup()
{
    pinMode(led, OUTPUT);
    digitalWrite(led, HIGH);
    init_ard(MORPHOLOGY);
    digitalWrite(led, LOW);
    int i;
    for(i=0; i<MOTOR_NUMBER; i++) {
        servos[i].attach(morpho_pins[i], MORPHO_MIN, MORPHO_MAX);
    }
    //servo.attach(52,800,2200);
}

void handleMessage(MSG_TYPE type, int idMsg, int expe, HardwareSerial serial)
{
    switch(type) {
    case COMMAND:
    {
        int motor = serial.parseInt();
        int time = serial.parseInt();
        if(motor < MOTOR_NUMBER) {
            servos[motor].writeMicroseconds(time);
        }
        break;
    }
    default:
        break;
    }
}

void loop()
{
}
