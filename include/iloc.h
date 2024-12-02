#pragma once

#include <stdint.h>

#define SOURCE_REGISTERS 2
#define SOURCE_OPERANDS 2

typedef struct {
  const char *mnemonic;
  uint8_t source_registers[SOURCE_REGISTERS];
  int operands[SOURCE_OPERANDS];
  uint8_t destination_register;
} iloc_instruction_t;
