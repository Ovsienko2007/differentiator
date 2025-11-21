#ifndef TREE_H_
#define TREE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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
};

#define ADD_   (left, right) create_op_node(ADD   , left, right)
#define SUB_   (left, right) create_op_node(SUB   , left, right)
#define MUL_   (left, right) create_op_node(MUL   , left, right)
#define DIV_   (left, right) create_op_node(DIV   , left, right)
#define POW_   (left, right) create_op_node(POW   , left, right)
#define LOG_   (left, right) create_op_node(LOG   , left, right)
#define LN_    (left)        create_op_node(LN    , left, nullptr)
#define SIN_   (left)        create_op_node(SIN   , left, nullptr)
#define COS_   (left)        create_op_node(COS   , left, nullptr)
#define TG_    (left)        create_op_node(TG    , left, nullptr)
#define CTG_   (left)        create_op_node(CTG   , left, nullptr)
#define ARCSIN_(left)        create_op_node(ARCSIN, left, nullptr)
#define ARCCOS_(left)        create_op_node(ARCCOS, left, nullptr)
#define ARCTG_ (left)        create_op_node(ARCTG , left, nullptr)
#define ARCCTG_(left)        create_op_node(ARCCTG, left, nullptr)
#define SH_    (left)        create_op_node(SH    , left, nullptr)
#define CH_    (left)        create_op_node(CH    , left, nullptr)
#define TH_    (left)        create_op_node(TH    , left, nullptr)
#define CTH_   (left)        create_op_node(CTH   , left, nullptr) 


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


void    destroy_expression(expression_t expression);
node_t *init_node(type_node type, val_node val, node_t *left, node_t *right);

node_t *create_op_node(op_t operation, node_t *left, node_t *right);
node_t *create_var_node(int var_id);
node_t *create_num_node(double value);
expression_t get_tree_from_file(const char *file_name);

void show_dump(expression_t expression, dump_position position);

struct func_t{
    op_t         op;
    const char * str_op;
    const char * char_op;
    int          num_of_par;
};

enum pos_t {
    left  = 0,
    right = 1,
};

static const func_t op_list[] = {
{ADD    , "ADD"   , "+"    , 2},
{SUB    , "SUB"   , "-"    , 2},
{MUL    , "MUL"   , "*"    , 2},
{DIV    , "DIV"   , "/"    , 2},
{POW    , "POW"   , "^"    , 2},
{LOG    , "LOG"   , "_"    , 2},
{LN     , "LN"    , "ln"   , 1},
{SIN    , "SIN"   ,"sin"   , 1},
{COS    , "COS"   ,"cos"   , 1},
{TG     , "TG "   ,"tg"    , 1},
{CTG   , "CTG"   ,"ctg"   , 1},
{ARCSIN, "ARCSIN","arcsin", 1},
{ARCCOS, "ARCCOS","arccos", 1},
{ARCTG , "ARCTG" ,"arctg" , 1},
{ARCCTG, "ARCCTG","arcctg", 1},
{SH    , "SH"    ,"sh"    , 1},
{CH    , "CH"    ,"ch"    , 1},
{TH    , "TH"    ,"th"    , 1},
{CTH   , "CTH"   ,"cth"   , 1},
};

#endif