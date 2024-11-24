/*
Grupo C
Alunos:
Gustavo Sleman Lenz - 00290394
Vinicius Daniel Spadotto - 00341554
*/
#include "stack.h"
#include <stdlib.h>
#include <string.h>

Stack *create_stack() {
  Stack *stack = malloc(sizeof(Stack));
  stack->head = NULL;
  stack->tail = NULL;
  stack->count = 0;
  return stack;
}

void stack_push(Stack *stack, Table *value) {
  StackNode *tail = stack->tail;
  StackNode *new_node = malloc(sizeof(StackNode));
  new_node->value = value;
  new_node->next = NULL;
  if (tail == NULL) {
    new_node->previous = NULL;
    stack->tail = stack->head = new_node;
    stack->count = 1;
    return;
  }
  tail->next = new_node;
  new_node->previous = tail;
  stack->tail = new_node;
  stack->count++;
}

Table *stack_pop(Stack *stack) {
  if (stack->count == 0)
    return NULL;
  if (stack->count == 1) {
    Table *result = stack->head->value;
    stack->head = stack->tail = NULL;
    stack->count = 0;
    free(stack->head);
    return result;
  }
  StackNode *node = stack->head;
  while (node->next != stack->tail)
    node = node->next;
  Table *result = node->next->value;
  free(node->next);
  node->next = NULL;
  stack->tail = node;
  stack->count--;
  return result;
}

TableEntry *table_search(Stack *stack, char *key) {
  StackNode *node = stack->tail;
  while (node != NULL) {
    TableEntry *entry = table_get(node->value, key);
    if (entry != NULL)
      return entry;
    node = node->previous;
  }
  return NULL;
}

Table *stack_peek(Stack *stack) {
  if (stack->count == 0)
    return NULL;
  return stack->tail->value;
}

void _node_free(StackNode *node) {
  if (node == NULL)
    return;
  _node_free(node->next);
  free(node);
}

void stack_destroy(Stack *stack) {
  _node_free(stack->head);
  free(stack);
}
