#pragma once

#include <stdint.h>
#include <stdio.h>
#include "iloc.h"

typedef struct list_node {
  iloc_instruction_t instruction;
  struct list_node *next;
} ListNode;

typedef struct {
  ListNode *head;
  size_t count;
} List;

List *create_list();
void list_add(List *list, iloc_instruction_t instruction);
void list_append(List *destination, List *source);
void list_destroy(List *list);
