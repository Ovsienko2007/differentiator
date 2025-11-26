#include "tree.h"

#define COL_NEXT_BACKGROUND    "#7df879ff"
#define COL_LEFT_BACKGROUND    "#f879e7ff"
#define COL_CURRENT_BACKGROUND "#d3cd77ff"

#define COL_OP_BACKGROUND      "#26cfdb9f"
#define COL_VAR_BACKGROUND     "#d6ba7fbe"
#define COL_NUM_BACKGROUND     "#ac7fd6be"

#define COL_VALUE_BACKGROUND   "#7f8cd6be"

#define COL_RIGHT_ARROW        "#0ea30eff"
#define COL_LEFT_ARROW         "#fa2821ff"

static FILE *html_stream = nullptr;

static const char *dump_file_position  = "DUMP/dump.tex";
static const int kMaxFileNameLen       = 24;

static void print_tree(expression_t expression, int n = 0, bool is_in_frac = true);

static void creat_html(int num_call, dump_position position, expression_t expression);
static void creat_dot(int num_call, expression_t tree);
static void print_dump_elem(FILE *stream_out, node_t *elem);

static void start_dump();
static void end_dump();

void show_dump(expression_t expression, dump_position position){
    static int num_call = 1;

    if (num_call == 1){
        start_dump();
        atexit(end_dump);
    }

    creat_dot(num_call, expression);
    creat_html(num_call, position, expression);
    printf("\n____________\n");
    num_call++;
}

static void creat_html(int num_call, dump_position position, expression_t expression){
    if (!html_stream) return;

    fprintf(html_stream, "\\section*{Dump called from %s:%d from func %s. The %d call}\n",
         position.file, position.line, position.func, num_call);

    fprintf(html_stream,  "\\begin{figure}[!h]"
                                         "\\begin{center}\n"
                                         "    \\includegraphics[scale=0.5]{%d.png}\n"
                                         "    \\begin{center}\n"
                                         "    \\end{center}\n"
                                         "    \\label{graphic1b}\n"
                                         "\\end{center}\n"
                                         "\\end{figure}\n", num_call);
    print_tree(expression);
}

static void creat_dot(int num_call, expression_t expression){
    char file_name[kMaxFileNameLen] = {};
    sprintf(file_name, "DUMP/%d.dot", num_call);

    FILE *stream_out = fopen(file_name ,"w");
    if (!stream_out) return;

    fprintf(stream_out, "digraph structs {\n");
    if (expression){
        print_dump_elem(stream_out, expression);
    }
    fprintf(stream_out, "}\n");
    fclose(stream_out);
}

static void print_dump_elem(FILE *stream_out, node_t *elem){
    if (!elem) return;

    fprintf(stream_out, 
        "%lu [shape=\"plaintext\", label=<\n"
        "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n", (unsigned long)elem);

    fprintf(stream_out, "<TR><TD BGCOLOR=\"" COL_CURRENT_BACKGROUND "\" COLSPAN=\"2\">%p</TD></TR>\n", elem);

    switch(elem->type){
        case OP:
            fprintf(stream_out, "<TR><TD BGCOLOR=\"" COL_OP_BACKGROUND "\" COLSPAN=\"2\">%s</TD></TR>\n", "OP");

            for (size_t pos_op_list = 0; pos_op_list < sizeof(op_list) / sizeof(op_list[0]); pos_op_list++){
                if (op_list[pos_op_list].op == elem->val.op){
                    fprintf(stream_out, "<TR><TD BGCOLOR=\"" COL_VALUE_BACKGROUND "\" COLSPAN=\"2\">%s</TD></TR>\n", op_list[pos_op_list].str_op);
                }
            }
            break;

        case VAR:
            fprintf(stream_out, "<TR><TD BGCOLOR=\"" COL_VAR_BACKGROUND   "\" COLSPAN=\"2\">%s</TD></TR>\n", "VAR");
            fprintf(stream_out, "<TR><TD BGCOLOR=\"" COL_VALUE_BACKGROUND "\" COLSPAN=\"2\">%c</TD></TR>\n", elem->val.var + 'a');
            break;

        case NUM:
            fprintf(stream_out, "<TR><TD BGCOLOR=\"" COL_NUM_BACKGROUND   "\" COLSPAN=\"2\">%s</TD></TR>\n", "NUM");
            fprintf(stream_out, "<TR><TD BGCOLOR=\"" COL_VALUE_BACKGROUND "\" COLSPAN=\"2\">%lf</TD></TR>\n", elem->val.num);
            break;

        default:
            break;
    }
        
    fprintf(stream_out,
        "    <TR>\n"
        "        <TD BGCOLOR=\"" COL_LEFT_BACKGROUND    "\"> %p </TD>\n"
        "        <TD BGCOLOR=\"" COL_NEXT_BACKGROUND    "\"> %p </TD>\n"
        "    </TR>\n"
        "</TABLE>\n"
        ">];\n", elem->left, elem->right);

    if (elem->right){
        fprintf(stream_out, "%lu->%lu[color=\"" COL_RIGHT_ARROW "\"];\n", (unsigned long)elem, (unsigned long)elem->right);
        print_dump_elem(stream_out, elem->right);
    }

    if (elem->left){
        fprintf(stream_out, "%lu->%lu[color=\"" COL_LEFT_ARROW "\"];\n", (unsigned long)elem, (unsigned long)elem->left);
        print_dump_elem(stream_out, elem->left);
    }
}

