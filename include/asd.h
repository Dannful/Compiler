/*
Grupo C
Alunos:
Gustavo Sleman Lenz - 00290394
Vinicius Daniel Spadotto - 00341554

Arquivo obtido e alterado a partir dos arquivos disponibilizados pelo professor
no Moodle.
*/
#ifndef _ARVORE_H_
#define _ARVORE_H_

#include "iloc.h"
#include "list.h"

typedef enum {
    INT = 0,
    FLOAT = 1,
    UNKNOWN = 2
} DataType;

typedef struct asd_tree {
    char *label;
    List *code;
    register_identifier_t local;
    DataType data_type;
    int number_of_children;
    struct asd_tree **children;
} asd_tree_t;

const char *get_string_for_data_type(DataType data_type);

/*
 * Função asd_new, cria um nó sem filhos com o label informado.
 */
asd_tree_t *asd_new(const char *label);

/*
 * Função asd_tree, libera recursivamente o nó e seus filhos.
 */
void asd_free(asd_tree_t *tree);

/*
 * Função asd_add_child, adiciona child como filho de tree.
 */
void asd_add_child(asd_tree_t *tree, asd_tree_t *child);

/*
 * Função asd_print, imprime recursivamente a árvore.
 */
void asd_print(asd_tree_t *tree);

/*
 * Função asd_print_graphviz, idem, em formato DOT
 */
void asd_print_graphviz(asd_tree_t *tree);
#endif  //_ARVORE_H_
