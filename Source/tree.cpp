#include "tree.h"

node_t *create_num_node(double value) {
    node_t *new_node  = (node_t *)calloc(1, sizeof(node_t));

    new_node->type    = NUM;
    new_node->val.num = value;
    return new_node;
}

node_t *create_var_node(int var_id) {
    node_t *new_node  = (node_t *)calloc(1, sizeof(node_t));

    new_node->type    = VAR;
    new_node->val.var = var_id - 'a';

    return new_node;
}

node_t *create_op_node(op_t operation, node_t *left, node_t *right) {
    node_t *new_node  = (node_t *)calloc(1, sizeof(node_t));

    new_node->type   = OP;
    new_node->val.op = operation;
    new_node->left   = left;
    new_node->right  = right;

    return new_node;
}

void destroy_expression(expression_t expression){
    if (expression->left) {
        destroy_expression(expression->left);
    }
    if (expression->right) {
        destroy_expression(expression->right);
    }

    free(expression);

    return;
}

