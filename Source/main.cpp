#include "tree.h"

int main(){

    expression_t expr = get_tree_from_file("expression.txt");
    fprintf(stderr, "%d %lf %p", expr->val.op, expr->val.num, expr->right);
    DUMP(expr);
}