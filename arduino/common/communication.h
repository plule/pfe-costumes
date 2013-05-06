#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include "../../interfaces/interfaces.h"
#include <stdint.h>
//#include <cstdio>
#include <Arduino.h>
#include <HardwareSerial.h>

#define DBG(msg) sendMessage(DEBUG, 0, ARD_MASTER, msg)

extern HardwareSerial Serial;

void sendMessage(MSG_TYPE type, int idMsg, int dest, int data);
void sendMessage(MSG_TYPE type, int idMsg, int dest, const char* data);
int getId();
void init_ard(ARD_ROLE role);
void handleMessage(MSG_TYPE type, int idMsg, int expe, HardwareSerial serial); // unimplemented !

#endif // COMMUNICATION_H
