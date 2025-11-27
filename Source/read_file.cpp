#include "tree.h"

struct str_code{
    int   pos;
    int   size;
    char *data;
    bool  error;
};

static str_code get_data(const char *file_name);
static expression_t add_elems_from_data_to_tree(str_code *data, int n = 0);
static void skip_spaces(str_code *data);

static const int nil_len = 3;

static str_code get_data(const char *file_name){
    str_code data = {};

    FILE *stream = fopen(file_name, "r");
    if (!stream) return data;

    fscanf(stream, "%m[^\n]%n", &data.data, &data.size);

    return data;
}

expression_t get_tree_from_file(const char *file_name){
    str_code     data   =  get_data(file_name);
    expression_t new_tree = add_elems_from_data_to_tree(&data);

    return new_tree;
}

static expression_t add_elems_from_data_to_tree(str_code *data, int n){
    if (data->error) return nullptr;

    node_t *node = nullptr;
    char *ans    = nullptr;
    int len      = 0;

    n++;
    
    skip_spaces(data);
    while (data->data[data->pos] == ')'){
        data->pos++;
        skip_spaces(data);
    }

    if (data->data[data->pos] == '('){
        data->pos++;

        if(sscanf(data->data + data->pos, "%ms%n", &ans, &len) == 0) return nullptr;
        data->pos += len;

        node_t *left = add_elems_from_data_to_tree(data, n);
        if (data->error) return nullptr;

        node_t *right = add_elems_from_data_to_tree(data, n);
        if (data->error) return nullptr;

        if (isdigit(ans[0])){
            if (left || right){
                data->error = true;
            }
            return create_num_node(atof(ans));
        }

        for (size_t pos_op_list = 0; pos_op_list < sizeof(op_list) / sizeof(op_list[0]); pos_op_list++){
            if (strcmp(op_list[pos_op_list].str_op, ans) == 0){
                return create_op_node(op_list[pos_op_list].op, left, right);
            }
        }

        if (len == 1){
            return create_var_node(ans[0]);
        }
    }

    skip_spaces(data);
    while (data->data[data->pos] == ')'){
        data->pos++;
        skip_spaces(data);
    }

    if (sscanf(data->data + data->pos, "%ms%n", &ans, &len) == 1){
        ans[nil_len] = '\0';
        if (strcmp(ans, "nil") == 0){
            data->pos += nil_len;
            return nullptr;
        }
    }

    data->error = true;
    free(node);
    return nullptr;
}

static void skip_spaces(str_code *data){
    if (data->data[data->pos] == ' '){
        data->pos++;
    }
}

static expression_t get_number(str_code *data);
static expression_t get_primary_expression(str_code *data);
static expression_t get_expression(str_code *data);
static expression_t get_term(str_code *data);
static expression_t get_var(str_code *data);
static expression_t get_one_param_func(str_code *data);
static void skip_spaces2(str_code *data);

expression_t get_general(const char* file_name){
    str_code     data   =  get_data(file_name);
    expression_t new_tree = get_expression(&data);

    skip_spaces2(&data);

    if (*data.data != '$'){
        fprintf(stderr, "error");
        data.error = true;
    }
    data.data++;

    if (data.error){
        return nullptr;
    }

    return new_tree;
}

static expression_t get_term(str_code *data){
    fprintf(stderr, "get_t\n");

    node_t *par1 = get_primary_expression(data);

    skip_spaces2(data);
    while (*data->data == '*' || *data->data == '/'){
        bool is_div = (*data->data == '*');
        data->data++;

        node_t *par2 = get_primary_expression(data);

        if (is_div){
            par1 = MUL_(par1, par2);
        }
        else{
            par1 = DIV_(par1, par2);
        }
    }

    return par1;
}

static expression_t get_expression(str_code *data){
    fprintf(stderr, "get_e\n");
    node_t *par1 = get_term(data);

    skip_spaces2(data);
    while (*data->data == '+' || *data->data == '-' || *data->data == '^'){
        char is_sum = *data->data;
        data->data++;

        node_t *par2 = get_term(data);

        if (is_sum == '+'){
            par1 = ADD_(par1, par2);
        }
        else if (is_sum == '-'){
            par1 = SUB_(par1, par2);
        }
        else{
            par1 = POW_(par1, par2);
        }
    }

    return par1;
}

static expression_t get_primary_expression(str_code *data){
    fprintf(stderr, "get_p\n");

    node_t *ans = nullptr;

    skip_spaces2(data);
    if (*data->data == '('){
        data->data++;

        ans = get_expression(data);

        skip_spaces2(data);
        if (*data->data != ')'){
            data->error = true;
            data->data++;
        }
        data->data++;

        return ans;
    }
    else {
        return get_one_param_func(data);
    }
}

static expression_t get_number(str_code *data){
    fprintf(stderr, "get_n\n");

    node_t *par = nullptr;

    skip_spaces2(data);
    if ('a' <= *data->data && *data->data <= 'z'){
        par = get_var(data);
    }
    if ('0' <= *data->data && *data->data <= '9'){
        double x = 0;
        bool   is_n = false;
        double after_dot = 1;

        while (('0' <= *data->data && *data->data <= '9') || 
                *data->data == '.' || *data->data == ','){
            is_n = true;

            if ((*data->data == '.' || *data->data == ',') && 
                 !doubles_are_equal(after_dot, 1)){
                
                data->error = true;
                break;
            }

            if (*data->data == '.' || *data->data == ','){
                after_dot /= 10;
            } 
            else if (!doubles_are_equal(after_dot, 1)){
                x += (*data->data - '0') * after_dot;
                after_dot /= 10;
            }
            else {
                x = x * 10 + (*data->data - '0');
            }

            data->data++;
        }

        if (!is_n){
            data->error = true;
        }

        par = create_num_node(x);
    } 

    if (!par) data->error = true;

    return par;
}

static expression_t get_var(str_code *data){
    node_t *new_node = nullptr;

    if ('a' <= *data->data && *data->data <= 'z'){
        char var = *data->data;
        data->data++;

        return create_var_node(var);
    }

    new_node = get_number(data);

    if (!new_node){
        data->error = true;
    }

    return new_node;
}

static expression_t get_one_param_func(str_code *data){
    node_t *new_node = nullptr;

    for (size_t pos_op_list = 0; pos_op_list < sizeof(op_list) / sizeof(op_list[0]); pos_op_list++){
        if (op_list[pos_op_list].num_of_par == 1) {
            if (strncmp(data->data, op_list[pos_op_list].char_op, 
                             strlen(op_list[pos_op_list].char_op)) == 0){
                data->data += strlen(op_list[pos_op_list].char_op);

                return create_op_node(op_list[pos_op_list].op, get_expression(data), nullptr);
            }
        }
    }
    if (strncmp(data->data, "log_{", strlen("log_{")) == 0){
        data->data += strlen("log_{");

        node_t *par1 = get_expression(data);

        if (*data->data != '}'){
            data->error = true;
        }
        data->data++;

        if (*data->data != '{'){
            data->error = true;
        }
        data->data++;

        node_t *par2 = get_expression(data);

        if (*data->data != '}'){
            data->error = true;
        }
        data->data++;

        return create_op_node(LOG, par1, par2);
    }

    new_node = get_var(data);

    if (!new_node){
        data->error = true;
    }

    return new_node;
}

static void skip_spaces2(str_code *data){
    while (*data->data == ' '){
        data->data++;
    }
}