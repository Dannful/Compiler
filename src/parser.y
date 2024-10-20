%{
    #include <stdio.h>
    int yylex(void);
    void yyerror (char const *mensagem);
%}

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
%token TK_IDENTIFICADOR
%token TK_LIT_INT
%token TK_LIT_FLOAT
%token TK_ERRO

%%

programa:
    lista_de_funcoes
    | ;

lista_de_funcoes:
    lista_de_funcoes funcao
    | funcao;

funcao:
    cabecalho_funcao bloco_comando;

cabecalho_funcao:
    TK_IDENTIFICADOR '=' lista_de_parametros '>' tipo;

tipo:
    TK_PR_INT
    | TK_PR_FLOAT;

literal:
    TK_LIT_FLOAT
    | TK_LIT_INT;

lista_de_parametros:
    lista_de_parametros TK_OC_OR parametro
    | parametro;

parametro:
    TK_IDENTIFICADOR '<' '-' tipo
    | ;

bloco_comando:
    '{' lista_comandos_simples '}'
    | '{' '}';

lista_comandos_simples:
    lista_comandos_simples comando ';'
    | comando ';';

comando:
    declaracao
    | atribuicao
    | chamada_funcao
    | retorno
    | controle_fluxo;

declaracao:
    tipo lista_variavel;

lista_variavel:
    lista_variavel ',' variavel
    | variavel;

variavel:
    TK_IDENTIFICADOR
    | TK_IDENTIFICADOR TK_OC_LE literal;

atribuicao:
    TK_IDENTIFICADOR '=' expressao;

chamada_funcao:
    TK_IDENTIFICADOR '(' lista_de_argumentos ')';

lista_de_argumentos:
    lista_de_argumentos ',' expressao
    | expressao;

retorno:
    TK_PR_RETURN expressao;

controle_fluxo:
    construcao_condicional | construcao_iterativa;

construcao_condicional:
    TK_PR_IF '(' expressao ')' bloco_comando
    | TK_PR_IF '(' expressao ')' bloco_comando TK_PR_ELSE bloco_comando;

construcao_iterativa:
    TK_PR_WHILE '(' expressao ')' bloco_comando;

expressao:
    expressao TK_OC_OR operando_and
    | operando_and;

operando_and:
    operando_and TK_OC_AND operando_eq
    | operando_eq;

operando_eq:
    operando_eq TK_OC_NE operando_bool
    | operando_eq TK_OC_EQ operando_bool
    | operando_bool;

operando_bool:
    operando_bool TK_OC_GE operando_sum
    | operando_bool TK_OC_LE operando_sum
    | operando_bool '>' operando_sum
    | operando_bool '<' operando_sum
    | operando_sum;

operando_sum:
    operando_sum '-' fator
    | operando_sum '+' fator
    | fator;

fator:
    fator '%' unario
    | fator '/' unario
    | fator '*' unario
    | unario;

unario:
    '(' expressao ')'
    | '!' unario
    | '-' unario
    | literal
    | TK_IDENTIFICADOR
    | chamada_funcao;

%%
void yyerror(char const *mensagem) {
    fprintf(stderr, "%s\n", mensagem);
}

void set_input_string(const char* in);
void end_lexical_scan(void);

int parse_string(const char* in) {
  set_input_string(in);
  int rv = yyparse();
  end_lexical_scan();
  return rv;
}
