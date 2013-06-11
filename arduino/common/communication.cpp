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
    Serial.setTimeout(4000);
    Serial.print("+++"); /* Enter xbee command mode (first bytes are often lost) */
    Serial.readStringUntil('\r'); /* Wait for XBee entering cmd mode (should be 3sec) and read "OK" */
    Serial.println("ATSL");
    Serial.readBytesUntil('\r', Id, 8);
    Id[8] = 0;
    Serial.println("ATCN"); /* Exit command mode */
    Serial.readStringUntil('\r');
    Serial.flush();
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

    /* Get message */
    char message[128];
    size_t size = Serial.readBytesUntil(MSG_SEP, message, 128);
    /*if(message[size-1] != MSG_SEP) {
        DBG("bad message");
        digitalWrite(13, LOW);
        return;
    }*/
    message[size] = 0;
    /* Parse message header */
    char *dest = strtok(message, ARG_SEP_S);
    char *expe = strtok(NULL, ARG_SEP_S);
    char *idMsg_s = strtok(NULL, ARG_SEP_S);
    char *type_s = strtok(NULL, ARG_SEP_S);
    if(dest == NULL || expe == NULL || idMsg_s == NULL || type_s == NULL) {
        digitalWrite(13, LOW);
        DBG("bad message");
        return;
    }
    int idMsg = atoi(idMsg_s);
    MSG_TYPE type = (MSG_TYPE)atoi(type_s);

    /* Parse arguments */
    char *parg = strtok(NULL, ARG_SEP_S);
    char *pargs[MAX_ARGS];
    int nargs = 0;
    while(parg != NULL && nargs < MAX_ARGS) {
        pargs[nargs] = parg;
        nargs++;
        parg = strtok(NULL, ARG_SEP_S);
    }

    if(type == MSG_DISCOVER)
        sendMessage(MSG_HELLO, 0, expe, Role);
    else if(strcmp(dest, Id) == 0 && type != MSG_ACK) {
        if(handleMessage(type, idMsg, expe, pargs, nargs))
            sendMessage(MSG_ACK, idMsg, expe);
    }
    digitalWrite(13, LOW);

    /*digitalWrite(13, HIGH);
    char dest[9] = {0};
    char expe[9] = {0};
    Serial.readBytesUntil(ARG_SEP, dest,9);
    Serial.readBytesUntil(ARG_SEP, expe,9);

    dest[8] = 0;
    expe[8] = 0;
    int idMsg = Serial.parseInt();
    MSG_TYPE type = (MSG_TYPE)Serial.parseInt();
    if(type == MSG_DISCOVER)
    {
        sendMessage(MSG_HELLO, 0, ARD_MASTER, Role);
    } else if(strcmp(dest, Id) == 0 && type != MSG_ACK)
    {
        if(handleMessage(type, idMsg, expe, Serial))
            sendMessage(MSG_ACK, idMsg, expe);
    }
    Serial.readStringUntil(MSG_SEP);
    digitalWrite(13,LOW);*/
}
