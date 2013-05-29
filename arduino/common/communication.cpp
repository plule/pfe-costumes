#include "communication.h"

char Id[9] = {0};
ARD_ROLE Role;

static void _sendArg(uint32_t arg)
{
    Serial.print(ARG_SEP);
    Serial.print(arg);
}

static void _sendArg(const char *arg)
{
    Serial.print(ARG_SEP);
    Serial.print(arg);
}

static void _initSendMessage(MSG_TYPE type, int idMsg, const char *dest)
{
    Serial.print(dest);
    _sendArg(Id);
    _sendArg(idMsg);
    _sendArg(type);
}

static void _endSendMessage()
{
    Serial.print(MSG_SEP);
}

void sendMessage(MSG_TYPE type, int idMsg, const char *dest)
{
    _initSendMessage(type, idMsg, dest);
    _endSendMessage();
}

void sendMessage(MSG_TYPE type, int idMsg, const char *dest, int data)
{
    _initSendMessage(type, idMsg, dest);
    _sendArg(data);
    _endSendMessage();
}

void sendMessage(MSG_TYPE type, int idMsg, const char *dest, int data1, int data2)
{
    _initSendMessage(type, idMsg, dest);
    _sendArg(data1);
    _sendArg(data2);
    _endSendMessage();
}

void sendMessage(MSG_TYPE type, int idMsg, const char *dest, const char* data)
{
    _initSendMessage(type, idMsg, dest);
    _sendArg(data);
    _endSendMessage();
}

/*
 * Read ATMY from XBee
 */
void getId()
{
    uint32_t id;
    Serial.setTimeout(3000);
    Serial.print("+++"); /* Enter xbee command mode */
    Serial.readStringUntil('\r'); /* Wait for XBee entering cmd mode (should be 3sec) and read "OK" */

    Serial.println("ATSL");
    Serial.readBytesUntil('\r', Id, 8);
    Id[8] = 0;
    Serial.println("ATCN"); /* Exit command mode */
    Serial.readStringUntil('\r');
}


void init_ard(ARD_ROLE role)
{
    Serial.begin(9600);
    Role = role;
    getId();
    Serial.setTimeout(200);
    sendMessage(MSG_HELLO, 0, ARD_MASTER, Role);
}

void serialEvent() {
    /* Got a message */
    digitalWrite(13, HIGH);
    char dest[9] = {0};
    char expe[9] = {0};
    Serial.readBytesUntil(ARG_SEP, dest,9);
    Serial.readBytesUntil(ARG_SEP, expe,9);
    dest[8] = 0;
    expe[8] = 0;
    int idMsg = Serial.parseInt();
    MSG_TYPE type = (MSG_TYPE)Serial.parseInt();
    if(type == MSG_DISCOVER) /* Server is looking for the arduinos */
    {
        sendMessage(MSG_HELLO, 0, ARD_MASTER, Role);
    } else if(strcmp(dest, Id) == 0 && type != MSG_ACK) /* Message is for me */
    {
        if(handleMessage(type, idMsg, expe, Serial))
            sendMessage(MSG_ACK, idMsg, expe, 0); /* Acknowledge reception */
    } else if(strcmp(dest, Id) != 0){
        DBG("Not for me");
    }
    Serial.readStringUntil(MSG_SEP);
    digitalWrite(13,LOW);
}
