#ifndef INTERFACES_H
#define INTERFACES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef uint8_t ard_size_t;

typedef enum {
    COMMAND,
    ACK,
    DONE,
    DATA,
    DEBUG,
} MSG_TYPE;

#ifdef __cplusplus
} // extern c
#endif

#endif // INTERFACES_H
