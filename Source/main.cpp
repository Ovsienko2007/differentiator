#include "tree.h"

int main(){  // TODO 0-... -> -...

    expression_t expr = get_general("expression.txt");
    DUMP(expr);

    DUMP(simplification_of_expression(D_(expr, 'x')));
}