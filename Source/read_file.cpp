#include "tree.h"


struct str_code{
    int   pos;
    int   size;
    char *data;
    bool  error;
};

static str_code get_data(const char *file_name);
static expression_t add_elems_from_data_to_tree(str_code *data);
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
    str_code data   =  get_data(file_name);
    expression_t new_tree = add_elems_from_data_to_tree(&data);

    return new_tree;
}

static expression_t add_elems_from_data_to_tree(str_code *data){
    if (data->error) return nullptr;

    node_t *node = nullptr;
    char *ans    = nullptr;
    int len      = 0;
    
    skip_spaces(data);
    while (data->data[data->pos] == ')'){
        data->pos++;
        skip_spaces(data);
    }

    if (data->data[data->pos] == '('){
        data->pos++;

        if(sscanf(data->data + data->pos, "%ms%n", &ans, &len) == 0) return nullptr;
        data->pos += len;

        node_t *left = add_elems_from_data_to_tree(data);
        if (data->error) return nullptr;

        node_t *right = add_elems_from_data_to_tree(data);
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