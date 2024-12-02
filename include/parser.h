#pragma once

#define VARIABLE_ALREADY_DECLARED "ERROR - Linha %d: Variável de nome \"%s\" já declarada na linha %d.\n"
#define FUNCTION_ALREADY_DECLARED "ERROR - Linha %d: Função de nome \"%s\" já declarada na linha %d.\n"
#define UNDECLARED_IDENTIFIER "ERROR - Linha %d: Identificador \"%s\" referido na linha %d não declarado.\n"
#define VARIABLE_AS_FUNCTION "ERROR - Linha %d: Esperava-se um identificador de função na linha %d, mas \"%s\" refere-se a uma variável da linha %d.\n"
#define FUNCTION_AS_VARIABLE "ERROR - Linha %d: Esperava-se um identificador de variável na linha %d, mas \"%s\" refere-se a uma função da linha %d.\n"
#define INCOMPATIBLE_TYPES "WARNING: a variável \"%s\" declarada na linha %d é do tipo \"%s\", enquanto que o valor atribuído é do tipo \"%s\".\n"

typedef struct lex_value {
    unsigned int line_number;
    char token_type;
    char *value;
} lex_value_t;