#include "parser.h"

#include "asd.h"
#include "errors.h"
#include "table.h"

// Used in many generations
void parse_simple(asd_tree_t* head, asd_tree_t* body) { head = body; }

// Unary operators
void parse_unary_parenthesis(asd_tree_t* head, asd_tree_t* expression) {
    head = expression;
    head->data_type = expression->data_type;
}

void parse_unary_operator(asd_tree_t* head, const char* op, asd_tree_t* operand) {
    head = asd_new(op);
    head->data_type = operand->data_type;
}

void parse_unary_identifier(asd_tree_t* head, lex_value_t* identifier) {
    TableEntry* entry = table_search(get_tables_stack(), identifier->value);
    if (entry == NULL) {
        printf(UNDECLARED_IDENTIFIER, get_line_number(), identifier->value, get_line_number());
        exit(ERR_UNDECLARED);
    }
    if (entry->entry_type == FUNCTION) {
        printf(FUNCTION_AS_VARIABLE, get_line_number(), get_line_number(), identifier->value, entry->line);
        exit(ERR_FUNCTION);
    }
    head = asd_new(identifier->value);
    head->data_type = entry->data_type;
}

void parse_binary_operator(asd_tree_t* head, const char* op, asd_tree_t* operand1, asd_tree_t* operand2) {
    head = asd_new(op);
    if (operand1->data_type == FLOAT || operand2->data_type == FLOAT) {
        head->data_type = FLOAT;
    } else {
        head->data_type = INT;
    }
    asd_add_child(head, operand1);
    asd_add_child(head, operand2);
}

void parse_expression_construction(asd_tree_t* head, const char* name, asd_tree_t* expression,
                                   asd_tree_t* command_block) {
    head = asd_new(name);
    asd_add_child(head, expression);
    asd_add_child(head, command_block);
}

void parse_expression_while(asd_tree_t* head, asd_tree_t* expression, asd_tree_t* command_block) {
    parse_expression_construction(head, "while", expression, command_block);
}

void parse_expression_if(asd_tree_t* head, asd_tree_t* expression, asd_tree_t* command_block) {
    parse_expression_construction(head, "if", expression, command_block);
}

void parse_expression_if_else(asd_tree_t* head, asd_tree_t* expression, asd_tree_t* command_block_then,
                              asd_tree_t* command_block_else) {
    parse_expression_if(head, expression, command_block_then);
    asd_add_child(head, command_block_else);
}

void parse_expression_return(asd_tree_t* head, asd_tree_t* expression) {
    head = asd_new("return");
    asd_add_child(head, expression);
}

void parse_argument_list(asd_tree_t* expression, asd_tree_t* argument_list) {
    asd_add_child(expression, argument_list);
}

void parse_function_call(asd_tree_t* head, lex_value_t* identifier, asd_tree_t* argument_list) {
    TableEntry* entry = table_search(get_tables_stack(), identifier->value);
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
    head = asd_new(node_label);
    head->data_type = entry->data_type;
    asd_add_child(head, argument_list);
}

void parse_assignment(asd_tree_t* head, lex_value_t* identifier, asd_tree_t* expression) {
    TableEntry* entry = table_search(get_tables_stack(), identifier->value);
    if (entry == NULL) {
        printf(UNDECLARED_IDENTIFIER, get_line_number(), identifier->value, get_line_number());
        exit(ERR_UNDECLARED);
    }
    if (entry->entry_type == FUNCTION) {
        printf(FUNCTION_AS_VARIABLE, get_line_number(), get_line_number(), identifier->value, entry->line);
        exit(ERR_FUNCTION);
    }
    head = asd_new("=");
    head->data_type = entry->data_type;
    if (head->data_type != expression->data_type) {
        printf(INCOMPATIBLE_TYPES, identifier->value, get_line_number(), get_string_for_data_type(head->data_type),
               get_string_for_data_type(expression->data_type));
    }

    asd_add_child(head, asd_new(identifier->value));
    asd_add_child(head, expression);
}

void parse_variable(asd_tree_t* head, lex_value_t* identifier) {
    Table* table = stack_peek(get_tables_stack());
    TableEntry* foundValue = table_get(table, identifier->value);
    if (foundValue != NULL) {
        printf(VARIABLE_ALREADY_DECLARED, get_line_number(), identifier->value, foundValue->line);
        exit(ERR_DECLARED);
    }
    table_set_value(table, identifier->value, VARIABLE, "");
    head = NULL;
}

void parse_variable_literal(asd_tree_t* head, lex_value_t* identifier, asd_tree_t* literal) {
    Table* table = stack_peek(get_tables_stack());
    TableEntry* foundValue = table_get(table, identifier->value);
    if (foundValue != NULL) {
        printf(VARIABLE_ALREADY_DECLARED, get_line_number(), identifier->value, foundValue->line);
        exit(ERR_DECLARED);
    }
    table_set_value(table, identifier->value, VARIABLE, "");
    head = asd_new("<=");
    asd_add_child(head, asd_new(identifier->value));
    asd_add_child(head, literal);
}