/*
Grupo C
Alunos:
Gustavo Sleman Lenz - 00290394
Vinicius Daniel Spadotto - 00341554
*/

/*
Função principal para realização da análise sintática.

Este arquivo será posterioremente substituído, não acrescente nada.
*/
#include "../obj/parser.tab.h" //arquivo gerado com bison -d parser.y
extern int yylex_destroy(void);

int main(int argc, char **argv) {
  int ret = yyparse();
  yylex_destroy();
  return ret;
}
