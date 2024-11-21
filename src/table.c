/*
Grupo C
Alunos:
Gustavo Sleman Lenz - 00290394
Vinicius Daniel Spadotto - 00341554
*/
#include "table.h"
#include <stdlib.h>
#include <string.h>

#define SIZE 1024

size_t hash(const char *key, size_t size) {
  unsigned long hash = 5381;
  int c;

  while ((c = *key++))
    hash = ((hash << 5) + hash) + c;

  return hash % size;
}

Table *table_create(void) {
  Table *map = malloc(sizeof(Table));
  if (map == NULL)
    return NULL;
  map->count = 0;
  map->elements = calloc(sizeof(Bucket *), SIZE);
  map->size = SIZE;
  if (map->elements == NULL)
    return NULL;
  sem_init(&map->semaphore, 0, 1);
  return map;
}

void table_destroy(Table *map) {
  for (int i = 0; i < SIZE; i++) {
    Bucket *bucket = map->elements[i];
    while (bucket) {
      Bucket *aux = bucket->next;
      free((void *)bucket->key);
      free(bucket);
      bucket = aux;
    }
  }
  sem_destroy(&map->semaphore);
  free(map->elements);
  free(map);
}

TableEntry *table_get(Table *map, const char *key) {
  sem_wait(&map->semaphore);
  Bucket *bucket = map->elements[hash(key, SIZE)];
  while (bucket != NULL) {
    if (strcmp(bucket->key, key) == 0) {
      sem_post(&map->semaphore);
      return &bucket->value;
    }
    bucket = bucket->next;
  }
  sem_post(&map->semaphore);
  return NULL;
}

uint8_t table_has(Table *map, const char *key) {
  sem_wait(&map->semaphore);
  Bucket *bucket = map->elements[hash(key, SIZE)];
  while (bucket != NULL) {
    if (strcmp(bucket->key, key) == 0) {
      sem_post(&map->semaphore);
      return 1;
    }
    bucket = bucket->next;
  }
  sem_post(&map->semaphore);
  return 0;
}

void table_set(Table *map, const char *key, TableEntry value) {
  size_t index = hash(key, SIZE);
  Bucket *bucket = map->elements[index];
  Bucket *target = NULL;
  sem_wait(&map->semaphore);
  if (bucket == NULL) {
    map->elements[index] = malloc(sizeof(Bucket));
    target = map->elements[index];
  }
  while (target == NULL) {
    if (strcmp(bucket->key, key) == 0) {
      bucket->value = value;
      sem_post(&map->semaphore);
      return;
    }
    if (bucket->next == NULL) {
      target = malloc(sizeof(Bucket));
      bucket->next = target;
    } else {
      bucket = bucket->next;
    }
  }
  target->key = strdup(key);
  target->value = value;
  target->next = NULL;
  map->count++;
  sem_post(&map->semaphore);
}

void table_set_value(Table *table, const char *key, EntryType entry_type,
                     char *type) {
  extern int yylineno;
  TableEntry entry;
  entry.line = yylineno;
  if (strcmp(type, "int") == 0) {
    entry.data_type = INT;
  } else if (strcmp(type, "float") == 0) {
    entry.data_type = FLOAT;
  } else {
    entry.data_type = UNKNOWN;
  }
  entry.entry_type = entry_type;
  table_set(table, key, entry);
}

void table_remove(Table *map, const char *key) {
  size_t index = hash(key, SIZE);
  sem_wait(&map->semaphore);
  Bucket *bucket = map->elements[index];
  if (bucket == NULL)
    return;
  if (strcmp(bucket->key, key) == 0) {
    map->elements[index] = bucket->next;
    free(bucket);
    map->count--;
    sem_post(&map->semaphore);
    return;
  }
  while (strcmp(bucket->next->key, key) != 0)
    bucket = bucket->next;
  Bucket *aux = bucket->next;
  bucket->next = aux->next;
  free((void *)aux->key);
  free(aux);
  map->count--;
  sem_post(&map->semaphore);
}
