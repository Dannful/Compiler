#pragma once

#include <stdint.h>
#include <stdio.h>

#include "iloc.h"

#define INITIAL_WRITER_SIZE 64

typedef struct {
    uint8_t *buffer;
    size_t length;
    size_t capacity;
} Writer;

Writer *create_writer();
void destroy_writer(Writer *writer);

void write_bytes(Writer *writer, const void *buf, size_t count);
void write_string(Writer *writer, const char *text);
void write_constant(Writer *writer, int c);
void write_register(Writer *writer, iloc_register_t reg);
void write_label(Writer *writer, label_identifier_t label);
void write_operand_separator(Writer *writer);
void write_branch_separator(Writer *writer);
void write_standard_separator(Writer *writer);
void finish_line(Writer *writer);
void finish_word(Writer *writer);
