#include "./writer.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define REGISTER_COUNT 6

Writer *create_writer() {
    Writer *writer = malloc(sizeof(Writer));
    if (writer == NULL)
        return NULL;
    writer->buffer = calloc(sizeof(uint8_t), INITIAL_WRITER_SIZE);
    if (writer->buffer == NULL)
        return NULL;
    writer->capacity = INITIAL_WRITER_SIZE;
    writer->length = 0;
    return writer;
}

void destroy_writer(Writer *writer) {
    free(writer->buffer);
    free(writer);
}

void write_bytes(Writer *writer, const void *buf, size_t count) {
    if (writer->length + count > writer->capacity) {
        writer->buffer = realloc(writer->buffer, writer->capacity + count + INITIAL_WRITER_SIZE);
        if (writer->buffer == NULL)
            return;
        writer->capacity += count + INITIAL_WRITER_SIZE;
    }
    memcpy(writer->buffer + writer->length, buf, count);
    writer->length += count;
}

void write_string(Writer *writer, const char *text) {
    write_bytes(writer, text, strlen(text));
}

void write_constant(Writer *writer, int c) {
    int char_count = c == 0 ? 1 : floor(log10(abs(c)) + 1);
    size_t count = char_count + 1 + (c < 0 ? 1 : 0) + 1;
    char str[count];
    snprintf(str, count, "$%d", c);
    write_string(writer, str);
}

const char registers[REGISTER_COUNT][4] = {
  "eax\0", "ebx\0", "ecx\0", "edx\0", "esi\0", "edi\0"
};

const char *get_register(register_identifier_t reg) {
  return registers[reg % REGISTER_COUNT];
}

void write_register(Writer *writer, iloc_register_t reg) {
    if (reg.type != GENERAL)
      return;
    write_string(writer, "%");
    write_string(writer, get_register(reg.identifier));
}

void write_register_offset(Writer *writer, uint32_t offset, iloc_register_t reg) {
  write_string(writer, "-");
  int char_count = offset == 0 ? 1 : floor(log10(abs(offset)) + 1);
  size_t count = char_count + 1 + (offset < 0 ? 1 : 0);
  char str[count];
  snprintf(str, count, "%d", offset);
  write_string(writer, str);
  if(reg.type == FRAME_POINTER) {
    write_string(writer, "(%rbp)");
  } else {
    write_string(writer, "(%");
    write_register(writer, reg);
    write_string(writer, ")");
  }
}

void write_label(Writer *writer, label_identifier_t label) {
    write_string(writer, "l");
    int char_count = label == 0 ? 1 : floor(log10(abs(label)) + 1);
    size_t count = char_count + 1 + (label < 0 ? 1 : 0);
    char str[count];
    snprintf(str, count, "%d", label);
    write_string(writer, str);
}

void write_operand_separator(Writer *writer) {
    char *sep = ", ";
    write_string(writer, sep);
}

void write_branch_separator(Writer *writer) {
    char *sep = " -> ";
    write_string(writer, sep);
}

void write_standard_separator(Writer *writer) {
    char *sep = " => ";
    write_string(writer, sep);
}

void finish_word(Writer *writer) {
    char c = ' ';
    write_bytes(writer, &c, 1);
}

void finish_line(Writer *writer) {
    char c = '\n';
    write_bytes(writer, &c, 1);
}
