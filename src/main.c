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
extern void exporta(void *arvore);
int main(int argc, char **argv) {
  int ret = yyparse();
  exporta(arvore);
  asd_print_graphviz(arvore);
  yylex_destroy();
  return ret;
}
