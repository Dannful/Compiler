/*
Grupo C
Alunos:
Gustavo Sleman Lenz - 00290394
Vinicius Daniel Spadotto - 00341554
*/

%{
    #include <stdio.h>
    extern void *arvore;
    extern int get_line_number(void);
    int yylex(void);
    void yyerror(char const *mensagem);
%}

%code requires { #include "generations.h" }

%union {
  lex_value_t* lex_value;
  asd_tree_t* tree;
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
      parse_program(&$$, $1);
    }
    | {
      parse_null(&$$);
    }

lista_de_funcoes:
    funcao lista_de_funcoes {
      parse_list($1, $2);
    }
    | funcao {
      parse_simple(&$$, $1);
    }

funcao:
    cabecalho_funcao '{' lista_comandos_simples '}' destroi_escopo {
      parse_function(&$$, $1, $3);
    } |
    cabecalho_funcao '{' '}' destroi_escopo {
      parse_simple(&$$, $1);
    }

cabecalho_funcao:
    TK_IDENTIFICADOR '=' gerar_escopo lista_de_parametros '>' tipo {
      parse_function_header(&$$, $1, $6);
    }

tipo:
    TK_PR_INT {
      parse_type(&$$, "int");
    }
    | TK_PR_FLOAT {
      parse_type(&$$, "float");
    }

literal:
    TK_LIT_FLOAT {
      parse_literal(&$$, $1, FLOAT);
    }
    | TK_LIT_INT {
      parse_literal(&$$, $1, INT);
    }

lista_de_parametros:
    lista_de_parametros TK_OC_OR parametro {
      parse_null(&$$);
    }
    | parametro {
      parse_null(&$$);
    }

parametro:
    TK_IDENTIFICADOR '<' '-' tipo {
      parse_parameter(&$$, $1, $4);
    }
    | {
      parse_null(&$$);
    }

bloco_comando:
    '{' gerar_escopo lista_comandos_simples destroi_escopo '}' {
      parse_simple(&$$, $3);
    }
    | '{' '}' {
      parse_null(&$$);
    }

gerar_escopo: {
  parse_generate_scope();
}

destroi_escopo: {
  parse_destroy_scope();
}

lista_comandos_simples:
    comando ';' lista_comandos_simples {
      parse_simple_command_list(&$$, $1, $3);
    }
    | comando ';' {
      parse_simple(&$$, $1);
    }

comando:
    declaracao {
      parse_simple(&$$, $1);
    }
    | atribuicao {
      parse_simple(&$$, $1);
    }
    | chamada_funcao {
      parse_simple(&$$, $1);
    }
    | retorno {
      parse_simple(&$$, $1);
    }
    | controle_fluxo {
      parse_simple(&$$, $1);
    }
    | bloco_comando {
      parse_simple(&$$, $1);
    }

declaracao:
    tipo lista_variavel {
      parse_declaration(&$$, $1, $2);
    }

lista_variavel:
    variavel ',' lista_variavel {
      parse_variable_list(&$$, $1, $3);
    }
    | variavel {
      parse_simple(&$$, $1);
    }

variavel:
    TK_IDENTIFICADOR {
      parse_variable(&$$, $1);
    }
    | TK_IDENTIFICADOR TK_OC_LE literal {
      parse_variable_literal(&$$, $1, $3);
    }

atribuicao:
    TK_IDENTIFICADOR '=' expressao {
      parse_assignment(&$$, $1, $3);
    }

chamada_funcao:
    TK_IDENTIFICADOR '(' lista_de_argumentos ')' {
      parse_function_call(&$$, $1, $3);
    }

lista_de_argumentos:
    expressao ',' lista_de_argumentos {
      parse_list($1, $3);
    }
    | expressao {
      parse_simple(&$$, $1);
    }

retorno:
    TK_PR_RETURN expressao {
      parse_expression_return(&$$, $2);
    }

controle_fluxo:
    construcao_condicional {
      parse_simple(&$$, $1);
    } | construcao_iterativa {
      parse_simple(&$$, $1);
    }

construcao_condicional:
    TK_PR_IF '(' expressao ')' bloco_comando {
      parse_expression_condition(&$$, $3, $5, NULL);
    }
    | TK_PR_IF '(' expressao ')' bloco_comando TK_PR_ELSE bloco_comando {
      parse_expression_condition(&$$, $3, $5, $7);
    }

construcao_iterativa:
    TK_PR_WHILE '(' expressao ')' bloco_comando {
      parse_expression_while(&$$, $3, $5);
    }

expressao:
    expressao TK_OC_OR operando_and {
      parse_binary_operator(&$$, "|", $1, $3);
    }
    | operando_and {
      parse_simple(&$$, $1);
    }

operando_and:
    operando_and TK_OC_AND operando_eq {
      parse_binary_operator(&$$, "&", $1, $3);
    }
    | operando_eq {
      parse_simple(&$$, $1);
    }

operando_eq:
    operando_eq TK_OC_NE operando_bool {
      parse_binary_operator(&$$, "!=", $1, $3);
    }
    | operando_eq TK_OC_EQ operando_bool {
      parse_binary_operator(&$$, "==", $1, $3);
    }
    | operando_bool {
      parse_simple(&$$, $1);
    }

operando_bool:
    operando_bool TK_OC_GE operando_sum {
      parse_binary_operator(&$$, ">=", $1, $3);
    }
    | operando_bool TK_OC_LE operando_sum {
      parse_binary_operator(&$$, "<=", $1, $3);
    }
    | operando_bool '>' operando_sum {
      parse_binary_operator(&$$, ">", $1, $3);
    }
    | operando_bool '<' operando_sum {
      parse_binary_operator(&$$, "<", $1, $3);
    }
    | operando_sum {
      parse_simple(&$$, $1);
    }

operando_sum:
    operando_sum '-' fator {
      parse_binary_operator(&$$, "-", $1, $3);
    }
    | operando_sum '+' fator {
      parse_binary_operator(&$$, "+", $1, $3);
    }
    | fator {
      parse_simple(&$$, $1);
    }

fator:
    fator '%' unario {
      parse_binary_operator(&$$, "%", $1, $3);
    }
    | fator '/' unario {
      parse_binary_operator(&$$, "/", $1, $3);
    }
    | fator '*' unario {
      parse_binary_operator(&$$, "*", $1, $3);
    }
    | unario {
      parse_simple(&$$, $1);
    }

unario:
    '(' expressao ')' {
      parse_unary_parenthesis(&$$, $2);
    }
    | '!' unario {
      parse_unary_operator(&$$, "!", $2);
    }
    | '-' unario {
      parse_unary_operator(&$$, "-", $2);
    }
    | literal {
      parse_simple(&$$, $1);
    }
    | TK_IDENTIFICADOR {
      parse_unary_identifier(&$$, $1);
    }
    | chamada_funcao {
      parse_simple(&$$, $1);
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

void exporta(void *arvore) { asd_print((asd_tree_t *)arvore); }
