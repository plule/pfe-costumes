#ifndef INTERFACES_H
#define INTERFACES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef uint8_t ard_size_t;
typedef uint16_t ard_id_t;

/*
 * Possible type of messages
 */
typedef enum {
    HELLO,
    PING,
    DISCOVER,
    RENAME,
    COMMAND,
    ACK,
    DONE,
    DATA,
    DEBUG
} MSG_TYPE;

/*
 * Arduino's roles
 */
typedef enum {
    ROTATION,
    MORPHOLOGY
} ARD_ROLE;

/*
 * Content order in message
 */
typedef enum {
    ARG_DEST,
    ARG_EXPE,
    ARG_ID,
    ARG_TYPE,
    ARG_NUMBER
} MSG_CONTENT;

/*
 * Id of the master
 */
#define ARD_MASTER 1

#define MSG_SEP '\n'
#define ARG_SEP '|'


/* Morphology */
#define X(pin, define, string) define,
enum MORPHO_MOTORS {
    #include "morphology.h"
};
#undef X

#define MORPHO_MIN 800
#define MORPHO_MAX 2200

#ifdef __cplusplus
} // extern c
#endif

#endif // INTERFACES_H
