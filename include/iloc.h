#pragma once

#include <stdint.h>

typedef uint8_t register_identifier_t;
typedef uint32_t label_identifier_t;

typedef enum {
    FRAME_POINTER = 0,
    GENERAL = 1
} iloc_register_type_t;

typedef struct {
    iloc_register_type_t type;
    register_identifier_t identifier;
} iloc_register_t;

typedef enum {
    SREG_SREG_DREG = 0,
    SREG_OPERAND_DREG = 1,
    SREG_DREG_OPERAND = 2,
    SREG_DREG = 3,
    OPERAND_DREG = 4,
    LABEL = 5,
    SREG_LABEL_LABEL = 6,
    JUMP = 7
} iloc_instruction_type_t;

typedef struct {
    iloc_register_t source;
    int value;
} register_and_value;

typedef struct {
    const char *mnemonic;
    iloc_instruction_type_t type;
    union {
        iloc_register_t sources[2];
        iloc_register_t source;
        int value;
        label_identifier_t label;
        register_and_value reg_and_value;
    } operands;
    union {
        iloc_register_t reg;
        iloc_register_t registers[2];
        label_identifier_t label;
        label_identifier_t labels[2];
        register_and_value reg_and_value;
    } destination;
} iloc_instruction_t;
