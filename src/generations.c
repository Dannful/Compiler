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
#include "sys/types.h"
#include "table.h"

Stack *g_tables_stack = NULL;
register_identifier_t last_register_number = 1;
label_identifier_t last_label_number = 0;

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
void parse_simple(asd_tree_t **head, asd_tree_t *body) {
    *head = body;
}
void parse_null(asd_tree_t **head) {
    *head = NULL;
}

register_t generate_zero_register(List *out) {
  iloc_instruction_t load_zero_into_reg;
  load_zero_into_reg.mnemonic = "loadI";
  load_zero_into_reg.type = OPERAND_DREG;
  load_zero_into_reg.operands.value = 0;
  load_zero_into_reg.destination.reg.type = GENERAL;
  load_zero_into_reg.destination.reg.identifier = generate_register();

  list_add(out, load_zero_into_reg);

  return load_zero_into_reg.destination.reg.identifier;
}

void parse_generate_scope() {
    Table *new_table = table_create();
    Stack *stack = get_tables_stack();
    if (stack->count >= 2)
        new_table->offset = stack->tail->value->offset;
    stack_push(stack, new_table);
}

void parse_destroy_scope() {
    stack_pop(get_tables_stack());
}

// Unary operators
void parse_unary_parenthesis(asd_tree_t **head, asd_tree_t *expression) {
    *head = expression;
    (*head)->data_type = expression->data_type;
}

void handle_not_instruction(asd_tree_t **head, const char *op, asd_tree_t *operand) {
    if (strcmp(op, "!") != 0)
        return;
    register_identifier_t reg_with_result = generate_register();

    iloc_instruction_t not_instruction;
    not_instruction.mnemonic = "cmp_EQ";
    not_instruction.type = SREG_SREG_DREG;
    not_instruction.operands.sources[0].type = GENERAL;
    not_instruction.operands.sources[1].type = GENERAL;

    not_instruction.operands.sources[0].identifier = operand->local;
    not_instruction.operands.sources[1].identifier = generate_zero_register(operand->code);

    not_instruction.destination.reg.type = GENERAL;
    not_instruction.destination.reg.identifier = reg_with_result;

    (*head)->code = operand->code;
    list_add((*head)->code, not_instruction);
    (*head)->local = reg_with_result;
}

void handle_minus_instruction(asd_tree_t **head, const char *op, asd_tree_t *operand) {
    if (strcmp(op, "-") != 0)
        return;
    register_identifier_t reg_with_result = generate_register();

    iloc_instruction_t minus_instruction;
    minus_instruction.mnemonic = "multI";
    minus_instruction.type = SREG_OPERAND_DREG;
    minus_instruction.operands.reg_and_value.source.type = GENERAL;
    minus_instruction.operands.reg_and_value.source.identifier = operand->local;
    minus_instruction.operands.reg_and_value.value = -1;

    minus_instruction.destination.reg.type = GENERAL;
    minus_instruction.destination.reg.identifier = reg_with_result;

    (*head)->code = operand->code;
    list_add((*head)->code, minus_instruction);
    (*head)->local = reg_with_result;
}

void parse_unary_operator(asd_tree_t **head, const char *op, asd_tree_t *operand) {
    *head = asd_new(op);
    (*head)->data_type = operand->data_type;
    handle_not_instruction(head, op, operand);
    handle_minus_instruction(head, op, operand);
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
    register_identifier_t output_register = generate_register();
    iloc_instruction_t instruction;
    instruction.mnemonic = "loadAI";
    instruction.type = SREG_OPERAND_DREG;
    instruction.operands.reg_and_value.source.type = FRAME_POINTER;
    instruction.operands.reg_and_value.value = entry->offset;

    instruction.destination.reg.type = GENERAL;
    instruction.destination.reg.identifier = output_register;

    *head = asd_new(identifier->value);
    (*head)->data_type = entry->data_type;
    (*head)->code = create_list();
    list_add((*head)->code, instruction);
    (*head)->local = output_register;
    free(identifier);
}

