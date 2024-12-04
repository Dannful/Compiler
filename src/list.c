#include "list.h"
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
  while(head != NULL) {
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
  while(head != NULL) {
    iloc_instruction_t instruction = head->instruction;
    switch(instruction.type) {
      case SREG_SREG_DREG: {
        write_string(writer, instruction.mnemonic);
        finish_word(writer);
        write_register(writer, instruction.operands.sources[0]);
        write_operand_separator(writer);
        write_register(writer, instruction.operands.sources[1]);
        if(instruction.mnemonic[0] == 'c' && instruction.mnemonic[1] == 'm' && instruction.mnemonic[2] == 'p') {
          write_branch_separator(writer);
        } else {
          write_standard_separator(writer);
        }
        write_register(writer, instruction.destination.reg);
        finish_line(writer);
        break;
      }
      case SREG_OPERAND_DREG: {
        write_string(writer, instruction.mnemonic);
        finish_word(writer);
        write_register(writer, instruction.operands.reg_and_value.source);
        write_operand_separator(writer);
        write_constant(writer, instruction.operands.reg_and_value.value);
        write_standard_separator(writer);
        write_register(writer, instruction.destination.reg);
        finish_line(writer);
        break;
      }
      case SREG_DREG_OPERAND: {
        write_string(writer, instruction.mnemonic);
        finish_word(writer);
        write_register(writer, instruction.operands.source);
        write_standard_separator(writer);
        write_register(writer, instruction.destination.reg_and_value.source);
        write_operand_separator(writer);
        write_constant(writer, instruction.destination.reg_and_value.value);
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
        write_string(writer, instruction.mnemonic);
        finish_word(writer);
        write_constant(writer, instruction.operands.value);
        write_standard_separator(writer);
        write_register(writer, instruction.destination.reg);
        finish_line(writer);
        break;
      }
      case LABEL: {
        write_label(writer, instruction.operands.label);
        write_string(writer, ":");
        finish_line(writer);
        break;
      }
      case SREG_LABEL_LABEL: {
        write_string(writer, instruction.mnemonic);
        finish_word(writer);
        write_register(writer, instruction.operands.source);
        write_branch_separator(writer);
        write_label(writer, instruction.destination.labels[0]);
        write_operand_separator(writer);
        write_label(writer, instruction.destination.labels[1]);
        finish_line(writer);
        break;
      }
      case JUMP: {
        write_string(writer, instruction.mnemonic);
        write_branch_separator(writer);
        write_label(writer, instruction.destination.label);
        finish_line(writer);
        break;
      }
    }
    head = head->next;
  }
  char *result = strdup((char*) writer->buffer);
  destroy_writer(writer);
  return result;
}
