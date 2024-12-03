/*
Grupo C
Alunos:
Gustavo Sleman Lenz - 00290394
Vinicius Daniel Spadotto - 00341554
*/

#include "generations.h"

#include <stdlib.h>
#include <string.h>

#include "asd.h"
#include "errors.h"
#include "stack.h"
#include "table.h"

Stack* g_tables_stack = NULL;
uint8_t last_register_number = 0;
uint8_t last_label_number = 0;

extern void *arvore;
extern int get_line_number();

Stack *get_tables_stack() {
    if (g_tables_stack == NULL) {
        g_tables_stack = create_stack();
        stack_push(g_tables_stack, table_create());
    }
    return g_tables_stack;
}

// Used in many generations
void parse_simple(asd_tree_t **head, asd_tree_t *body) { *head = body; }
void parse_null(asd_tree_t **head) { *head = NULL; }

void parse_generate_scope() { stack_push(get_tables_stack(), table_create()); }

void parse_destroy_scope() { stack_pop(get_tables_stack()); }

// Unary operators
void parse_unary_parenthesis(asd_tree_t **head, asd_tree_t *expression) {
    *head = expression;
    (*head)->data_type = expression->data_type;
}

void parse_unary_operator(asd_tree_t **head, const char *op, asd_tree_t *operand) {
    *head = asd_new(op);
    (*head)->data_type = operand->data_type;
    asd_add_child(*head, operand);
}

void parse_unary_identifier(asd_tree_t **head, lex_value_t *identifier) {
    TableEntry *entry = table_search(get_tables_stack(), identifier->value);
    if (entry == NULL) {
        printf(UNDECLARED_IDENTIFIER, get_line_number(), identifier->value, get_line_number());
        exit(ERR_UNDECLARED);
    }
    if (entry->entry_type == FUNCTION) {
        printf(FUNCTION_AS_VARIABLE, get_line_number(), get_line_number(), identifier->value, entry->line);
        exit(ERR_FUNCTION);
    }
    *head = asd_new(identifier->value);
    (*head)->data_type = entry->data_type;
    free(identifier);
}

void parse_binary_operator(asd_tree_t **head, const char *op, asd_tree_t *operand1, asd_tree_t *operand2) {
    *head = asd_new(op);
    if (operand1->data_type == FLOAT || operand2->data_type == FLOAT) {
        (*head)->data_type = FLOAT;
    } else {
        (*head)->data_type = INT;
    }
    asd_add_child(*head, operand1);
    asd_add_child(*head, operand2);
}

void parse_expression_construction(asd_tree_t **head, const char *name, asd_tree_t *expression,
                                   asd_tree_t *command_block) {
    *head = asd_new(name);
    asd_add_child(*head, expression);
    asd_add_child(*head, command_block);
}

void parse_expression_while(asd_tree_t **head, asd_tree_t *expression, asd_tree_t *command_block) {
    parse_expression_construction(head, "while", expression, command_block);
}

void parse_expression_if(asd_tree_t **head, asd_tree_t *expression, asd_tree_t *command_block) {
    parse_expression_construction(head, "if", expression, command_block);
}

void parse_expression_if_else(asd_tree_t **head, asd_tree_t *expression, asd_tree_t *command_block_then,
                              asd_tree_t *command_block_else) {
    parse_expression_if(head, expression, command_block_then);
    asd_add_child(*head, command_block_else);
}

void parse_expression_return(asd_tree_t **head, asd_tree_t *expression) {
    *head = asd_new("return");
    asd_add_child(*head, expression);
}

void parse_argument_list(asd_tree_t *expression, asd_tree_t *argument_list) {
    asd_add_child(expression, argument_list);
}

void parse_function_call(asd_tree_t **head, lex_value_t *identifier, asd_tree_t *argument_list) {
    TableEntry *entry = table_search(get_tables_stack(), identifier->value);
    if (entry == NULL) {
        printf(UNDECLARED_IDENTIFIER, get_line_number(), identifier->value, get_line_number());
        exit(ERR_UNDECLARED);
    }
    if (entry->entry_type == VARIABLE) {
        printf(VARIABLE_AS_FUNCTION, get_line_number(), get_line_number(), identifier->value, entry->line);
        exit(ERR_VARIABLE);
    }
    char node_label[strlen(identifier->value) + 5];
    sprintf(node_label, "call %s", identifier->value);
    *head = asd_new(node_label);
    (*head)->data_type = entry->data_type;
    asd_add_child(*head, argument_list);
    free(identifier);
}

void parse_assignment(asd_tree_t **head, lex_value_t *identifier, asd_tree_t *expression) {
    TableEntry *entry = table_search(get_tables_stack(), identifier->value);
    if (entry == NULL) {
        printf(UNDECLARED_IDENTIFIER, get_line_number(), identifier->value, get_line_number());
        exit(ERR_UNDECLARED);
    }
    if (entry->entry_type == FUNCTION) {
        printf(FUNCTION_AS_VARIABLE, get_line_number(), get_line_number(), identifier->value, entry->line);
        exit(ERR_FUNCTION);
    }
    *head = asd_new("=");
    (*head)->data_type = entry->data_type;
    if ((*head)->data_type != expression->data_type) {
        printf(INCOMPATIBLE_TYPES, identifier->value, get_line_number(), get_string_for_data_type((*head)->data_type),
               get_string_for_data_type(expression->data_type));
    }
    iloc_instruction_t instruction;
    instruction.mnemonic = "storeAI";
    instruction.type = REGISTER_DEST_OPERAND;

    instruction.data.source.type = GENERAL;
    instruction.data.source.identifier = expression->local;

    instruction.destination.type = FRAME_POINTER;
    instruction.destination.operand = entry->offset;

    asd_add_child(*head, asd_new(identifier->value));
    asd_add_child(*head, expression);
    (*head)->code = expression->code;
    list_add((*head)->code, instruction);
    free(identifier);
}

