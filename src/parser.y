/*
Grupo C
Alunos:
Gustavo Sleman Lenz - 00290394
Vinicius Daniel Spadotto - 00341554
*/

%{
    #include <string.h>
    #include <stdio.h>
    #include "../include/asd.h"
    extern void *arvore;
    extern int get_line_number(void);
    asd_tree_t *children[5] = {0};
    int yylex(void);
    void yyerror (char const *mensagem);
%}

%code requires { #include "../include/asd.h" }

%union {
  struct lex_value {
    unsigned int line_number;
    char token_type;
    char *value;
  } lex_value;
  asd_tree_t *tree;
}

%define parse.error verbose
%token TK_PR_INT
%token TK_PR_FLOAT
%token TK_PR_IF
%token TK_PR_ELSE
%token TK_PR_WHILE
%token TK_PR_RETURN
%token TK_OC_LE
%token TK_OC_GE
%token TK_OC_EQ
%token TK_OC_NE
%token TK_OC_AND
%token TK_OC_OR
%token<lex_value> TK_IDENTIFICADOR
%token<lex_value> TK_LIT_INT
%token<lex_value> TK_LIT_FLOAT
%token TK_ERRO

%type<tree> programa
%type<tree> lista_de_funcoes
%type<tree> funcao
%type<tree> cabecalho_funcao
%type<tree> tipo
%type<tree> literal
%type<tree> lista_de_parametros
%type<tree> parametro
%type<tree> bloco_comando
%type<tree> lista_comandos_simples
%type<tree> comando
%type<tree> declaracao
%type<tree> lista_variavel
%type<tree> variavel
%type<tree> atribuicao
%type<tree> chamada_funcao
%type<tree> lista_de_argumentos
%type<tree> retorno
%type<tree> controle_fluxo
%type<tree> construcao_condicional
%type<tree> construcao_iterativa
%type<tree> expressao
%type<tree> operando_and
%type<tree> operando_eq
%type<tree> operando_bool
%type<tree> operando_sum
%type<tree> fator
%type<tree> unario

%%

programa:
    lista_de_funcoes {
      $$ = $1;
      arvore = $1;
    }
    | ;

lista_de_funcoes:
    lista_de_funcoes funcao {
      if(children[0]) {
        asd_add_child(children[0], $2);
      } else {
        asd_add_child($$, $2);
      }
      children[0] = $2;
    }
    | funcao {
      $$ = $1;
    }

funcao:
    cabecalho_funcao bloco_comando {
      asd_add_child($1, $2);
      $$ = $1;
    }

cabecalho_funcao:
    TK_IDENTIFICADOR '=' lista_de_parametros '>' tipo {
      $$ = asd_new($1.value);
    }

tipo:
    TK_PR_INT {
      $$ = asd_new("int");
    }
    | TK_PR_FLOAT {
      $$ = asd_new("float");
    }

literal:
    TK_LIT_FLOAT {
      $$ = asd_new(strdup($1.value));
    }
    | TK_LIT_INT {
      $$ = asd_new(strdup($1.value));
    }

lista_de_parametros:
    lista_de_parametros TK_OC_OR parametro {
      if(children[1]) {
        asd_add_child(children[1], $3);
      } else {
        asd_add_child($$, $3);
      }
      children[1] = $3;
    }
    | parametro {
      $$ = $1;
    }

parametro:
    TK_IDENTIFICADOR '<' '-' tipo {
      $$ = asd_new("parameter");
      asd_add_child($$, asd_new($1.value));
      asd_add_child($$, $4);
    }
    | ;

bloco_comando:
    '{' lista_comandos_simples '}' {
      $$ = $2;
    }
    | '{' '}';

lista_comandos_simples:
    lista_comandos_simples comando ';' {
      if(children[2]) {
        asd_add_child(children[2], $2);
      } else {
        asd_add_child($$, $2);
      }
      children[2] = $2;
    }
    | comando ';' {
      $$ = $1;
    }

comando:
    declaracao {
      $$ = $1;
    }
    | atribuicao {
      $$ = $1;
    }
    | chamada_funcao {
      $$ = $1;
    }
    | retorno {
      $$ = $1;
    }
    | controle_fluxo {
      $$ = $1;
    }
    | bloco_comando {
      $$ = $1;
    }

declaracao:
    tipo lista_variavel {
      $$ = asd_new("declaration");
      asd_add_child($$, $1);
      asd_add_child($$, $2);
    }

lista_variavel:
    lista_variavel ',' variavel {
      if(children[3]) {
        asd_add_child(children[3], $3);
      } else {
        asd_add_child($$, $3);
      }
      children[3] = $3;
    }
    | variavel {
      $$ = $1;
    }

variavel:
    TK_IDENTIFICADOR {
      $$ = asd_new("variable");
      asd_add_child($$, asd_new($1.value));
    }
    | TK_IDENTIFICADOR TK_OC_LE literal {
      $$ = asd_new("variable");
      asd_add_child($$, asd_new($1.value));
      asd_add_child($$, $3);
    }

atribuicao:
    TK_IDENTIFICADOR '=' expressao {
      $$ = asd_new("attribution");
      asd_add_child($$, asd_new($1.value));
      asd_add_child($$, $3);
    }

chamada_funcao:
    TK_IDENTIFICADOR '(' lista_de_argumentos ')' {
      $$ = asd_new($1.value);
      asd_add_child($$, $3);
    }

lista_de_argumentos:
    lista_de_argumentos ',' expressao {
      if(children[4]) {
        asd_add_child(children[4], $3);
      } else {
        asd_add_child($$, $3);
      }
      children[4] = $3;
    }
    | expressao {
      $$ = $1;
    }

retorno:
    TK_PR_RETURN expressao {
      $$ = asd_new("return");
      asd_add_child($$, $2);
    }

controle_fluxo:
    construcao_condicional {
      $$ = $1;
    } | construcao_iterativa {
      $$ = $1;
    }

construcao_condicional:
    TK_PR_IF '(' expressao ')' bloco_comando {
      $$ = asd_new("if");
      asd_add_child($$, $3);
      asd_add_child($$, $5);
    }
    | TK_PR_IF '(' expressao ')' bloco_comando TK_PR_ELSE bloco_comando {
      $$ = asd_new("if");
      asd_add_child($$, $3);
      asd_add_child($$, $5);
      asd_add_child($$, $7);
    }

construcao_iterativa:
    TK_PR_WHILE '(' expressao ')' bloco_comando {
      $$ = asd_new("while");
      asd_add_child($$, $3);
      asd_add_child($$, $5);
    }

expressao:
    expressao TK_OC_OR operando_and {
      $$ = asd_new("|");
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | operando_and {
      $$ = $1;
    }

operando_and:
    operando_and TK_OC_AND operando_eq {
      $$ = asd_new("&");
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | operando_eq {
      $$ = $1;
    }

operando_eq:
    operando_eq TK_OC_NE operando_bool {
      $$ = asd_new("!=");
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | operando_eq TK_OC_EQ operando_bool {
      $$ = asd_new("==");
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | operando_bool {
      $$ = $1;
    }

operando_bool:
    operando_bool TK_OC_GE operando_sum {
      $$ = asd_new(">=");
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | operando_bool TK_OC_LE operando_sum {
      $$ = asd_new("<=");
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | operando_bool '>' operando_sum {
      $$ = asd_new(">");
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | operando_bool '<' operando_sum {
      $$ = asd_new("<");
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | operando_sum {
      $$ = $1;
    }

operando_sum:
    operando_sum '-' fator {
      $$ = asd_new("-");
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | operando_sum '+' fator {
      $$ = asd_new("+");
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | fator {
      $$ = $1;
    }

fator:
    fator '%' unario {
      $$ = asd_new("%");
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | fator '/' unario {
      $$ = asd_new("/");
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | fator '*' unario {
      $$ = asd_new("*");
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | unario {
      $$ = $1;
    }

unario:
    '(' expressao ')' {
      $$ = $2;
    }
    | '!' unario {
      $$ = asd_new("!");
      asd_add_child($$, $2);
    }
    | '-' unario {
      $$ = asd_new("!");
      asd_add_child($$, $2);
    }
    | literal {
      $$ = $1;
    }
    | TK_IDENTIFICADOR {
      $$ = asd_new($1.value);
    }
    | chamada_funcao {
      $$ = $1;
    }

%%
void yyerror(char const *mensagem) {
    fprintf(stderr, "Line %i: %s\n", get_line_number(), mensagem);
}

void set_input_string(const char* in);
void end_lexical_scan(void);

int parse_string(const char* in) {
  set_input_string(in);
  int rv = yyparse();
  end_lexical_scan();
  return rv;
}
