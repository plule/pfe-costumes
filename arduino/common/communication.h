#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include "../../interfaces/interfaces.h"
#include <stdint.h>
//#include <cstdio>
#include <Arduino.h>
#include <util/crc16.h>
#include <avr/eeprom.h>
#include <HardwareSerial.h>

#define DBG(msg) sendMessage(MSG_DEBUG, 0, ARD_MASTER, msg)

#define EEPROM_NAME (uint8_t*)0
#define EEPROM_NAME_CRC (uint16_t*)(EEPROM_NAME+NAME_SIZE)
#define EEPROM_ROTATION (uint16_t*)(EEPROM_NAME_CRC+sizeof(uint16_t))
#define EEPROM_SERVO (uint16_t*)(EEPROM_ROTATION+sizeof(uint16_t))

extern HardwareSerial Serial;

void sendMessage(MSG_TYPE type, int idMsg, const char *dest, int data);
void sendMessage(MSG_TYPE type, int idMsg, const char *dest);
void sendMessage(MSG_TYPE type, int idMsg, const char *dest, int data1, int data2);
void sendMessage(MSG_TYPE type, int idMsg, const char *dest, const char* data);
void sendMessage(MSG_TYPE type, int idMsg, const char *dest, int data1, const char *data2);
void getId();
void init_ard(ARD_ROLE role);
bool handleMessage(MSG_TYPE type, int idMsg, char *expe, char **pargs, int nargs); // unimplemented !

#endif // COMMUNICATION_H