const char *get_mnemonic_for_binary_operation(const char *op) {
    if (strcmp(op, "+") == 0)
        return "add";
    if (strcmp(op, "-") == 0)
        return "sub";
    if (strcmp(op, "/") == 0)
        return "div";
    if (strcmp(op, "*") == 0)
        return "mult";
    if (strcmp(op, "|") == 0)
        return "or";
    if (strcmp(op, "&") == 0)
        return "and";
    if (strcmp(op, "!=") == 0)
        return "cmp_NE";
    if (strcmp(op, "==") == 0)
        return "cmp_EQ";
    if (strcmp(op, ">=") == 0)
        return "cmp_GE";
    if (strcmp(op, "<=") == 0)
        return "cmp_LE";
    if (strcmp(op, ">") == 0)
        return "cmp_GT";
    if (strcmp(op, "<") == 0)
        return "cmp_LT";
    return "nop";
}

void handle_binary_operation_code(asd_tree_t **head, const char *op, asd_tree_t *operand1, asd_tree_t *operand2) {
    register_identifier_t result_register = generate_register();
    iloc_instruction_t instruction;
    instruction.mnemonic = get_mnemonic_for_binary_operation(op);
    instruction.type = SREG_SREG_DREG;

    instruction.operands.sources[0].type = GENERAL;
    instruction.operands.sources[0].identifier = operand1->local;

    instruction.operands.sources[1].type = GENERAL;
    instruction.operands.sources[1].identifier = operand2->local;

    instruction.destination.reg.type = GENERAL;
    instruction.destination.reg.identifier = result_register;

    (*head)->local = result_register;
    (*head)->code = operand1->code;
    list_append((*head)->code, operand2->code);
    list_add((*head)->code, instruction);
}

void parse_binary_operator(asd_tree_t **head, const char *op, asd_tree_t *operand1, asd_tree_t *operand2) {
    *head = asd_new(op);
    if (operand1->data_type == FLOAT || operand2->data_type == FLOAT) {
        (*head)->data_type = FLOAT;
    } else {
        (*head)->data_type = INT;
    }
    handle_binary_operation_code(head, op, operand1, operand2);
    asd_add_child(*head, operand1);
    asd_add_child(*head, operand2);
}

void parse_expression_construction(asd_tree_t **head, const char *name, asd_tree_t *expression,
                                   asd_tree_t *command_block) {
    *head = asd_new(name);
    (*head)->code = expression->code;
    asd_add_child(*head, expression);
    asd_add_child(*head, command_block);
}

void generate_loop_code(asd_tree_t **head, asd_tree_t *expression, asd_tree_t *command_block) {
    label_identifier_t before_label = generate_label();
    label_identifier_t command_label = generate_label();
    label_identifier_t after_label = generate_label();

    iloc_instruction_t branch;
    branch.mnemonic = "cbr";
    branch.type = SREG_LABEL_LABEL;
    branch.operands.source.type = GENERAL;
    branch.operands.source.identifier = expression->local;
    branch.destination.labels[0] = command_label;
    branch.destination.labels[1] = after_label;

    iloc_instruction_t jump;
    jump.mnemonic = "jumpI";
    jump.type = JUMP;
    jump.destination.label = before_label;

    iloc_instruction_t before_label_instruction;
    before_label_instruction.type = LABEL;
    before_label_instruction.operands.label = before_label;

    iloc_instruction_t command_label_instruction;
    command_label_instruction.type = LABEL;
    command_label_instruction.operands.label = command_label;

    iloc_instruction_t after_label_instruction;
    after_label_instruction.type = LABEL;
    after_label_instruction.operands.label = after_label;

    (*head)->code = create_list();
    list_add((*head)->code, before_label_instruction);
    list_append((*head)->code, expression->code);
    list_add((*head)->code, branch);
    list_add((*head)->code, command_label_instruction);
    list_append((*head)->code, command_block->code);
    list_add((*head)->code, jump);
    list_add((*head)->code, after_label_instruction);
}

void parse_expression_while(asd_tree_t **head, asd_tree_t *expression, asd_tree_t *command_block) {
    parse_expression_construction(head, "while", expression, command_block);
    generate_loop_code(head, expression, command_block);
}

