#ifndef TREE_H_
#define TREE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

struct dump_position {
    const char *file;
    const char *func;
    int         line;
};

#define DUMP(expression) show_dump(expression, dump_position {__FILE__, __FUNCTION__,  __LINE__})

enum type_node{
    OP  = 0,
    NUM = 1,
    VAR = 2,
};

enum op_t{
    ADD    = 0,
    SUB    = 1,
    MUL    = 2,
    DIV    = 3,
    POW    = 4,
    LOG    = 5,
    LN     = 6,
    SIN    = 7,
    COS    = 8,
    TG     = 9,
    CTG    = 10,
    ARCSIN = 11,
    ARCCOS = 12,
    ARCTG  = 13,
    ARCCTG = 14,
    SH     = 15,
    CH     = 16,
    TH     = 17,
    CTH    = 18,
    EXP    = 19,
};

#define ADD_(   left, right) create_op_node(ADD   , left, right)
#define SUB_(   left, right) create_op_node(SUB   , left, right)
#define MUL_(   left, right) create_op_node(MUL   , left, right)
#define DIV_(   left, right) create_op_node(DIV   , left, right)
#define POW_(   left, right) create_op_node(POW   , left, right)
#define LOG_(   left, right) create_op_node(LOG   , left, right)
#define LN_(    left)        create_op_node(LN    , left, nullptr)
#define SIN_(   left)        create_op_node(SIN   , left, nullptr)
#define COS_(   left)        create_op_node(COS   , left, nullptr)
#define TG_(    left)        create_op_node(TG    , left, nullptr)
#define CTG_(   left)        create_op_node(CTG   , left, nullptr)
#define ARCSIN_(left)        create_op_node(ARCSIN, left, nullptr)
#define ARCCOS_(left)        create_op_node(ARCCOS, left, nullptr)
#define ARCTG_( left)        create_op_node(ARCTG , left, nullptr)
#define ARCCTG_(left)        create_op_node(ARCCTG, left, nullptr)
#define SH_(    left)        create_op_node(SH    , left, nullptr)
#define CH_(    left)        create_op_node(CH    , left, nullptr)
#define TH_(    left)        create_op_node(TH    , left, nullptr)
#define CTH_(   left)        create_op_node(CTH   , left, nullptr)
#define EXP_(   left)        create_op_node(EXP   , left, nullptr)

#define C_(expression)       copy_expression(expression)
#define D_(expression, par)  find_derivative(expression, par)

union val_node{
    double num;
    int    var;
    op_t   op;
};

struct node_t{
    type_node type;
    val_node  val;
    node_t   *left;
    node_t   *right;
};

typedef node_t * expression_t;

expression_t get_general(const char* file_name);

void         destroy_expression(expression_t expression);
node_t      *init_node(type_node type, val_node val, node_t *left, node_t *right);
node_t      *create_op_node(op_t operation, node_t *left, node_t *right);
node_t      *create_var_node(int var_id);
node_t      *create_num_node(double value);
expression_t get_tree_from_file(const char *file_name);

void show_dump(expression_t expression, dump_position position);

typedef double (*op_func_t)(double, double); 

struct func_t{
    op_t         op;
    const char  *str_op;
    const char  *char_op;
    op_func_t    op_func;
    int          num_of_par;
};

enum pos_t {
    kLeft  = 0,
    kRight = 1,
};

double add_func   (double par1, double par2);
double sub_func   (double par1, double par2);
double mul_func   (double par1, double par2);
double div_func   (double par1, double par2);
double pow_func   (double par1, double par2);
double log_func   (double par1, double par2);
double ln_func    (double par1, double par2);
double sin_func   (double par1, double par2);
double cos_func   (double par1, double par2);
double TG_func    (double par1, double par2);
double ctg_func   (double par1, double par2);
double arcsin_func(double par1, double par2);
double arccos_func(double par1, double par2);
double arctg_func (double par1, double par2);
double arcctg_func(double par1, double par2);
double sh_func    (double par1, double par2);
double ch_func    (double par1, double par2);
double th_func    (double par1, double par2);
double cth_func   (double par1, double par2);
double exp_func   (double par1, double par2);

static const func_t op_list[] = {
{ADD    , "ADD"   , "+"     , add_func   , 2},
{SUB    , "SUB"   , "-"     , sub_func   , 2},
{MUL    , "MUL"   , "*"     , mul_func   , 2},
{DIV    , "DIV"   , "/"     , div_func   , 2},
{POW    , "POW"   , "^"     , pow_func   , 2},
{LOG    , "LOG"   , "_"     , log_func   , 2},
{LN     , "LN"    , "ln"    , ln_func    , 1},
{SIN    , "SIN"   , "sin"   , sin_func   , 1},
{COS    , "COS"   , "cos"   , cos_func   , 1},
{TG     , "TG"    , "tg"    , TG_func    ,1},
{CTG    , "CTG"   , "ctg"   , ctg_func   , 1},
{ARCSIN , "ARCSIN", "arcsin", arcsin_func, 1},
{ARCCOS , "ARCCOS", "arccos", arccos_func, 1},
{ARCTG  , "ARCTG" , "arctg" , arctg_func , 1},
{ARCCTG , "ARCCTG", "arcctg", arcctg_func, 1},
{SH     , "SH"    , "sh"    , sh_func    , 1},
{CH     , "CH"    , "ch"    , ch_func    , 1},
{TH     , "TH"    , "th"    , th_func    , 1},
{CTH    , "CTH"   , "cth"   , cth_func   , 1},
{EXP    , "EXP"   , "exp"   , exp_func   , 1},
};

expression_t copy_expression(expression_t expression);
expression_t find_derivative(expression_t expression, int par = 'x');

expression_t simplification_of_expression(expression_t expression);

const double precision = 1e-5;
bool doubles_are_equal(double par1, double par2);

#endif