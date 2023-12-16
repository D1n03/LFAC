#include "program.h"
using namespace std;

expr::expr(const char* n, int t) : int_value(0), float_value(0.0f), char_value('\0'),
                                 bool_value(false), is_const(false),
                                 name(n), type(t), is_init(0), vorm(0), dim1(0) {}

expr::~expr() {
    for (expr* ptr : vector) {
            delete ptr;
    }
}

Symbol::Symbol() : const_flag(0) {}
Symbol::Symbol(expr* ptr) : expr_ptr(ptr) {}

SymbolTable::SymbolTable() : count_simb(0) {}
expr* SymbolTable::exists(const std::string& name, int type) {
    for (int i = 0; i < count_simb; i++) {
        if ((Symbols[i].expr_ptr->name == name) && (Symbols[i].expr_ptr->type == type))
            return Symbols[i].expr_ptr;
    }
    return nullptr;
}

expr* SymbolTable::search_by_name(const std::string& name) {
    for (int i = 0; i < count_simb; i++) {
        if (Symbols[i].expr_ptr->name == name)
            return Symbols[i].expr_ptr;
    }
    return nullptr;
}

void SymbolTable::add_symbol(expr* ptr) {
    if (count_simb < MAX_SYMBOLS) {
        Symbols[count_simb++] = Symbol(ptr);
    } else {
        std::cerr << "Exceeded maximum number of symbols." << std::endl;
    }
}

int main()
{
    SymbolTable symbolTable;

    expr * expr1 = new expr("un_int", 0);
    symbolTable.add_symbol(expr1);
    expr * found_expr1 = symbolTable.exists("un_float", 0);
    expr * found_expr2 = symbolTable.exists("un_int", 0);
    if (found_expr1)
    {
        std::cout << "merge";
    }
    else {
        std::cout << "papa";
    }
     if (found_expr2)
    {
        std::cout << "merge";
    }
    else {
        std::cout << "papa";
    }
    return 0;
}