#include "../../common/communication.h"
#include <Servo.h>
#include <avr/eeprom.h>

//Servo servo;
Servo rotationMotor;
int rotationAngle;

unsigned long lastSave;
unsigned long savePeriod = 2000;
bool completeTurn = false;
unsigned long completeTurnStart = 0;
int completeTurnAngle;
int completeTurnId;
int completeTurnTime;
int completeTurnStartAngle;


#define EEPROM_SERVO 0
#define EEPROM_ROTATION (uint16_t*)100

/*
 * Everything needed to configure a morphology slider
 */
typedef struct {
    int pin;
    /* umin to umax should correspond to 5 cm */
    int umin;
    int umax;
    uint16_t distance;
    Servo servo;
} ServoInfo;

/*
 * Array of morphology motors' infos
 */
#define X(pin, define, string, umin, umax) {pin, umin, umax, 0, Servo()},
ServoInfo morpho_motors[] = {
    #include "../../../interfaces/morphology.h"
};
#undef X


int led=13;

/*
 * Return a space to store the position of a motor in the eeprom
 */
uint16_t *servoAdress(int index)
{
    return (uint16_t *)EEPROM_SERVO+sizeof(int)*index;
}


void setAngle(int angle)
{
    if(angle != rotationAngle) {
        int ms = 1500 + (float)angle*232.55/360.0;
        rotationAngle = angle;
        rotationMotor.writeMicroseconds(ms);
    }
}

/*
 * Save in the eeprom the state of the modified morphology motors
 */
void saveState()
{
    int i;
    for(i=0; i<MOTOR_NUMBER; i++) {
        uint16_t value = eeprom_read_word(servoAdress(i));
        if(value != morpho_motors[i].distance) {
            eeprom_write_word(servoAdress(i), morpho_motors[i].distance);
        }
    }
    if(rotationAngle != eeprom_read_word(EEPROM_ROTATION))
        eeprom_write_word(EEPROM_ROTATION, rotationAngle);
}

/*
 * Modifiy the distance (in mm) of a servo.
 */
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
    init_ard(ROLE_MORPHOLOGY); // init the id and send the hello message
    digitalWrite(led, LOW);
    int i;
    for(i=0; i<MOTOR_NUMBER; i++) {
        uint16_t pos = eeprom_read_word(servoAdress(i));
        morpho_motors[i].servo.attach(morpho_motors[i].pin, morpho_motors[i].umin, morpho_motors[i].umax);
        setDistance(i, pos);
    }
    rotationMotor.attach(30, 1100, 1900);
    setAngle(eeprom_read_word(EEPROM_ROTATION));
    //rotationMotor.writeMicroseconds(eeprom_r);
}

unsigned long sendTime;
bool sent;
MSG_TYPE futureMessageType;
int futureMessageId;
char futureMessageDest[9];

/*
 * Wait "time" ms then send a message
 */
void sendMessageIn(int time, MSG_TYPE type, int id, char *dest)
{
    futureMessageType = type;
    futureMessageId = id;
    strcpy(futureMessageDest,dest);
    //futureMessageDest = dest;
    sent = false;
    sendTime = millis() + time;
}

/*
 * Cancel a complete turn
 */
void cancelCompleteTurn()
{
    if(completeTurn) {
        completeTurn = false;
        sendMessage(MSG_DONE, completeTurnId, ARD_MASTER);
    }
}


/*
 * React to a message (called by common communication.cpp file)
 */
bool handleMessage(MSG_TYPE type, int idMsg, char *expe, char **pargs, int nargs)
{
    bool ok = false;
    switch(type) {
    case MSG_SET_MORPHOLOGY:
    {
        if(nargs == 2) {
            int motor = atoi(pargs[0]);
            int distance = atoi(pargs[1]);
            ok = setDistance(motor, distance);
        }
        break;
    }
    case MSG_SET_ANGLE:
    {
        if(nargs == 1) {
            int angle = atoi(pargs[0]);
            cancelCompleteTurn();
            setAngle(angle);
            sendMessageIn(400,MSG_DONE, idMsg, expe);
            ok = true;
        }
        break;
    }
    case MSG_GET_MORPHOLOGY:
    {
        if(nargs == 0) {
            int i;
            for(i=0; i<MOTOR_NUMBER; i++) {
                sendMessage(MSG_MORPHOLOGY,0,ARD_MASTER,i,morpho_motors[i].distance);
            }
            ok = true;
        }
        break;
    }
    case MSG_TURN:
        DBG("turn");
        if(nargs == 2 && atoi(pargs[0]) > 0 && atoi(pargs[1]) >= 0 && atoi(pargs[1]) <= 360) {
            DBG("turnok");
            completeTurnStart = millis();
            completeTurnAngle = -1;
            completeTurnStartAngle = atoi(pargs[1]);
            completeTurnTime = atoi(pargs[0]);
            completeTurn = true;
            completeTurnId = idMsg;
            ok = true;
        }
        break;
    case MSG_CANCEL_TURN:
        DBG("cancel turn");
        if(nargs == 0) {
            cancelCompleteTurn();
            ok = true;
        }
    default:
        break;
    }
    return ok;
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
    if(completeTurn) {
        float angle = completeTurnStartAngle + 360*(millis()-completeTurnStart)/((float)completeTurnTime*1000.0);
        if(angle >= 360) {
            completeTurn = false;
            sendMessage(MSG_DONE, completeTurnId, ARD_MASTER);
        } else {
            setAngle(angle);
        }
        if((int)angle != completeTurnAngle) {
            completeTurnAngle = (int)angle;
            sendMessage(MSG_ANGLE, completeTurnId, ARD_MASTER, (int)completeTurnAngle);
        }
    }
}
