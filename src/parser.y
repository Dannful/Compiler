/*
Grupo C
Alunos:
Gustavo Sleman Lenz - 00290394
Vinicius Daniel Spadotto - 00341554
*/

%{
    #include <string.h>
    #include <stdio.h>
    #include "stack.h"
    #include "table.h"
    #include "errors.h"
    extern void *arvore;
    extern int get_line_number(void);
    int yylex(void);
    void yyerror (char const *mensagem);
    Stack *tables_stack = NULL;
    Stack *get_tables_stack();

    #define VARIABLE_ALREADY_DECLARED "ERROR: Variável de nome \"%s\" já declarada na linha %d.\n"
    #define UNDECLARED_IDENTIFIER "ERROR: Identificador \"%s\" referido na linha %d não declarado.\n"
    #define VARIABLE_AS_FUNCTION "ERROR: Esperava-se um identificador de função na linha %d, mas \"%s\" refere-se a uma variável da linha %d.\n"
    #define FUNCTION_AS_VARIABLE "ERROR: Esperava-se um identificador de variável na linha %d, mas \"%s\" refere-se a uma função da linha %d.\n"
    #define INCOMPATIBLE_TYPES "WARNING: a variável \"%s\" declarada na linha %d é do tipo \"%s\", enquanto que o valor atribuído é do tipo \"%s\".\n"
%}