void generate_condition_code(asd_tree_t **head, asd_tree_t *expression, asd_tree_t *command_block,
                             asd_tree_t *command_block_else) {
    label_identifier_t true_label = generate_label();
    label_identifier_t false_label = generate_label();
    label_identifier_t after_label = generate_label();

    iloc_instruction_t branch;
    branch.mnemonic = "cbr";
    branch.type = SREG_LABEL_LABEL;
    branch.operands.source.type = GENERAL;
    branch.operands.source.identifier = expression->local;
    branch.destination.labels[0] = true_label;
    branch.destination.labels[1] = false_label;

    iloc_instruction_t jump;
    jump.mnemonic = "jumpI";
    jump.type = JUMP;
    jump.destination.label = after_label;

    iloc_instruction_t true_label_instruction;
    true_label_instruction.type = LABEL;
    true_label_instruction.operands.label = true_label;

    iloc_instruction_t false_label_instruction;
    false_label_instruction.type = LABEL;
    false_label_instruction.operands.label = false_label;

    iloc_instruction_t after_label_instruction;
    after_label_instruction.type = LABEL;
    after_label_instruction.operands.label = after_label;

    (*head)->code = expression->code;
    list_add((*head)->code, branch);
    list_add((*head)->code, true_label_instruction);
    list_append((*head)->code, command_block->code);
    list_add((*head)->code, jump);
    list_add((*head)->code, false_label_instruction);
    if (command_block_else != NULL)
        list_append((*head)->code, command_block_else->code);
    list_add((*head)->code, after_label_instruction);
}

void parse_expression_condition(asd_tree_t **head, asd_tree_t *expression, asd_tree_t *command_block,
                                asd_tree_t *command_block_else) {
    parse_expression_construction(head, "if", expression, command_block);
    if (command_block_else != NULL)
        asd_add_child(*head, command_block_else);
    generate_condition_code(head, expression, command_block, command_block_else);
}

void parse_expression_return(asd_tree_t **head, asd_tree_t *expression) {
    *head = asd_new("return");
    asd_add_child(*head, expression);
    iloc_instruction_t ret;
    ret.mnemonic = "return";
    ret.type = RETURN;
    ret.destination.reg.type = GENERAL;
    ret.destination.reg.identifier = expression->local;
    (*head)->code = expression->code;
    list_add((*head)->code, ret);
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

void generate_store_immediate(asd_tree_t **head, asd_tree_t *value, uint32_t offset) {
    iloc_instruction_t instruction;
    instruction.mnemonic = "storeAI";
    instruction.type = SREG_DREG_OPERAND;

    instruction.operands.source.type = GENERAL;
    instruction.operands.source.identifier = value->local;

    instruction.destination.reg_and_value.source.type = FRAME_POINTER;
    instruction.destination.reg_and_value.value = offset;

    (*head)->code = value->code;
    list_add((*head)->code, instruction);
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
    asd_add_child(*head, asd_new(identifier->value));
    asd_add_child(*head, expression);
    generate_store_immediate(head, expression, entry->offset);
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
    uint32_t offset = table->offset;
    table_set_value(table, identifier->value, VARIABLE, "");
    *head = asd_new("<=");
    (*head)->code = literal->code;
    generate_store_immediate(head, literal, offset);
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
        list_append((*head)->code, variable_list->code);
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
            while (last->number_of_children == 3)
                last = last->children[last->number_of_children - 1];
            asd_add_child(last, command_list);
        } else {
            asd_add_child(command, command_list);
        }
        (*head)->code = command->code;
        list_append((*head)->code, command_list->code);
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
    register_identifier_t register_identifier = generate_register();
    iloc_instruction_t instruction;
    instruction.mnemonic = "loadI";
    instruction.type = OPERAND_DREG;
    instruction.operands.value = atoi(literal->value);
    instruction.destination.reg.type = GENERAL;
    instruction.destination.reg.identifier = register_identifier;

    *head = asd_new(strdup(literal->value));
    (*head)->data_type = data_type;
    (*head)->code = create_list();
    list_add((*head)->code, instruction);
    (*head)->local = register_identifier;
    free(literal);
}

void parse_type(asd_tree_t **head, const char *type_name) {
    *head = asd_new(type_name);
}

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
    if (strcmp(header->label, "main") == 0)
        (*head)->code = command_list->code;
}

void parse_list(asd_tree_t *hd, asd_tree_t *tl) {
    asd_add_child(hd, tl);
}

void parse_program(asd_tree_t **head, asd_tree_t *function_list) {
    *head = function_list;
    arvore = function_list;
    printf("%s\n", generate_program(function_list->code));
}

register_identifier_t generate_register() {
    return last_register_number++;
}

label_identifier_t generate_label() {
    return last_label_number++;
}
