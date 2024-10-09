
%define parse.error verbose
%{
    #include <stdio.h>
    int yylex(void);
    void yyerror (char const *mensagem);
%}

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
    cabecalho_funcao bloco_comando

cabecalho_funcao:
    TK_IDENTIFICADOR '=' lista_de_parametros '>' tipo

tipo:
    TK_PR_INT
    | TK_PR_FLOAT

literal:
    TK_LIT_FLOAT
    | TK_LIT_INT

lista_de_parametros:
    lista_de_parametros TK_OC_OR parametro
    | parametro

parametro:
    TK_IDENTIFICADOR '<' '-' tipo
    | ;

bloco_comando:
    '{' lista_comandos_simples '}'

lista_comandos_simples:
    lista_comandos_simples comando ';'
    | comando ';'

comando:
    declaracao
    | atribuicao
    | chamada
    | retorno
    | controle_fluxo

declaracao:
    tipo lista_variavel

lista_variavel:
    lista_variavel ',' variavel
    | variavel

variavel:
    TK_IDENTIFICADOR
    | TK_IDENTIFICADOR TK_OC_LE literal

atribuicao:
    TK_IDENTIFICADOR '=' expressao

funcao:
    TK_IDENTIFICADOR '(' lista_de_argumentos ')'

lista_de_argumentos:

chamada:

retorno:

controle_fluxo:

expressao:

%%
void yyerror(char const *mensagem) {
    fprintf(stderr, "%s\n", mensagem);
}