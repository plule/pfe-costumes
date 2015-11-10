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
    MSG_SET_RAW_MOTOR, // Set motor position by giving microsec (for calibration only)
    MSG_GET_RAW_MOTOR_BOUNDS, // Ask to get raw position of the motors in microsec
    MSG_RAW_MOTOR_BOUNDS, // Send the raw position of a servomotor in microsec
    MSG_SET_CLOSE, // Set starting position of a motor
    MSG_SET_OPEN, // Set stopping position of a motor
    MSG_RENAME, // Set the name of a model
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
 * State of a motor
 */
#define NOT_CALIBRATED_MOTOR 0
#define CALIBRATED_MOTOR 1

/*
 * Id of the master
 */
#define ARD_MASTER "2"

#define MSG_SEP '\n'
#define ARG_SEP '|'

#define MSG_SEP_S "\n"
#define ARG_SEP_S "|"

#define NAME_SIZE 16

/* Morphology */
typedef enum {
    SINGLE_MOTOR,
    LEFT_MOTOR,
    RIGHT_MOTOR,
    FRONT_MOTOR,
    BACK_MOTOR,
    INVALID_MOTOR
} MOTOR_TYPE;

typedef enum {
    BUST_GROUP,
    WAIST_GROUP,
    HIPS_GROUP,
    SIZE_GROUP,
    INVALID_GROUP
} MOTOR_GROUP;

#define X(pin, role, define, string) define ## role,
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
