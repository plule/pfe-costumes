#include "communication.h"

uint16_t Id = 0;
ARD_ROLE Role;

static void _sendArg(int arg)
{
    Serial.print(ARG_SEP);
    Serial.print(arg);
}

static void _sendArg(const char *arg)
{
    Serial.print(ARG_SEP);
    Serial.print(arg);
}

static void _initSendMessage(MSG_TYPE type, int idMsg, int dest)
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

void sendMessage(MSG_TYPE type, int idMsg, int dest)
{
    _initSendMessage(type, idMsg, dest);
    _endSendMessage();
}

void sendMessage(MSG_TYPE type, int idMsg, int dest, int data)
{
    _initSendMessage(type, idMsg, dest);
    _sendArg(data);
    _endSendMessage();
}

void sendMessage(MSG_TYPE type, int idMsg, int dest, int data1, int data2)
{
    _initSendMessage(type, idMsg, dest);
    _sendArg(data1);
    _sendArg(data2);
    _endSendMessage();
}

void sendMessage(MSG_TYPE type, int idMsg, int dest, const char* data)
{
    _initSendMessage(type, idMsg, dest);
    _sendArg(data);
    _endSendMessage();
}

/*
 * Read ATMY from XBee
 */
int getId()
{
    uint16_t id;
    char id_s[8];
    Serial.print("+++"); /* Enter xbee command mode */
    while(Serial.available() == 0); /* Wait for XBee entering cmd mode (should be 3sec) */
    Serial.readStringUntil('\n');

    Serial.println("ATSL");
    delay(10);
    Serial.readBytesUntil('\n', id_s, 8);
    sscanf(id_s, "%x", &id);
    //id = Serial.parseInt();

    Serial.println("ATCN"); /* Exit command mode */
    Serial.readStringUntil('\n');

    return id;
}


void init_ard(ARD_ROLE role)
{
    Serial.begin(9600);
    Role = role;
    Id = getId();
    sendMessage(HELLO, 0, ARD_MASTER, Role);
}

void serialEvent() {
    /* Got a message */
    int dest = Serial.parseInt();
    int expe = Serial.parseInt();
    int idMsg = Serial.parseInt();
    MSG_TYPE type = (MSG_TYPE)Serial.parseInt();
    if(type == DISCOVER) /* Server is looking for the arduinos */
    {
        sendMessage(HELLO, 0, ARD_MASTER, Role);
    } else if(dest == Id) /* Message is for me */
    {
        sendMessage(ACK, idMsg, expe, 0); /* Acknowledge reception */
        handleMessage(type, idMsg, expe, Serial);
    } else {
        DBG("Not for me");
    }
    Serial.readStringUntil(MSG_SEP);
}
