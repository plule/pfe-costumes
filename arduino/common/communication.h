#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include "../../interfaces/interfaces.h"
#include <stdint.h>
//#include <cstdio>
#include <Arduino.h>
#include <HardwareSerial.h>

#define DBG(msg) sendMessage(MSG_DEBUG, 0, ARD_MASTER, msg)

extern HardwareSerial Serial;

void sendMessage(MSG_TYPE type, int idMsg, const char *dest, int data);
void sendMessage(MSG_TYPE type, int idMsg, const char *dest);
void sendMessage(MSG_TYPE type, int idMsg, const char *dest, int data1, int data2);
void sendMessage(MSG_TYPE type, int idMsg, const char *dest, const char* data);
void getId();
void init_ard(ARD_ROLE role);
bool handleMessage(MSG_TYPE type, int idMsg, char *expe, HardwareSerial serial); // unimplemented !

#endif // COMMUNICATION_H
