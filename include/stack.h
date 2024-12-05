/*
Grupo C
Alunos:
Gustavo Sleman Lenz - 00290394
Vinicius Daniel Spadotto - 00341554
*/
#pragma once
#include <stdint.h>
#include <stdio.h>

#include "table.h"

typedef struct node {
    Table *value;
    struct node *next;
    struct node *previous;
} StackNode;

typedef struct {
    StackNode *tail;
    StackNode *head;
    size_t count;
} Stack;

Stack *create_stack();
void stack_push(Stack *stack, Table *value);
Table *stack_pop(Stack *stack);
Table *stack_peek(Stack *stack);
void stack_destroy(Stack *list);
TableEntry *table_search(Stack *stack, char *key);
