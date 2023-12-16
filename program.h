#include <iostream>
#include <vector>
#include <cstring>
#include <stack>

struct expr {
    int int_value;
    float float_value;
    char char_value;
    int bool_value;
    char* string_value; 
    const char* scope;
    int is_const;
    char name[256];
    int type; 
    char type_name[10];
    int is_init;
    int is_vec;
    unsigned int dim1;
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
    int find_type(const std::string& type_name);
    void get_data();
    void setScope();
    void pushScope(const char* scope);
    void popScope();
    char* computeScope();
};
