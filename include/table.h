/*
Grupo C
Alunos:
Gustavo Sleman Lenz - 00290394
Vinicius Daniel Spadotto - 00341554
*/
#pragma once
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>

#include "asd.h"

typedef enum {
    VARIABLE = 0,
    FUNCTION = 1
} EntryType;

typedef struct {
    unsigned int line;
    EntryType entry_type;
    DataType data_type;
    uint32_t offset;
} TableEntry;

typedef struct bucket {
    const char *key;
    TableEntry value;
    struct bucket *next;
} Bucket;

typedef struct {
    Bucket **elements;
    size_t count;
    size_t size;
    sem_t semaphore;
    uint32_t offset;
} Table;

Table *table_create(void);
void table_destroy(Table *table);
TableEntry *table_get(Table *table, const char *key);
uint8_t table_has(Table *table, const char *key);
void table_set(Table *table, const char *key, TableEntry value);
void table_set_value(Table *table, const char *key, EntryType entry_type, char *type);
void table_remove(Table *table, const char *key);
