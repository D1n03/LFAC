#include <iostream>
#include <vector>
#include <cstring>

class expr {
public:
    int int_value;
    float float_value;
    char char_value;
    bool bool_value;
    std::string string_value; 
    std::string scope;
    bool is_const;
    std::string name;
    int type; 
    std::string type_name;
    int is_init;
    int vorm;
    unsigned int dim1;
    std::vector <struct expr*> vector; // array
    expr(const char* n, int t);
    ~expr();
};

class Symbol {
public:
    int const_flag;
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
public:
    SymbolTable();
    expr* exists(const std::string& name, int type);
    expr* search_by_name(const std::string& name);
    void add_symbol(expr* ptr);
};
