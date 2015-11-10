#include "communication.h"

char Id[9] = {0};
char Name[NAME_SIZE+1] = {0};
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

void sendMessage(MSG_TYPE type, int idMsg, const char *dest, int data1, const char *data2)
{
    _initSendMessage(type, idMsg, dest);
    _sendArg(data1);
    _sendArg(data2);
    _endSendMessage();
}

void sendMessage(MSG_TYPE type, int idMsg, const char *dest, int data1, int data2, int data3)
{
    _initSendMessage(type, idMsg, dest);
    _sendArg(data1);
    _sendArg(data2);
    _sendArg(data3);
    _endSendMessage();
}

static uint16_t nameCrc(char *name) {
    uint16_t crc = 0;
    int i;
    for(i = 0; i < NAME_SIZE; i++) {
        if(name[i] == 0)
            break;
        crc = _crc16_update(crc, name[i]);
    }
    return crc;
}

void setName(char *name)
{
    for(int i = 0; i < NAME_SIZE; i++) { // Name[16] will always stay at 0
        Name[i] = name[i];
        eeprom_write_byte((uint8_t*)EEPROM_NAME+i,Name[i]);
    }
    eeprom_write_word(EEPROM_NAME_CRC, nameCrc(Name));
}

void loadName()
{
    for(int i = 0; i < NAME_SIZE; i++) {
        Name[i] = eeprom_read_byte(EEPROM_NAME+i);
    }
    uint16_t storedCrc = eeprom_read_word(EEPROM_NAME_CRC);
    if(storedCrc != nameCrc(Name))
        strcpy(Name, "Unnamed");
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
    loadName();
    Serial.setTimeout(200);
    sendMessage(MSG_HELLO, 0, ARD_MASTER, Role, Name);
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

    if(strcmp(expe, ARD_MASTER) == 0){ // Only answer to its master
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
            sendMessage(MSG_HELLO, 0, expe, Role, Name);
        else if(type == MSG_RENAME && strcmp(dest,Id) == 0) {
            sendMessage(MSG_ACK, idMsg, expe);
            setName(pargs[0]);
            DBG("renamed");
            DBG(Name);
        } else if(strcmp(dest, Id) == 0 && type != MSG_ACK) {
            if(handleMessage(type, idMsg, expe, pargs, nargs))
                sendMessage(MSG_ACK, idMsg, expe);
        }
    }
    digitalWrite(13, LOW);
}
