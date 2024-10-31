/*
Grupo C
Alunos:
Gustavo Sleman Lenz - 00290394
Vinicius Daniel Spadotto - 00341554
*/

#include "../include/asd.h"
#include <stdio.h>
extern int yyparse(void);
extern int yylex_destroy(void);
void *arvore = NULL;
void print_tree(asd_tree_t *tree, unsigned int indentation_level) {
  for (int i = 0; i < indentation_level; i++)
    printf(" ");
  printf("%s\n", tree->label);
  for (int i = 0; i < tree->number_of_children; i++)
    print_tree(tree->children[i], indentation_level + 1);
}
void exporta(void *arvore) { print_tree((asd_tree_t *)arvore, 0); }
int main(int argc, char **argv) {
  int ret = yyparse();
  exporta(arvore);
  yylex_destroy();
  return ret;
}