static void start_dump(){
    html_stream = fopen(dump_file_position,"w");
    fprintf(html_stream, "\\documentclass[a4paper,10pt]{article}\n"
                                        "\\usepackage[T2A]{fontenc}\n"
                                        "\\usepackage{graphicx}\n"
                                        "\\usepackage{float}\n"
                                        "\\begin{document}\n");
}   

static void end_dump(){
    if (!html_stream) return;

    fprintf(html_stream,  "\\end{document}");
                        
    fclose(html_stream);
}

static void print_value(expression_t expression, bool *is_in_frac);
static void print_right_bracket(expression_t expression, bool *is_in_frac);
static void print_left_bracket(expression_t expression, bool *is_in_frac);
static bool is_special_op(expression_t expression, int n);
static bool is_need_brackets(expression_t expression, pos_t pos);
static bool is_second_par_need(expression_t expression);

static void print_tree(expression_t expression, int n, bool need_brackets) {
    if ((++n) == 1) {
        fprintf(html_stream, "\\[");
    }

    if (need_brackets && expression->type == OP && is_special_op(expression, n)) {
        printf("%d ", n);
        fprintf(html_stream, "\\left(");
    }

    print_left_bracket(expression, &need_brackets);

    if (expression->left) {
        bool left_need_brackets = false;
        
        if (expression->type == OP) {
            left_need_brackets = is_need_brackets(expression, kLeft) && expression->val.op != LOG;
        }
        
        print_tree(expression->left, n, left_need_brackets);
    }

    print_value(expression, &need_brackets);

    if (expression->right && is_second_par_need(expression)){
        bool right_need_brackets = false;
        
        if (expression->type == OP) {
            right_need_brackets = is_need_brackets(expression, kRight);
        }

        print_tree(expression->right, n, right_need_brackets);
    }

    print_right_bracket(expression, &need_brackets);

    if (need_brackets && expression->type == OP && is_special_op(expression, n)){
        fprintf(html_stream, "\\right)");
    }

    if (n == 1) {
        fprintf(html_stream, "\\]\n");
    }
}

static void print_value(expression_t expression, bool *need_brackets) {
    if (expression->type == OP) {
        if (expression->val.op == DIV) {
            fprintf(html_stream, "}{");
        }
        else if (expression->val.op == LOG) {
            fprintf(html_stream, "}\\left(");
            *need_brackets = false;
        }
        else if (expression->val.op == POW) {
            fprintf(html_stream, "^{");
        }
        else {
            for (size_t pos_op_list = 0; pos_op_list < sizeof(op_list) / sizeof(op_list[0]); pos_op_list++) {
                if (op_list[pos_op_list].op == expression->val.op && op_list[pos_op_list].num_of_par == 2) {
                    fprintf(html_stream, "%s", op_list[pos_op_list].char_op);
                    break;
                }
            }
        }
    }
    else if (expression->type == VAR) {
        fprintf(html_stream, "%c", expression->val.var + 'a');
    }
    else {
        fprintf(html_stream, "%lg", expression->val.num);
    }
}

static void print_left_bracket(expression_t expression, bool *need_brackets) {
    if (expression->type == OP) {
        if (expression->val.op == DIV) {
            fprintf(html_stream, "\\frac{");
            *need_brackets = false;
        }
        else if (expression->val.op == LOG) {
            fprintf(html_stream, "\\log_{");
            *need_brackets = false;
        }
        else {
            for (size_t pos_op_list = 0; pos_op_list < sizeof(op_list) / sizeof(op_list[0]); pos_op_list++) {
                if (op_list[pos_op_list].op == expression->val.op && op_list[pos_op_list].num_of_par == 1) {
                    fprintf(html_stream, "%s", op_list[pos_op_list].char_op);
                    fprintf(html_stream, "\\left(");
                    break;
                }
            }
        }
    }
}

static void print_right_bracket(expression_t expression, bool *need_brackets) {
    if (expression->type == OP) {
        if (expression->val.op == DIV) {
            fprintf(html_stream, "}");
        }
        else if (expression->val.op == LOG) {
            fprintf(html_stream, "\\right)");

            *need_brackets = false;
        }
        else if (expression->val.op == POW) {
            fprintf(html_stream, "}");
        }
        else{
            for (size_t pos_op_list = 0; pos_op_list < sizeof(op_list) / sizeof(op_list[0]); pos_op_list++) {
                if (op_list[pos_op_list].op == expression->val.op && op_list[pos_op_list].num_of_par == 1) {
                    fprintf(html_stream, "\\right)");

                    *need_brackets = false;
                    break;
                }
            }
        }
    }
}

static bool is_special_op(expression_t expression, int n){
    return  expression->type == OP && 
           (expression->val.op == ADD || 
            expression->val.op == SUB) && n != 1;
}

static bool is_need_brackets(expression_t expression, pos_t pos){
    switch (expression->val.op) {
        case POW: 
            return expression->left->type == OP && pos == kLeft;
        case SIN   : case COS   : case TG   : case CTG   :
        case ARCSIN: case ARCCOS: case ARCTG: case ARCCTG:
        case SH    : case CH    : case TH   : case CTH   :
        case EXP   : case LN    : case DIV  : case LOG   :
        
            return false;
        case ADD   : case SUB   : case MUL   :
        default:
            return true;
    }
}

static bool is_second_par_need(expression_t expression){
    if (expression->type != OP) return true;

    for (size_t pos_op_list = 0; pos_op_list < sizeof(op_list) / sizeof(op_list[0]); pos_op_list++) {
        if (op_list[pos_op_list].op == expression->val.op ){
            return op_list[pos_op_list].num_of_par == 2;
        }
    }

    return true;
}