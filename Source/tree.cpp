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

expression_t copy_expression(expression_t expression){
    if (expression == nullptr) return nullptr;

    if (expression->type == VAR){
        return create_var_node(expression->val.var + 'a');
    }
    else if (expression->type == NUM){
        return create_num_node(expression->val.num);
    }
    else {
        return create_op_node(expression->val.op,
                                   copy_expression(expression->left),
                                  copy_expression(expression->right));
    }

    return nullptr;
}



static expression_t convolution_of_constants(expression_t expression, bool *no_changes);
static expression_t removal_of_neutral_elements(expression_t expression, bool *no_changes);

expression_t simplification_of_expression(expression_t expression){
    bool no_changes = false;

    while (!no_changes){
        no_changes = true;
        expression = removal_of_neutral_elements(expression, &no_changes);
        expression = convolution_of_constants(expression, &no_changes);
    }

    return expression;
}

static expression_t convolution_of_constants(expression_t expression, bool *no_changes){
    if (expression->left && expression->left->type == OP){
        expression->left = convolution_of_constants(expression->left, no_changes);
    }

    if (expression->right && expression->right->type == OP){
        expression->right = convolution_of_constants(expression->right, no_changes);
    }

    if (expression->type == OP &&
        expression->left->type == NUM && 
       (expression->right == nullptr || expression->right->type == NUM)){
        for (size_t pos_op_list = 0; pos_op_list < sizeof(op_list) / sizeof(op_list[0]); pos_op_list++){
            if (op_list[pos_op_list].op == expression->val.op){
                *no_changes = false;

                double val1 = expression->left->val.num;
                double val2 = expression->right ? expression->right->val.num : 0;

                destroy_expression(expression);

                return create_num_node(op_list[pos_op_list].op_func(val1, val2));
            }
        }
    }

    return expression;
}

double add_func   (double par1,                  double par2) { return par1 + par2            ; }
double sub_func   (double par1,                  double par2) { return par1 - par2            ; }
double mul_func   (double par1,                  double par2) { return par1 * par2            ; }
double div_func   (double par1,                  double par2) { return par1 / par2            ; }
double pow_func   (double par1,                  double par2) { return pow(par1, par2)        ; }
double log_func   (double par1,                  double par2) { return log(par1) / log(par2)  ; }
double ln_func    (double par1, [[maybe_unused]] double par2) { return log(par1)              ; }
double sin_func   (double par1, [[maybe_unused]] double par2) { return sin(par1)              ; }
double cos_func   (double par1, [[maybe_unused]] double par2) { return cos(par1)              ; }
double TG_func    (double par1, [[maybe_unused]] double par2) { return sin(par1) / cos(par1)  ; }
double ctg_func   (double par1, [[maybe_unused]] double par2) { return cos(par1) / sin(par1)  ; }
double arcsin_func(double par1, [[maybe_unused]] double par2) { return asin(par1)             ; }
double arccos_func(double par1, [[maybe_unused]] double par2) { return acos(par1)             ; }
double arctg_func (double par1, [[maybe_unused]] double par2) { return atan(par1)             ; }
double arcctg_func(double par1, [[maybe_unused]] double par2) { return M_PI_2 - atan(par1)    ; }
double sh_func    (double par1, [[maybe_unused]] double par2) { return sinh(par1)             ; }
double ch_func    (double par1, [[maybe_unused]] double par2) { return cosh(par1)             ; }
double th_func    (double par1, [[maybe_unused]] double par2) { return tanh(par1)             ; }
double cth_func   (double par1, [[maybe_unused]] double par2) { return cosh(par1) / sinh(par1); }
double exp_func   (double par1, [[maybe_unused]] double par2) { return exp(par1)              ; }

static expression_t removal_of_neutral_elements(expression_t expression, bool *no_changes){
    if (expression->left && expression->left->type == OP){
        expression->left = removal_of_neutral_elements(expression->left, no_changes);
    }

    if (expression->right && expression->right->type == OP){
        expression->right = removal_of_neutral_elements(expression->right, no_changes);
    }

    expression_t new_expression = nullptr;
    if (expression->type == OP){
        if (expression->val.op == MUL){
            if ((expression->left->type  == NUM && doubles_are_equal(expression->left->val.num, 0)) ||
                (expression->right->type == NUM && doubles_are_equal(expression->right->val.num, 0))){
                    *no_changes = false;
                    new_expression = create_num_node(0);

                    destroy_expression(expression);
            }
            else if (expression->left->type == NUM && doubles_are_equal(expression->left->val.num, 1)){
                *no_changes = false;  
                new_expression = expression->right;

                destroy_expression(expression->left);
                free(expression);              
            }
            else if (expression->right->type == NUM && doubles_are_equal(expression->right->val.num, 1)){
                *no_changes = false;
                new_expression = expression->left;

                destroy_expression(expression->right);
                free(expression); 
            }
        }
        else if (expression->val.op == ADD){
            if (expression->left->type == NUM && doubles_are_equal(expression->left->val.num, 0)){
                *no_changes = false;
                new_expression = expression->right;

                destroy_expression(expression->left);
                free(expression); 
            }
            else if (expression->right->type == NUM && doubles_are_equal(expression->right->val.num, 0)){
                *no_changes = false;
                new_expression = expression->left;

                destroy_expression(expression->right);
                free(expression); 
            }
        }
        else if (expression->val.op == SUB){
            if (expression->right->type == NUM && doubles_are_equal(expression->right->val.num, 0)){
                *no_changes = false;
                new_expression = expression->left;

                destroy_expression(expression->right);
                free(expression); 
            }
        }
        else if (expression->val.op == POW){
            if (expression->right->type == NUM && doubles_are_equal(expression->right->val.num, 1)){
                *no_changes = false;
                new_expression = expression->left;

                destroy_expression(expression->right);
                free(expression); 
            }
            else if (expression->right->type == NUM && doubles_are_equal(expression->right->val.num, 0)){
                *no_changes = false;
                new_expression = create_num_node(1);

                destroy_expression(expression);
            }
        }
        else if (expression->val.op == DIV){
            if (expression->right->type == NUM && doubles_are_equal(expression->right->val.num, 1)){
                *no_changes = false;
                new_expression = expression->left;

                destroy_expression(expression->right);
                free(expression); 
            }

            else if (expression->right->type == OP && expression->right->val.op == DIV){
                *no_changes = false;
                new_expression = DIV_(
                                     MUL_(
                                         expression->left,
                                         expression->right->right
                                    ),
                                    expression->right->left
                                );
                free(expression); 
            }
        }   
    }

    if (new_expression){
        return new_expression;
    }

    return expression;
}

bool doubles_are_equal(double par1, double par2){
    return fabs(par1 - par2) < precision;
}