#pragma once

#include <stdint.h>

typedef enum {
  FRAME_POINTER = 0,
  GENERAL = 1
} iloc_register_type_t;

typedef struct {
  iloc_register_type_t type;
  uint8_t identifier;
  uint32_t operand;
} iloc_register_t;

typedef union {
  iloc_register_t sources[2];
  iloc_register_t source;
} iloc_instruction_data_t;

typedef enum {
  REGISTER_REGISTER_DEST = 0,
  REGISTER_OPERAND_DEST = 1,
  REGISTER_DEST_OPERAND = 2,
  REGISTER_DEST = 3
} iloc_instruction_type_t;

typedef struct {
  const char *mnemonic;
  iloc_instruction_type_t type;
  iloc_instruction_data_t data;
  iloc_register_t destination;
} iloc_instruction_t;