%code requires { #include "asd.h" }

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
    | {
      $$ = NULL;
    }

lista_de_funcoes:
    funcao lista_de_funcoes {
      asd_add_child($1, $2);
    }
    | funcao {
      $$ = $1;
    }

funcao:
    cabecalho_funcao '{' lista_comandos_simples '}' destroi_escopo {
      asd_add_child($1, $3);
      $$ = $1;
    } |
    cabecalho_funcao '{' '}' destroi_escopo {
      $$ = $1;
    }

cabecalho_funcao:
    TK_IDENTIFICADOR '=' gerar_escopo lista_de_parametros '>' tipo {
      Table *table = get_tables_stack()->tail->previous->value;
      if(table_has(table, $1.value))
        return ERR_DECLARED;
      table_set_value(table, $1.value, FUNCTION, $6->label);

      $$ = asd_new($1.value);
    }

tipo:
    TK_PR_INT {
      $$ = asd_new("int");
      $$->data_type = INT;
    }
    | TK_PR_FLOAT {
      $$ = asd_new("float");
      $$->data_type = FLOAT;
    }

literal:
    TK_LIT_FLOAT {
      $$ = asd_new(strdup($1.value));
      $$->data_type = FLOAT;
    }
    | TK_LIT_INT {
      $$ = asd_new(strdup($1.value));
      $$->data_type = INT;
    }

lista_de_parametros:
    lista_de_parametros TK_OC_OR parametro {
      $$ = NULL;
    }
    | parametro {
      $$ = NULL;
    }

parametro:
    TK_IDENTIFICADOR '<' '-' tipo {
      Table *table = stack_peek(get_tables_stack());
      table_set_value(table, $1.value, VARIABLE, $4->label);
      $$ = NULL;
    }
    | {
      $$ = NULL;
    }

bloco_comando:
    '{' gerar_escopo lista_comandos_simples destroi_escopo '}' {
      $$ = $3;
    }
    | '{' '}' {
      $$ = NULL;
    }

gerar_escopo: {
  stack_push(get_tables_stack(), table_create());
}

destroi_escopo: {
  stack_pop(get_tables_stack());
}

lista_comandos_simples:
    comando ';' lista_comandos_simples {
      if ($1 == NULL) {
        $$ = $3;
      } else {
        if(strcmp($1->label, "<=") == 0) {
          asd_tree_t *last = $1;
          while(last->number_of_children == 3)
            last = last->children[last->number_of_children - 1];
          asd_add_child(last, $3);
        } else {
          asd_add_child($1, $3);
        }
      }
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
      Table *table = stack_peek(get_tables_stack());
      asd_tree_t *root = $2;
      for(int i = 0; i < table->size; i++) {
        if(table->elements[i] != NULL) {
          if(table->elements[i]->value.data_type != UNKNOWN)
            continue;
          table_set_value(table, table->elements[i]->key, VARIABLE, $1->label);
        }
      }
      while(root != NULL && root->number_of_children > 0) {
        table_set_value(table, root->children[0]->label, VARIABLE, $1->label);
        root->data_type = $1->data_type;
        if(root->children[1]->data_type != root->data_type) {
          printf(INCOMPATIBLE_TYPES, root->children[0]->label, get_line_number(), get_string_for_data_type(root->data_type), get_string_for_data_type(root->children[1]->data_type));
        }
        root = root->children[root->number_of_children - 1];
      }
      $$ = $2;
    }

lista_variavel:
    variavel ',' lista_variavel {
      if ($1 == NULL) {
        $$ = $3;
      } else {
        asd_add_child($1, $3);
        $$ = $1;
      }
    }
    | variavel {
      $$ = $1;
    }

variavel:
    TK_IDENTIFICADOR {
      Table *table = stack_peek(get_tables_stack());
      TableEntry *foundValue = table_get(table, $1.value);
      if(foundValue != NULL) {
        printf(VARIABLE_ALREADY_DECLARED, $1.value, foundValue->line);
        return ERR_DECLARED;
      }
      table_set_value(table, $1.value, VARIABLE, "");
      $$ = NULL;
    }
    | TK_IDENTIFICADOR TK_OC_LE literal {
      Table *table = stack_peek(get_tables_stack());
      TableEntry *foundValue = table_get(table, $1.value);
      if(foundValue != NULL) {
        printf(VARIABLE_ALREADY_DECLARED, $1.value, foundValue->line);
        return ERR_DECLARED;
      }
      table_set_value(table, $1.value, VARIABLE, "");
      $$ = asd_new("<=");
      asd_add_child($$, asd_new($1.value));
      asd_add_child($$, $3);
    }

atribuicao:
    TK_IDENTIFICADOR '=' expressao {
      TableEntry *entry = table_search(get_tables_stack(), $1.value);
      if(entry == NULL) {
        printf(UNDECLARED_IDENTIFIER, $1.value, get_line_number());
        return ERR_UNDECLARED;
      }
      if(entry->entry_type == FUNCTION) {
        printf(FUNCTION_AS_VARIABLE, get_line_number(), $1.value, entry->line);
        return ERR_FUNCTION;
      }
      $$ = asd_new("=");
      $$->data_type = entry->data_type;
      if($$->data_type != $3->data_type) {
        printf(INCOMPATIBLE_TYPES, $1.value, get_line_number(), get_string_for_data_type($$->data_type), get_string_for_data_type($3->data_type));
      }

      asd_add_child($$, asd_new($1.value));
      asd_add_child($$, $3);
    }

chamada_funcao:
    TK_IDENTIFICADOR '(' lista_de_argumentos ')' {
      TableEntry *entry = table_search(get_tables_stack(), $1.value);
      if(entry == NULL) {
        printf(UNDECLARED_IDENTIFIER, $1.value, get_line_number());
        return ERR_UNDECLARED;
      }
      if(entry->entry_type == VARIABLE) {
        printf(VARIABLE_AS_FUNCTION, get_line_number(), $1.value, entry->line);
        return ERR_VARIABLE;
      }
      char node_label[strlen($1.value) + 5];
      sprintf(node_label, "call %s", $1.value);
      $$ = asd_new(node_label);
      asd_add_child($$, $3);
    }

lista_de_argumentos:
    expressao ',' lista_de_argumentos {
      asd_add_child($1, $3);
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
      if($1->data_type == FLOAT || $3->data_type == FLOAT) {
        $$->data_type = FLOAT;
      } else {
        $$->data_type = INT;
      }
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | operando_and {
      $$ = $1;
    }

operando_and:
    operando_and TK_OC_AND operando_eq {
      $$ = asd_new("&");
      if($1->data_type == FLOAT || $3->data_type == FLOAT) {
        $$->data_type = FLOAT;
      } else {
        $$->data_type = INT;
      }
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | operando_eq {
      $$ = $1;
    }

operando_eq:
    operando_eq TK_OC_NE operando_bool {
      $$ = asd_new("!=");
      if($1->data_type == FLOAT || $3->data_type == FLOAT) {
        $$->data_type = FLOAT;
      } else {
        $$->data_type = INT;
      }
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | operando_eq TK_OC_EQ operando_bool {
      $$ = asd_new("==");
      if($1->data_type == FLOAT || $3->data_type == FLOAT) {
        $$->data_type = FLOAT;
      } else {
        $$->data_type = INT;
      }
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | operando_bool {
      $$ = $1;
    }

operando_bool:
    operando_bool TK_OC_GE operando_sum {
      $$ = asd_new(">=");
      if($1->data_type == FLOAT || $3->data_type == FLOAT) {
        $$->data_type = FLOAT;
      } else {
        $$->data_type = INT;
      }
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | operando_bool TK_OC_LE operando_sum {
      $$ = asd_new("<=");
      if($1->data_type == FLOAT || $3->data_type == FLOAT) {
        $$->data_type = FLOAT;
      } else {
        $$->data_type = INT;
      }
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | operando_bool '>' operando_sum {
      $$ = asd_new(">");
      if($1->data_type == FLOAT || $3->data_type == FLOAT) {
        $$->data_type = FLOAT;
      } else {
        $$->data_type = INT;
      }
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | operando_bool '<' operando_sum {
      $$ = asd_new("<");
      if($1->data_type == FLOAT || $3->data_type == FLOAT) {
        $$->data_type = FLOAT;
      } else {
        $$->data_type = INT;
      }
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | operando_sum {
      $$ = $1;
    }

operando_sum:
    operando_sum '-' fator {
      $$ = asd_new("-");
      if($1->data_type == FLOAT || $3->data_type == FLOAT) {
        $$->data_type = FLOAT;
      } else {
        $$->data_type = INT;
      }
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | operando_sum '+' fator {
      $$ = asd_new("+");
      if($1->data_type == FLOAT || $3->data_type == FLOAT) {
        $$->data_type = FLOAT;
      } else {
        $$->data_type = INT;
      }
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | fator {
      $$ = $1;
    }

fator:
    fator '%' unario {
      $$ = asd_new("%");
      if($1->data_type == FLOAT || $3->data_type == FLOAT) {
        $$->data_type = FLOAT;
      } else {
        $$->data_type = INT;
      }
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | fator '/' unario {
      $$ = asd_new("/");
      if($1->data_type == FLOAT || $3->data_type == FLOAT) {
        $$->data_type = FLOAT;
      } else {
        $$->data_type = INT;
      }
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | fator '*' unario {
      $$ = asd_new("*");
      if($1->data_type == FLOAT || $3->data_type == FLOAT) {
        $$->data_type = FLOAT;
      } else {
        $$->data_type = INT;
      }
      asd_add_child($$, $1);
      asd_add_child($$, $3);
    }
    | unario {
      $$ = $1;
    }

unario:
    '(' expressao ')' {
      $$ = $2;
      $$->data_type = $2->data_type;
    }
    | '!' unario {
      $$ = asd_new("!");
      $$->data_type = $2->data_type;
      asd_add_child($$, $2);
    }
    | '-' unario {
      $$ = asd_new("-");
      $$->data_type = $2->data_type;
      asd_add_child($$, $2);
    }
    | literal {
      $$ = $1;
    }
    | TK_IDENTIFICADOR {
      TableEntry *entry = table_search(get_tables_stack(), $1.value);
      if(entry == NULL) {
        printf(UNDECLARED_IDENTIFIER, $1.value, get_line_number());
        return ERR_UNDECLARED;
      }
      if(entry->entry_type == FUNCTION) {
        printf(FUNCTION_AS_VARIABLE, get_line_number(), $1.value, entry->line);
        return ERR_FUNCTION;
      }
      $$ = asd_new($1.value);
      $$->data_type = entry->data_type;
    }
    | chamada_funcao {
      $$ = $1;
    }

%%
void yyerror(char const *mensagem) {
    fprintf(stderr, "Line %i: %s\n", get_line_number(), mensagem);
}

Stack *get_tables_stack() {
  if(tables_stack == NULL) {
    tables_stack = create_stack();
    stack_push(tables_stack, table_create());
  }
  return tables_stack;
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
