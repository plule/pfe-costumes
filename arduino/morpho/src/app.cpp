#include <Arduino.h>

#include "../../../interfaces/interfaces.h"

#define DBG(msg) sendMessage(DEBUG, 0, ARD_MASTER, msg)

extern HardwareSerial Serial;

int id=0;
int led=13;

/*
 * Read ATMY from XBee
 */
int getId()
{
    int id;
    Serial.print("+++"); /* Enter xbee command mode */
    while(Serial.available() == 0); /* Wait for XBee entering cmd mode (should be 3sec) */
    Serial.readStringUntil('\n');

    Serial.println("ATMY"); /* Read ATMY */
    id = Serial.parseInt();
    Serial.readStringUntil('\n');

    Serial.println("ATCL"); /* Exit command mode */
    Serial.readStringUntil('\n');

    return id;
}

void setup()
{
    pinMode(led, OUTPUT);
    Serial.begin(9600);
    id = getId();
}

void sendMessage(MSG_TYPE type, int idMsg, int dest, char *data)
{
    Serial.print(type);
    Serial.print(ARG_SEP);
    Serial.print(idMsg);
    Serial.print(ARG_SEP);
    Serial.print(dest);
    Serial.print(ARG_SEP);
    Serial.print(id);
    Serial.print(ARG_SEP);
    Serial.print(data);
    Serial.print(MSG_SEP);
}

void handleMessage(MSG_TYPE type, int idMsg, int expe)
{
    DBG("Got your message");
}

void serialEvent() {
    /* Got a message */
    MSG_TYPE type = (MSG_TYPE)Serial.parseInt();
    int idMsg = Serial.parseInt();
    int dest = Serial.parseInt();
    int expe = Serial.parseInt();
    Serial.readStringUntil(MSG_SEP);

    if(dest == id) /* check if message is for me */
    {
        sendMessage(ACK, idMsg, expe, ""); /* Acknowledge reception */
        handleMessage(type, idMsg, expe);
    } else
    {
        DBG("Not for me");
    }
}

void loop()
{
}
