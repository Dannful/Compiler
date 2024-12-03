#include "list.h"
#include <stdlib.h>
#include <string.h>

List *create_list() {
  List *list = malloc(sizeof(List));
  list->head = NULL;
  list->count = 0;
  return list;
}

void list_add(List *list, iloc_instruction_t instruction) {
  ListNode *head = list->head;
  ListNode *new_node = malloc(sizeof(ListNode));
  new_node->value = instruction;
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
  while(head != NULL)
    list_add(destination, head->value);
}

void _node_free(ListNode *node) {
  if (node == NULL)
    return;
  _node_free(node->next);
  free(node);
}

void list_destroy(List *list) {
  _node_free(list->head);
  free(list);
}
