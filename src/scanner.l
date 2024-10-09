%{
    #include "../obj/parser.tab.h" //arquivo gerado com bison -d parser.y
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

[a-z_][a-z_0-9]* return TK_IDENTIFICADOR;

[0-9]+ return TK_LIT_INT;
[0-9]*\.[0-9]+ return TK_LIT_FLOAT;

. return TK_ERRO;