void parse_variable(asd_tree_t **head, lex_value_t *identifier) {
    Table *table = stack_peek(get_tables_stack());
    TableEntry *foundValue = table_get(table, identifier->value);
    if (foundValue != NULL) {
        printf(VARIABLE_ALREADY_DECLARED, get_line_number(), identifier->value, foundValue->line);
        exit(ERR_DECLARED);
    }
    table_set_value(table, identifier->value, VARIABLE, "");
    *head = NULL;
    free(identifier);
}

void parse_variable_literal(asd_tree_t **head, lex_value_t *identifier, asd_tree_t *literal) {
    Table *table = stack_peek(get_tables_stack());
    TableEntry *foundValue = table_get(table, identifier->value);
    if (foundValue != NULL) {
        printf(VARIABLE_ALREADY_DECLARED, get_line_number(), identifier->value, foundValue->line);
        exit(ERR_DECLARED);
    }
    table_set_value(table, identifier->value, VARIABLE, "");
    *head = asd_new("<=");
    asd_add_child(*head, asd_new(identifier->value));
    asd_add_child(*head, literal);
    free(identifier);
}

void parse_variable_list(asd_tree_t **head, asd_tree_t *variable, asd_tree_t *variable_list) {
    if (variable == NULL) {
        *head = variable_list;
    } else {
        asd_add_child(variable, variable_list);
        *head = variable;
    }
}

void parse_declaration(asd_tree_t **head, asd_tree_t *type, asd_tree_t *variable_list) {
    Table *table = stack_peek(get_tables_stack());
    asd_tree_t *root = variable_list;
    for (int i = 0; i < table->size; i++) {
        if (table->elements[i] != NULL) {
            if (table->elements[i]->value.data_type != UNKNOWN) {
                continue;
            }
            table_set_value(table, table->elements[i]->key, VARIABLE, type->label);
        }
    }
    while (root != NULL && root->number_of_children > 0) {
        TableEntry *foundValue = table_get(table, root->children[0]->label);
        table_set_value(table, root->children[0]->label, VARIABLE, type->label);
        root->data_type = foundValue->data_type;
        if (root->children[1]->data_type != root->data_type) {
            printf(INCOMPATIBLE_TYPES, root->children[0]->label, get_line_number(),
                   get_string_for_data_type(root->data_type), get_string_for_data_type(root->children[1]->data_type));
        }
        root = root->children[root->number_of_children - 1];
    }
    *head = variable_list;
}

void parse_simple_command_list(asd_tree_t **head, asd_tree_t *command, asd_tree_t *command_list) {
    if (command == NULL) {
        *head = command_list;
    } else {
        if (strcmp(command->label, "<=") == 0) {
            asd_tree_t *last = command;
            while (last->number_of_children == 3) last = last->children[last->number_of_children - 1];
            asd_add_child(last, command_list);
        } else {
            asd_add_child(command, command_list);
        }
    }
}

void parse_parameter(asd_tree_t **head, lex_value_t *identifier, asd_tree_t *type) {
    Table *table = stack_peek(get_tables_stack());
    TableEntry *value = table_get(table, identifier->value);
    if (value != NULL) {
        printf(VARIABLE_ALREADY_DECLARED, get_line_number(), identifier->value, value->line);
        exit(ERR_DECLARED);
    }
    table_set_value(table, identifier->value, VARIABLE, type->label);
    *head = NULL;
    free(identifier);
}

void parse_literal(asd_tree_t **head, lex_value_t *literal, DataType data_type) {
    *head = asd_new(strdup(literal->value));
    (*head)->data_type = data_type;
    free(literal);
}

void parse_type(asd_tree_t **head, const char *type_name) { *head = asd_new(type_name); }

void parse_function_header(asd_tree_t **head, lex_value_t *identifier, asd_tree_t *type) {
    Table *table = get_tables_stack()->tail->previous->value;
    TableEntry *found_value = table_get(table, identifier->value);
    if (found_value != NULL) {
        printf(FUNCTION_ALREADY_DECLARED, get_line_number(), identifier->value, found_value->line);
        exit(ERR_DECLARED);
    }
    table_set_value(table, identifier->value, FUNCTION, type->label);
    *head = asd_new(identifier->value);
    free(identifier);
}

void parse_function(asd_tree_t **head, asd_tree_t *header, asd_tree_t *command_list) {
    asd_add_child(header, command_list);
    *head = header;
}

void parse_list(asd_tree_t *hd, asd_tree_t *tl) { asd_add_child(hd, tl); }

void parse_program(asd_tree_t **head, asd_tree_t *function_list) {
    *head = function_list;
    arvore = function_list;
}

uint8_t generate_register() {
    return last_register_number++;
}

uint8_t generate_label() {
    return last_label_number++;
}
