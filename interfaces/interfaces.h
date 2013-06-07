#ifndef INTERFACES_H
#define INTERFACES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef uint8_t ard_size_t;
typedef char* ard_id_t;

#define MAX_ID 1000
#define MAX_ARGS 10
/*
 * Possible type of messages
 */
typedef enum {
    MSG_ACK, // Acknowledgement
    MSG_DONE, // Work is done
    MSG_DISCOVER, // Broadcast to find all arduinos
    MSG_HELLO, // Sent by a new arduino to identify itself
    MSG_SET_MORPHOLOGY, // Set position of a morphology motor
    MSG_GET_MORPHOLOGY, // Ask to get position of the morphology elements
    MSG_MORPHOLOGY, // Send the position of a morphology servo
    MSG_SET_ANGLE, // Set the rotation of the turntable
    MSG_TURN, // Do a complete rotation and notify angle evolution
    MSG_CANCEL_TURN, // Stop the complete rotation
    MSG_ANGLE, // Send current rotation angle
    MSG_DEBUG, // send a debug message
    MSG_INVALID // invalid message
} MSG_TYPE;

/*
 * Broadcast destination
 */
#define DEST_BROADCAST "0"

/*
 * Arduino's roles
 */
typedef enum {
    ROLE_ROTATION,
    ROLE_MORPHOLOGY
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
#define ARD_MASTER "1"

#define MSG_SEP '\n'
#define ARG_SEP '|'

#define MSG_SEP_S "\n"
#define ARG_SEP_S "|"

/* Morphology */
#define X(pin, define, string, umin, umax) define,
enum MORPHO_MOTORS {
    #include "morphology.h"
    MOTOR_NUMBER
};
#undef X

// Morphology distance (mm)
#define MORPHO_DISTANCE 50

#ifdef __cplusplus
} // extern c
#endif

#endif // INTERFACES_H
