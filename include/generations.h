/*
Grupo C
Alunos:
Gustavo Sleman Lenz - 00290394
Vinicius Daniel Spadotto - 00341554
*/

#pragma once

#include "iloc.h"
#define VARIABLE_ALREADY_DECLARED "ERROR - Linha %d: Variável de nome \"%s\" já declarada na linha %d.\n"
#define FUNCTION_ALREADY_DECLARED "ERROR - Linha %d: Função de nome \"%s\" já declarada na linha %d.\n"
#define UNDECLARED_IDENTIFIER "ERROR - Linha %d: Identificador \"%s\" referido na linha %d não declarado.\n"
#define VARIABLE_AS_FUNCTION                                                                                           \
    "ERROR - Linha %d: Esperava-se um identificador de função na linha %d, mas \"%s\" refere-se a uma variável da " \
    "linha %d.\n"
#define FUNCTION_AS_VARIABLE                                                                                           \
    "ERROR - Linha %d: Esperava-se um identificador de variável na linha %d, mas \"%s\" refere-se a uma função da " \
    "linha %d.\n"
#define INCOMPATIBLE_TYPES                                                                                             \
    "WARNING: a variável \"%s\" declarada na linha %d é do tipo \"%s\", enquanto que o valor atribuído é do tipo " \
    "\"%s\".\n"

#include "asd.h"
#include "stack.h"

#include <stdint.h>

typedef struct lex_value {
    unsigned int line_number;
    char token_type;
    char *value;
} lex_value_t;

// Parsing function prototypes
void parse_simple(asd_tree_t **head, asd_tree_t *body);
void parse_null(asd_tree_t **head);
void parse_unary_parenthesis(asd_tree_t **head, asd_tree_t *expression);
void parse_unary_operator(asd_tree_t **head, const char *op, asd_tree_t *operand);
void parse_unary_identifier(asd_tree_t **head, lex_value_t *identifier);
void parse_binary_operator(asd_tree_t **head, const char *op, asd_tree_t *operand1, asd_tree_t *operand2);
void parse_expression_construction(asd_tree_t **head, const char *name, asd_tree_t *expression,
                                   asd_tree_t *command_block);
void parse_expression_while(asd_tree_t **head, asd_tree_t *expression, asd_tree_t *command_block);
void parse_expression_condition(asd_tree_t **head, asd_tree_t *expression, asd_tree_t *command_block, asd_tree_t *command_block_else);
void parse_expression_return(asd_tree_t **head, asd_tree_t *expression);
void parse_function_call(asd_tree_t **head, lex_value_t *identifier, asd_tree_t *argument_list);
void parse_assignment(asd_tree_t **head, lex_value_t *identifier, asd_tree_t *expression);
void parse_variable(asd_tree_t **head, lex_value_t *identifier);
void parse_variable_literal(asd_tree_t **head, lex_value_t *identifier, asd_tree_t *literal);
void parse_variable_list(asd_tree_t **head, asd_tree_t *variable, asd_tree_t *variable_list);
void parse_declaration(asd_tree_t **head, asd_tree_t *type, asd_tree_t *variable_list);
void parse_simple_command_list(asd_tree_t **head, asd_tree_t *command, asd_tree_t *command_list);
void parse_parameter(asd_tree_t **head, lex_value_t *identifier, asd_tree_t *type);
void parse_literal(asd_tree_t **head, lex_value_t *literal, DataType data_type);
void parse_type(asd_tree_t **head, const char *type_name);
void parse_function_header(asd_tree_t **head, lex_value_t *identifier, asd_tree_t *type);
void parse_function(asd_tree_t **head, asd_tree_t *header, asd_tree_t *command_list);
void parse_list(asd_tree_t *hd, asd_tree_t *tl);
void parse_program(asd_tree_t **head, asd_tree_t *function_list);
void parse_generate_scope();
void parse_destroy_scope();

// External declarations
extern void *arvore;
extern int get_line_number();

// Labels and registers generation
register_identifier_t generate_register();
label_identifier_t generate_label();

// Global variable declaration
extern Stack *g_tables_stack;
