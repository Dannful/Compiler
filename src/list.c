#include "list.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "writer.h"

#define PROGRAM_BUFFER_SIZE 1024

List *create_list() {
    List *list = malloc(sizeof(List));
    list->head = NULL;
    list->count = 0;
    return list;
}

void list_add(List *list, iloc_instruction_t instruction) {
    ListNode *head = list->head;
    ListNode *new_node = malloc(sizeof(ListNode));
    new_node->instruction = instruction;
    new_node->next = NULL;
    if (head == NULL) {
        list->head = new_node;
        list->count++;
        return;
    }
    while (head->next != NULL)
        head = head->next;
    head->next = new_node;
    list->count++;
}

void list_append(List *destination, List *source) {
    ListNode *head = source->head;
    while (head != NULL) {
        list_add(destination, head->instruction);
        head = head->next;
    }
}

void _list_node_free(ListNode *node) {
    if (node == NULL)
        return;
    _list_node_free(node->next);
    free(node);
}

void list_destroy(List *list) {
    _list_node_free(list->head);
    free(list);
}

char *generate_program(List *list) {
    ListNode *head = list->head;
    Writer *writer = create_writer();
    write_string(writer, ".text");
    finish_line(writer);
    write_string(writer, ".globl main");
    finish_line(writer);
    write_string(writer, ".type main, @function");
    finish_line(writer);
    write_string(writer, "main:");
    finish_line(writer);
    write_string(writer, "pushq %rbp");
    finish_line(writer);
    write_string(writer, "movq %rsp, %rbp");
    finish_line(writer);
    while (head != NULL) {
        iloc_instruction_t instruction = head->instruction;
        switch (instruction.type) {
            case SREG_SREG_DREG: {
                if (instruction.mnemonic[0] == 'c' && instruction.mnemonic[1] == 'm' &&
                  instruction.mnemonic[2] == 'p') {
                  write_string(writer, "cmp ");
                  write_register(writer, instruction.operands.sources[0]);
                  write_operand_separator(writer);
                  write_register(writer, instruction.operands.sources[1]);
                  finish_line(writer);
                  if(strcmp(instruction.mnemonic, "cmp_NE") == 0) {
                    write_string(writer, "setne %al");
                  } else if(strcmp(instruction.mnemonic, "cmp_EQ") == 0) {
                    write_string(writer, "sete %al");
                  } else if(strcmp(instruction.mnemonic, "cmp_GE") == 0) {
                    write_string(writer, "setge %al");
                  } else if(strcmp(instruction.mnemonic, "cmp_LE") == 0) {
                    write_string(writer, "setle %al");
                  } else if(strcmp(instruction.mnemonic, "cmp_GT") == 0) {
                    write_string(writer, "setg %al");
                  } else if(strcmp(instruction.mnemonic, "cmp_LT") == 0) {
                    write_string(writer, "setl %al");
                  }
                  finish_line(writer);
                  write_string(writer, "movzbl %al, ");
                  write_register(writer, instruction.destination.reg);
                  finish_line(writer);
                  break;
                } else {
                  if(strcmp(instruction.mnemonic, "div") != 0) {
                    write_string(writer, "movl ");
                    write_register(writer, instruction.operands.sources[1]);
                    write_operand_separator(writer);
                    write_register(writer, instruction.destination.reg);
                    finish_line(writer);
                  }
                  if(strcmp(instruction.mnemonic, "mult") == 0) {
                    write_string(writer, "imull");
                  } else if(strcmp(instruction.mnemonic, "div") == 0) {
                    write_string(writer, "idiv ");
                    write_register(writer, instruction.operands.sources[0]);
                    write_operand_separator(writer);
                    write_register(writer, instruction.operands.sources[1]);
                    finish_line(writer);
                    write_string(writer, "movl %eax, ");
                    write_register(writer, instruction.destination.reg);
                    finish_line(writer);
                    break;
                  } else {
                    write_string(writer, instruction.mnemonic);
                  }
                }
                finish_word(writer);
                write_register(writer, instruction.operands.sources[0]);
                write_operand_separator(writer);
                write_register(writer, instruction.destination.reg);
                finish_line(writer);
                break;
            }
            case SREG_OPERAND_DREG: {
                if(strcmp(instruction.mnemonic, "loadAI") == 0) {
                  write_string(writer, "movl ");
                  write_register_offset(writer, instruction.operands.reg_and_value.value, instruction.operands.reg_and_value.source);
                  write_operand_separator(writer);
                  write_register(writer, instruction.destination.reg);
                } else if(strcmp(instruction.mnemonic, "multI") == 0) {
                  write_string(writer, "imull ");
                  write_constant(writer, instruction.operands.reg_and_value.value);
                  write_operand_separator(writer);
                  write_register(writer, instruction.operands.reg_and_value.source);
                  finish_line(writer);
                  write_string(writer, "movl ");
                  write_register(writer, instruction.operands.reg_and_value.source);
                  write_operand_separator(writer);
                  write_register(writer, instruction.destination.reg);
                }
                finish_line(writer);
                break;
            }
            case SREG_DREG_OPERAND: {
                write_string(writer, "movl ");
                write_register(writer, instruction.operands.source);
                write_operand_separator(writer);
                write_register_offset(writer, instruction.destination.reg_and_value.value, instruction.destination.reg_and_value.source);
                finish_line(writer);
                break;
            }
            case SREG_DREG: {
                write_string(writer, instruction.mnemonic);
                finish_word(writer);
                write_register(writer, instruction.operands.source);
                write_standard_separator(writer);
                write_register(writer, instruction.destination.reg);
                finish_line(writer);
                break;
            }
            case OPERAND_DREG: {
                if(strcmp(instruction.mnemonic, "loadI") == 0) {
                  write_string(writer, "movl ");
                  write_constant(writer, instruction.operands.value);
                  write_operand_separator(writer);
                  write_register(writer, instruction.destination.reg);
                  finish_line(writer);
                }
                break;
            }
            case LABEL: {
                write_label(writer, instruction.operands.label);
                write_string(writer, ":");
                finish_line(writer);
                break;
            }
            case SREG_LABEL_LABEL: {
                if(strcmp(instruction.mnemonic, "cbr") == 0) {
                  write_string(writer, "cmp $0, ");
                  write_register(writer, instruction.operands.source);
                  finish_line(writer);
                  write_string(writer, "je ");
                  write_label(writer, instruction.destination.labels[1]);
                  finish_line(writer);
                  write_string(writer, "jmp ");
                  write_label(writer, instruction.destination.labels[0]);
                }
                finish_line(writer);
                break;
            }
            case JUMP: {
                write_string(writer, "jmp ");
                write_label(writer, instruction.destination.label);
                finish_line(writer);
                break;
            }
            case RETURN: {
              write_string(writer, "movl ");
              write_register(writer, instruction.destination.reg);
              write_string(writer, ", %eax");
              finish_line(writer);
              write_string(writer, "popq %rbp");
              finish_line(writer);
              write_string(writer, "ret");
              finish_line(writer);
              write_bytes(writer, "\0", 1);
              break;
            }
        }
        head = head->next;
    }
    char *result = strdup((char *)writer->buffer);
    destroy_writer(writer);
    return result;
}
