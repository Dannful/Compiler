/*
Grupo C
Alunos:
Gustavo Sleman Lenz - 00290394
Vinicius Daniel Spadotto - 00341554
*/

%{
    #include "parser.tab.h"  // arquivo gerado com bison -d parser.y
%}
%option yylineno

%%

\/\/.*$ {}
[[:space:]] {}

int return TK_PR_INT;
float return TK_PR_FLOAT;
if return TK_PR_IF;
else return TK_PR_ELSE;
while return TK_PR_WHILE;
return return TK_PR_RETURN;

-|!|\*|\/|%|\+|<|>|\{|\}|\(|\)|=|,|; { return (int) yytext[0]; }

\<= return TK_OC_LE;
\>= return TK_OC_GE;
== return TK_OC_EQ;
!= return TK_OC_NE;
& return TK_OC_AND;
\| return TK_OC_OR;

[a-z_][a-z_0-9]* {
  yylval.lex_value = (lex_value_t*)malloc(sizeof(lex_value_t));
  yylval.lex_value->line_number = yylineno;
  yylval.lex_value->token_type = 'I';
  yylval.lex_value->value = strdup(yytext);
  return TK_IDENTIFICADOR;
}

[0-9]+ {
  yylval.lex_value = (lex_value_t*)malloc(sizeof(lex_value_t));
  yylval.lex_value->line_number = yylineno;
  yylval.lex_value->token_type = 'L';
  yylval.lex_value->value = strdup(yytext);
  return TK_LIT_INT;
}
[0-9]*\.[0-9]+ {
  yylval.lex_value = (lex_value_t*)malloc(sizeof(lex_value_t));
  yylval.lex_value->line_number = yylineno;
  yylval.lex_value->token_type = 'L';
  yylval.lex_value->value = strdup(yytext);
  return TK_LIT_FLOAT;
}

. return TK_ERRO;

%%

int get_line_number(void) { return yylineno; }

void set_input_string(const char* in) {
    yy_scan_string(in);
}

void end_lexical_scan(void) {
    yy_delete_buffer(YY_CURRENT_BUFFER);
}
