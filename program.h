#include <iostream>
#include <vector>
#include <array>
#include <cstring>
#include <stack>

const int NMAX = 1024;

int find_type(const std::string &type_name);
char *computeScope();
void pushScope(const char *scope);
void popScope();

struct expr
{
    int int_value;
    float float_value;
    char char_value;
    char *string_value;
    const char *scope;
    int is_const;
    char name[256];
    int type;
    char type_name[10];
    int is_init;
    int is_vec;
    int is_class;
    int is_matrix;
    unsigned int array_size = 0;
    unsigned int array_size_2 = 0;
    struct expr **vector;
    struct expr ***matrix;
};

class Symbol
{
public:
    bool const_flag;
    int type_vec;
    expr *expr_ptr;
    Symbol();
    Symbol(expr *ptr);
};

class SymbolTable
{
private:
    int count_simb;
    Symbol Symbols[NMAX];

public:
    SymbolTable();
    struct expr *exists(const char *name, const char *type);
    struct expr *search_by_name(const char *name);
    void add_symbol(const char *name, const char *type_name, expr *init_val);
    void add_array(const char *name, const char *type_name, int new_array_size);
    void add_matrix(const char *name, const char *type_name, int size1, int size2);
    void update_array_size(int new_size1, int new_size2 = 0);
    void add_class(const char *name);
    void get_data();
    void setScope();
    int get_count_simb();
    void table_symbol_display();
    void dellocEverything();
};

class Function
{
public:
    char name[256];
    int count_params;
    int type;
    char return_type[10];
    int int_value;
    float float_value;
    char char_value;
    char *string_value;
    expr *expr_ptr;
    expr **array_params;
};

class FunctionTable
{
private:
    Function Functions[NMAX];
    int cnt_fn;

public:
    FunctionTable();
    expr *params[16];
    expr *call_function_list[16];
    int call_cnt;
    int params_cnt;
    struct expr **exists_fn(const char *name, int cnt_param);
    struct expr *exists_fn(const char *name, const char *type);
    struct expr *get_expr_fn(const char *name);
    int empty_fn(const char *name);
    void create_fn(const char *name, char type[], int is_empty);
    void table_function_display();
};

enum AST_TYPES
{
    OP,
    IDENTIFIER_INT,
    IDENTIFIER_BOOL,
    IDENTIFIER_FLOAT,
    NUMBER_INT,
    NUMBER_BOOL,
    NUMBER_FLOAT,
    UNKNOWN
};

enum OPS
{
    ADD,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MOD,
    LESSTHAN,
    LESSEQTHAN,
    GREATERTHAN,
    GREATEREQTHAN,
    EQUAL,
    NOTEQUAL,
    NOT,
};

struct root_data
{
    int op;
    struct expr *expr_ptr;
    int number_int;
    int number_bool;
    float number_float;
    char *unknown;
};

struct node
{
    int expr_type;
    int type_node;
    struct root_data *root;
    struct node *left;
    struct node *right;
};

class AST
{
public:
    std::array<node *, NMAX> nodes_stack = {NULL};
    int nodes_stack_cnt = 0;
    node *buildAST(root_data *root, node *left_tree, node *right_tree, int type);
    int evalAST(node *ast);
    float evalAST_f(node *ast);
    int evalAST_b(node *ast);
    void deallocateAST(node *root);
    void deallocateStack();
    void buildASTRoot(int op);
    int get_size();
    int evaluate(node *left, node *right, int type);
    int evaluate_f(node *left, node *right, int type);
    int evaluate_b(node *left, node *right, int type);
};

expr *new_int_expr(int value);

expr *new_char_expr(char value);

expr *new_string_expr(char *value);

expr *concat_string_expr(char *value1, char *value2);

expr *new_float_expr(float value);

expr *new_bool_expr(int value);

void free_expr(expr *expr);