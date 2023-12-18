#include <iostream>
#include <vector>
#include <cstring>
#include <stack>

struct expr {
    int int_value;
    float float_value;
    char char_value;
    char* string_value; 
    const char* scope;
    int is_const;
    char name[256];
    int type; 
    char type_name[10];
    int is_init;
    int is_vec;
    unsigned int array_size = 0;
    struct expr **vector;
};

class Symbol {
public:
    bool const_flag;
    int type_vec;
    expr * expr_ptr;
    Symbol();
    Symbol(expr* ptr);
};

class SymbolTable {
private:
    static const int MAX_SYMBOLS = 1024;
    int count_simb;
    Symbol Symbols [MAX_SYMBOLS];
    std::stack<std::string> scopeStack;
public:
    SymbolTable();
    struct expr* exists(const char* name, const char* type);
    struct expr* search_by_name(const char* name);
    void add_symbol(const char* name, const char* type_name, expr* init_val); 
    void add_array(const char* name, const char* type_name, int new_array_size);
    void update_array_size(int new_size);
    int find_type(const std::string& type_name);
    void get_data();
    void setScope();
    void pushScope(const char* scope);
    void popScope();
    char* computeScope();
    int get_count_simb();
    void table_symbol_display();
};

enum AST_TYPES
{   
    OP,
    IDENTIFIER,
    NUMBER,
    UNKNOWN
};

struct root_data {
    char op;
    struct expr * expr_ptr;
    int number;
    char* unknown;
};

struct node{
	int expr_type;
	struct root_data* root;
	struct node* left;
	struct node* right;
};

class AST 
{
public:
    std::stack<node*>nodes_stack;
    node *buildAST(root_data * root, node * left_tree, node* right_tree, int type);
    int evalAST(node *ast);
    void deallocateAST(node *root);
    void deallocateStack();
    void buildASTRoot(char op);
    int get_size();
};

expr* new_int_expr(int value);

expr* new_char_expr(char value);

expr* new_string_expr(char* value);

expr* concat_string_expr(char* value1, char* value2);

expr* new_float_expr(float value);

expr* new_bool_expr(int value);

void free_expr(expr* expr);

