#pragma once

#include <stdint.h>

typedef struct {
  uint8_t register;
  int operand;
} iloc_register_operand_t;

typedef union {
  uint8_t source_registers[2];
  uint8_t source_register;
  iloc_register_operand_t register_operand;
} iloc_instruction_data_t;

typedef enum {
  REGISTER_REGISTER_DEST = 0,
  REGISTER_OPERAND_DEST = 1,
  REGISTER_DEST_OPERAND = 2,
  REGISTER_DEST = 3
} iloc_instruction_type_t;

typedef struct {
  const char *mnemonic;
  iloc_instruction_type type;
  iloc_instruction_data_t data;
  uint8_t destination_register;
} iloc_instruction_t;
