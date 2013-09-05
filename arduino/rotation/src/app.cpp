#include "../../common/communication.h"

int relayPin = 7;
int led = 13;

void setup()
{
    pinMode(led, OUTPUT);
    pinMode(relayPin, OUTPUT);
    digitalWrite(led, HIGH);
    init_ard(ROLE_ROTATION); // init the id and send the hello message
    digitalWrite(led, LOW);
}

/*
 * React to a message (called by common communication.cpp file)
 */
bool handleMessage(MSG_TYPE type, int idMsg, char *expe, char **pargs, int nargs)
{
    bool ok = false;
    switch(type) {
    case MSG_TURN:
        ok = true;
        digitalWrite(relayPin, HIGH);
        break;
    case MSG_CANCEL_TURN:
        ok = true;
        digitalWrite(relayPin, LOW);
        break;
    default:
        break;
    }
    return ok;
}

void loop()
{
}
