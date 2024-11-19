/*
Grupo C
Alunos:
Gustavo Sleman Lenz - 00290394
Vinicius Daniel Spadotto - 00341554

Arquivo obtido e alterado a partir dos arquivos disponibilizados pelo professor
no Moodle.
*/
#include "../include/asd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ARQUIVO_SAIDA "saida.dot"

const char *get_string_for_data_type(DataType data_type) {
  switch (data_type) {
  case INT:
    return "int";
  case FLOAT:
    return "float";
  case UNKNOWN:
    return "unknown";
  }
  return "unknown";
}

asd_tree_t *asd_new(const char *label) {
  asd_tree_t *ret = NULL;
  ret = calloc(1, sizeof(asd_tree_t));
  if (ret != NULL) {
    ret->label = strdup(label);
    ret->number_of_children = 0;
    ret->children = NULL;
    ret->data_type = UNKNOWN;
  }
  return ret;
}

void asd_free(asd_tree_t *tree) {
  if (tree == NULL)
    return;
  int i;
  for (i = 0; i < tree->number_of_children; i++) {
    asd_free(tree->children[i]);
  }
  free(tree->children);
  free(tree->label);
  free(tree);
}

void asd_add_child(asd_tree_t *tree, asd_tree_t *child) {
  if (tree == NULL || child == NULL)
    return;
  tree->number_of_children++;
  tree->children =
      realloc(tree->children, tree->number_of_children * sizeof(asd_tree_t *));
  tree->children[tree->number_of_children - 1] = child;
}

static void _asd_print(FILE *foutput, asd_tree_t *tree) {
  int i;
  if (tree != NULL) {
    fprintf(foutput, "%p [label=\"%s\"];\n", tree, tree->label);
    for (int child = 0; child < tree->number_of_children; child++)
      fprintf(foutput, "%p, %p\n", tree, tree->children[child]);
    for (int child = 0; child < tree->number_of_children; child++)
      _asd_print(foutput, tree->children[child]);
  } else {
    printf("Erro: %s recebeu parâmetro tree = %p.\n", __FUNCTION__, tree);
  }
}

void asd_print(asd_tree_t *tree) {
  if (tree == NULL)
    return;
  FILE *foutput = stdout;
  _asd_print(foutput, tree);
}

static void _asd_print_graphviz(FILE *foutput, asd_tree_t *tree) {
  if (tree == NULL)
    return;
  int i;
  fprintf(foutput, "  %ld [ label=\"%s\" ];\n", (long)tree, tree->label);
  for (i = 0; i < tree->number_of_children; i++) {
    fprintf(foutput, "  %ld -> %ld;\n", (long)tree, (long)tree->children[i]);
    _asd_print_graphviz(foutput, tree->children[i]);
  }
}

void asd_print_graphviz(asd_tree_t *tree) {
  if (tree == NULL)
    return;
  FILE *foutput = fopen(ARQUIVO_SAIDA, "w+");
  if (foutput == NULL) {
    printf("Erro: %s não pude abrir o arquivo [%s] para escrita.\n",
           __FUNCTION__, ARQUIVO_SAIDA);
  }
  if (tree != NULL) {
    fprintf(foutput, "digraph grafo {\n");
    _asd_print_graphviz(foutput, tree);
    fprintf(foutput, "}\n");
    fclose(foutput);
  } else {
    printf("Erro: %s recebeu parâmetro tree = %p.\n", __FUNCTION__, tree);
  }
}
