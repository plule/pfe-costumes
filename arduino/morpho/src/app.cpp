#include "../../common/communication.h"
#include <Servo.h>
#include <avr/eeprom.h>

//Servo servo;
Servo servos[MOTOR_NUMBER];

Servo rotationMotor;

unsigned long lastSave;
unsigned long savePeriod = 2000;

#define EEPROM_SERVO 0


#define X(pin, define, string) pin,
int morpho_pins[] = {
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
        if(value != servos[i].readMicroseconds()) {
            DBG("Saving...");
            eeprom_write_word(servoAdress(i), servos[i].readMicroseconds());
        }
    }
}

void setup()
{
    pinMode(led, OUTPUT);
    digitalWrite(led, HIGH);
    init_ard(ROLE_MORPHOLOGY);
    digitalWrite(led, LOW);
    int i;
    for(i=0; i<MOTOR_NUMBER; i++) {
        int pos = eeprom_read_word(servoAdress(i));
        servos[i].attach(morpho_pins[i], MORPHO_MIN, MORPHO_MAX);
        servos[i].writeMicroseconds(pos);
        sendMessage(MSG_SERVO_POS,0,ARD_MASTER,i,pos);
    }
    rotationMotor.attach(30, MORPHO_MIN, MORPHO_MAX);
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
        int time = serial.parseInt();
        if(motor < MOTOR_NUMBER) {
            servos[motor].writeMicroseconds(time);
        }
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
            sendMessage(MSG_SERVO_POS,0,ARD_MASTER,i,servos[i].readMicroseconds());
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
