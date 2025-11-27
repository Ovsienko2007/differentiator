#include "tree.h"

static expression_t find_derivative_of_num(expression_t expression){
    destroy_expression(expression);
    return create_num_node(0);
}

static expression_t find_derivative_of_var(expression_t expression, int par){
    if (expression->val.var != par - 'a'){
        return find_derivative_of_num(expression);
    }

    destroy_expression(expression);
    return create_num_node(1);
}

static expression_t find_derivative_of_sum(expression_t expression, int par){
    expression_t new_expression = ADD_(
                                      D_(expression->left, par),
                                      D_(expression->right, par)
                                  );

    free(expression);
    return new_expression;
}

static expression_t find_derivative_of_sub(expression_t expression, int par){
    expression_t new_expression = SUB_(
                                      D_(expression->left, par),
                                      D_(expression->right, par)
                                  );

    free(expression);
    return new_expression;
}

static expression_t find_derivative_of_mul(expression_t expression, int par){
    expression_t new_expression = ADD_(
                                      MUL_(
                                          D_(C_(expression->left), par),
                                          C_(expression->right)
                                      ), 
                                      MUL_(
                                          C_(expression->left),
                                          D_(C_(expression->right), par)
                                      )
                                  );

    free(expression);
    return new_expression;
}

static expression_t find_derivative_of_div(expression_t expression, int par){
    expression_t new_expression = DIV_(
                                      SUB_(
                                          MUL_(
                                              D_(C_(expression->left), par),
                                              C_(expression->right)
                                          ), 
                                          MUL_(
                                              C_(expression->left),
                                              D_(C_(expression->right), par)
                                          )
                                      ), 
                                      POW_(
                                        C_(expression->right),
                                        create_num_node(2)
                                      )
                                  );

    destroy_expression(expression);
    return new_expression;
}

static expression_t find_derivative_of_exp(expression_t expression, int par){
    expression_t new_expression = MUL_(
                                      C_(expression),
                                      D_(C_(expression->left), par)
                                  );

    destroy_expression(expression);
    return new_expression;
}

static expression_t find_derivative_of_ln(expression_t expression, int par){
    expression_t new_expression = MUL_(
                                      DIV_(create_num_node(1),
                                          C_(expression->left)
                                      ),
                                      D_(C_(expression->left), par)
                                  );
    destroy_expression(expression);
    return new_expression;
}

static expression_t find_derivative_of_sin(expression_t expression, int par){
    expression_t new_expression = MUL_(
                                      COS_(C_(expression->left)),
                                      D_(C_(expression->left), par)
                                  );

    destroy_expression(expression);
    return new_expression;
}

static expression_t find_derivative_of_cos(expression_t expression, int par){
    expression_t new_expression = MUL_(
                                      SUB_(
                                        create_num_node(0),
                                        COS_(C_(expression->left))
                                      ),
                                      D_(C_(expression->left), par)
                                  );

    destroy_expression(expression);
    return new_expression;
}

static expression_t find_derivative_of_tg(expression_t expression, int par){
    expression_t new_expression = MUL_(
                                      DIV_(
                                        create_num_node(1),
                                        COS_(C_(expression->left)
                                      )
                                  ),
                                      D_(C_(expression->left), par)
                                  );

    destroy_expression(expression);
    return new_expression;
}

static expression_t find_derivative_of_ctg(expression_t expression, int par){
    expression_t new_expression = MUL_(
                                      SUB_(
                                          create_num_node(0),
                                          DIV_(
                                              create_num_node(1),
                                              SIN_(C_(expression->left)
                                          )
                                      )
                                  ),
                                      D_(C_(expression->left), par)
                                  );

    destroy_expression(expression);
    return new_expression;
}

static expression_t find_derivative_of_arcsin(expression_t expression, int par){
    expression_t new_expression = MUL_(
                                      DIV_(
                                          create_num_node(1),
                                          POW_(  
                                              SUB_(
                                                  create_num_node(1),
                                                  C_(expression->left)
                                              ),
                                              create_num_node(0.5)
                                          )
                                      ),
                                      D_(C_(expression->left), par)
                                  );

    destroy_expression(expression);
    return new_expression;
}

static expression_t find_derivative_of_arccos(expression_t expression, int par){
    expression_t new_expression = MUL_(
                                      SUB_(
                                          create_num_node(0),
                                          DIV_(
                                              create_num_node(1),
                                              POW_(  
                                                  SUB_(
                                                      create_num_node(1),
                                                      C_(expression->left)
                                                  ),
                                                  create_num_node(0.5)
                                              )
                                          )
                                      ),
                                      D_(C_(expression->left), par)
                                  );

    destroy_expression(expression);
    return new_expression;
}

