#include "../../common/communication.h"
#include <Servo.h>
#include <avr/eeprom.h>

//Servo servo;
Servo rotationMotor;

unsigned long lastSave;
unsigned long savePeriod = 2000;

#define EEPROM_SERVO 0

typedef struct {
    int pin;
    int umin;
    int umax;
    uint16_t distance;
    Servo servo;
} ServoInfo;

#define X(pin, define, string, umin, umax) {pin, umin, umax, 0, Servo()},
ServoInfo morpho_motors[] = {
    #include "../../../interfaces/morphology.h"
};
#undef X


int led=13;

uint16_t *servoAdress(int index)
{
    return (uint16_t *)EEPROM_SERVO+sizeof(int)*index;
}

void saveState()
{
    int i;
    for(i=0; i<MOTOR_NUMBER; i++) {
        uint16_t value = eeprom_read_word(servoAdress(i));
        if(value != morpho_motors[i].distance) {
            eeprom_write_word(servoAdress(i), morpho_motors[i].distance);
        }
    }
}

bool setDistance(int motor, uint16_t distance)
{
    if(distance > 0 && distance < MORPHO_DISTANCE && motor < MOTOR_NUMBER) {
        ServoInfo info = morpho_motors[motor];
        int time = info.umin + ((float)distance/(float)MORPHO_DISTANCE)*(info.umax - info.umin);
        info.servo.writeMicroseconds(time);
        morpho_motors[motor].distance = distance;
        return true;
    }
    return false;
}

void setup()
{
    pinMode(led, OUTPUT);
    digitalWrite(led, HIGH);
    init_ard(ROLE_MORPHOLOGY);
    digitalWrite(led, LOW);
    int i;
    for(i=0; i<MOTOR_NUMBER; i++) {
        uint16_t pos = eeprom_read_word(servoAdress(i));
        morpho_motors[i].servo.attach(morpho_motors[i].pin, morpho_motors[i].umin, morpho_motors[i].umax);
        setDistance(i, pos);
    }
    rotationMotor.attach(30, 800, 1300);
    rotationMotor.writeMicroseconds(1000);
}

unsigned long sendTime;
bool sent;
MSG_TYPE futureMessageType;
int futureMessageId;
char futureMessageDest[9];

void sendMessageIn(int time, MSG_TYPE type, int id, char *dest)
{
    futureMessageType = type;
    futureMessageId = id;
    strcpy(futureMessageDest,dest);
    //futureMessageDest = dest;
    sent = false;
    sendTime = millis() + time;
}

void handleMessage(MSG_TYPE type, int idMsg, char *expe, HardwareSerial serial)
{
    switch(type) {
    case MSG_MORPHOLOGY:
    {
        int motor = serial.parseInt();
        int distance = serial.parseInt();
        setDistance(motor, distance);
        break;
    }
    case MSG_ROTATION:
    {
        int angle = serial.parseInt();
        int ms = 1000 + (float)angle*211.66/360.0;
        rotationMotor.writeMicroseconds(ms);
        sendMessageIn(400,MSG_DONE, idMsg, expe);
        break;
    }
    case MSG_SERVO_POS:
    {
        int i;
        for(i=0; i<MOTOR_NUMBER; i++) {
            sendMessage(MSG_SERVO_POS,0,ARD_MASTER,i,morpho_motors[i].distance);
        }
        break;
    }
    default:
        break;
    }
}

void loop()
{
    unsigned long time = millis();
    if(time - lastSave > savePeriod) {
        saveState();
        lastSave = time;
    }
    if(time > sendTime && !sent) {
        sendMessage(futureMessageType, futureMessageId,futureMessageDest);
        sent = true;
    }
}
