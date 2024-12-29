/*
Grupo C
Alunos:
Gustavo Sleman Lenz - 00290394
Vinicius Daniel Spadotto - 00341554
*/

#include <stdio.h>
#include <stdlib.h>

extern int yyparse(void);
extern int yylex_destroy(void);
extern void set_input_string(const char* in);

void *arvore = NULL;
char *output_file = NULL;

extern void exporta(void *arvore);
int main(int argc, char **argv) {
    if(argc == 1) {
      printf("Missing input program!\n");
      return 1;
    }
    if(argc == 2) {
      printf("Missing output file!\n");
      return 1;
    }
    FILE *f = fopen(argv[1], "r");
    output_file = argv[2];
    if(f == NULL) {
      printf("Error: file %s not found.\n", argv[1]);
      return 1;
    }
    fseek(f, 0, SEEK_END);
    size_t length = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buffer = calloc(sizeof(char), length);
    if(fread(buffer, sizeof(char), length, f) == 0) {
      printf("Failed to read from file %s.\n", argv[1]);
      return 1;
    }
    fclose(f);
    set_input_string(buffer);
    int ret = yyparse();
    yylex_destroy();
    free(buffer);
    return ret;
}