static expression_t find_derivative_of_arctg(expression_t expression, int par){
    expression_t new_expression = MUL_(
                                      DIV_(
                                          create_num_node(1),
                                          SUB_(
                                              create_num_node(1),
                                              C_(expression->left)
                                          )
                                      ),
                                      D_(C_(expression->left), par)
                                  );

    destroy_expression(expression);
    return new_expression;
}

static expression_t find_derivative_of_arcctg(expression_t expression, int par){
    expression_t new_expression = MUL_(
                                      SUB_(
                                          create_num_node(0),
                                          DIV_(
                                              create_num_node(1),
                                              SUB_(
                                                  create_num_node(1),
                                                  C_(expression->left)
                                              )
                                          )
                                      ),
                                      D_(C_(expression->left), par)
                                  );

    destroy_expression(expression);
    return new_expression;
}

static expression_t find_derivative_of_sh(expression_t expression, int par){
    expression_t new_expression = MUL_(
                                      CH_(C_(expression->left)),
                                      D_(C_(expression->left), par)
                                  );

    destroy_expression(expression);
    return new_expression;
}

static expression_t find_derivative_of_ch(expression_t expression, int par){
    expression_t new_expression = MUL_(
                                      SH_(C_(expression->left)),
                                      D_(C_(expression->left), par)
                                  );

    destroy_expression(expression);
    return new_expression;
}

static expression_t find_derivative_of_th(expression_t expression, int par){
    expression_t new_expression = MUL_(
                                      DIV_(
                                          create_num_node(1),
                                          CH_(C_(expression->left)
                                      )
                                  ),
                                      D_(C_(expression->left), par)
                                  );

    destroy_expression(expression);
    return new_expression;
}

static expression_t find_derivative_of_cth(expression_t expression, int par){
    expression_t new_expression = MUL_(
                                      DIV_(
                                          create_num_node(1),
                                          SH_(C_(expression->left)
                                      )
                                  ),
                                      D_(C_(expression->left), par)
                                  );

    destroy_expression(expression);
    return new_expression;
}

static expression_t find_derivative_of_log(expression_t expression, int par){
    expression_t new_expression = MUL_(
                                      DIV_(
                                          create_num_node(1),
                                          MUL_(
                                              C_(expression->right),
                                              LN_(C_(expression->left))
                                          )
                                      ),
                                      D_(C_(expression->right), par)
                                  );

    destroy_expression(expression);
    return new_expression;
}

static expression_t find_derivative_of_pow(expression_t expression, int par){
    expression_t new_expression = MUL_(
                                      C_(expression),
                                      ADD_(
                                          MUL_(
                                              D_(C_(expression->right), par),
                                              LN_(C_(expression->left))
                                          ),
                                          DIV_(
                                              MUL_(
                                                  C_(expression->right),
                                                  D_(C_(expression->left), par)
                                              ),
                                              C_(expression->left)
                                          )
                                      )
                                  );
    destroy_expression(expression);
    return new_expression;

}

expression_t find_derivative(expression_t expression, int par){
    
    if (expression->type == NUM)      return find_derivative_of_num   (expression);
    if (expression->type == VAR)      return find_derivative_of_var   (expression, par);

    switch (expression->val.op){
        case ADD   :                  return find_derivative_of_sum   (expression, par);
        case SUB   :                  return find_derivative_of_sub   (expression, par);
        case MUL   :                  return find_derivative_of_mul   (expression, par);
        case DIV   :                  return find_derivative_of_div   (expression, par);

        case SIN   :                  return find_derivative_of_sin   (expression, par);
        case COS   :                  return find_derivative_of_cos   (expression, par);
        case TG    :                  return find_derivative_of_tg    (expression, par);
        case CTG   :                  return find_derivative_of_ctg   (expression, par);

        case ARCSIN:                  return find_derivative_of_arcsin(expression, par);
        case ARCCOS:                  return find_derivative_of_arccos(expression, par);
        case ARCTG :                  return find_derivative_of_arctg (expression, par);
        case ARCCTG:                  return find_derivative_of_arcctg(expression, par);

        case SH    :                  return find_derivative_of_sh    (expression, par);
        case CH    :                  return find_derivative_of_ch    (expression, par);
        case TH    :                  return find_derivative_of_th    (expression, par);
        case CTH   :                  return find_derivative_of_cth   (expression, par);

        case EXP   :                  return find_derivative_of_exp   (expression, par);
        case LN    :                  return find_derivative_of_ln    (expression, par);
        case LOG   :                  return find_derivative_of_log   (expression, par);
        case POW   :                  return find_derivative_of_pow   (expression, par);

        default    :                  return nullptr;
    }
}