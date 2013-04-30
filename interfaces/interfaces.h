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
    DATA
} MSG_TYPE;

struct ard_message {
    ard_size_t size;
    uint8_t type;
    uint16_t id;
    uint8_t dest;
    uint8_t expe;
    // union datas
};

#define ARD_MAX_SIZE (sizeof(ard_message))

#ifdef __cplusplus
} // extern c
#endif

#endif // INTERFACES_H
