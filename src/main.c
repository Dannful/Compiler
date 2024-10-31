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
void exporta(void *arvore) { asd_print((asd_tree_t *)arvore); }
int main(int argc, char **argv) {
  int ret = yyparse();
  exporta(arvore);
  yylex_destroy();
  return ret;
}
