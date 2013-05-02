#ifndef INTERFACES_H
#define INTERFACES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef uint8_t ard_size_t;

/*
 * Possible type of messages
 */
typedef enum {
    COMMAND,
    ACK,
    DONE,
    DATA,
    DEBUG,
} MSG_TYPE;

/*
 * Content order in message
 */
typedef enum {
    ARG_TYPE,
    ARG_ID,
    ARG_DEST,
    ARG_EXPE,
    ARG_DATA,
    ARG_NUMBER
} MSG_CONTENT;

/*
 * Id of the master
 */
#define ARD_MASTER 1

#define MSG_SEP '\n'
#define ARG_SEP '|'

#ifdef __cplusplus
} // extern c
#endif

#endif // INTERFACES_H
