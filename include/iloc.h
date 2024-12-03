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
  uint32_t operand;
} iloc_register_t;

typedef union {
  iloc_register_t sources[2];
  iloc_register_t source;
  uint32_t operand;
} iloc_instruction_data_t;

typedef enum {
  REGISTER_REGISTER_DEST = 0,
  REGISTER_OPERAND_DEST = 1,
  REGISTER_DEST_OPERAND = 2,
  REGISTER_DEST = 3,
  OPERAND_DEST = 4
} iloc_instruction_type_t;

typedef struct {
  const char *mnemonic;
  iloc_instruction_type_t type;
  iloc_instruction_data_t data;
  iloc_register_t destination;
} iloc_instruction_t;
