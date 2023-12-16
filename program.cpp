#include "program.h"
using namespace std;

Symbol::Symbol() : const_flag(0) {}
Symbol::Symbol(expr* ptr) : expr_ptr(ptr) {}

SymbolTable::SymbolTable() : count_simb(0) {}
expr* SymbolTable::exists(const char* name, const char* type) {
    for (int i = 0; i < count_simb; i++) {
        if (!strcmp(Symbols[i].expr_ptr->name,name) && !strcmp(Symbols[i].expr_ptr->type_name,type))
            return Symbols[i].expr_ptr;
    }
    return nullptr;
}

expr* SymbolTable::search_by_name(const char* name) {
    string strname = string(name);
    for (int i = 0; i < count_simb; i++) {
        if (Symbols[i].expr_ptr->name == strname)
            return Symbols[i].expr_ptr;
    }
    return nullptr;
}

int SymbolTable::find_type(const std::string& type_name)
{
    if (type_name == "integer")
        return 1;
    if (type_name == "char")
        return 2;
    if (type_name == "string")
        return 3;
    if (type_name == "float")
        return 4;
    if (type_name == "boolean")
        return 5;
    return -1;
}

void SymbolTable::add_symbol(const char* name, const char * type_name, struct expr* init_val) 
{
    string type_name_str = string(type_name);
    if (count_simb < MAX_SYMBOLS) {
        if (init_val == nullptr) {
            Symbols[count_simb].expr_ptr = (expr*)calloc(1, sizeof(expr));
            Symbols[count_simb].expr_ptr->is_init = 0;
            Symbols[count_simb].expr_ptr->type = find_type(type_name);
            strcpy(Symbols[count_simb].expr_ptr->name, name);
            strcpy(Symbols[count_simb].expr_ptr->type_name,type_name);
            Symbols[count_simb].const_flag = false;
            Symbols[count_simb].expr_ptr->is_const = 0;
            Symbols[count_simb].expr_ptr->is_vec = 0;
            Symbols[count_simb].type_vec = -1;
        } else {
            Symbols[count_simb].expr_ptr = init_val;
            Symbols[count_simb].const_flag = true;
            Symbols[count_simb].expr_ptr->is_init = 1;
            strcpy(Symbols[count_simb].expr_ptr->name, name);
            Symbols[count_simb].expr_ptr->is_const = 1;
        }
        setScope();
        count_simb++;
    } else std::cerr << "Exceeded maximum number of symbols." << std::endl;
}

void SymbolTable::pushScope(const char* scope) 
{
    scopeStack.push(scope);
}

void SymbolTable::popScope() 
{
    if (!scopeStack.empty())
        scopeStack.pop();
}

char* SymbolTable::computeScope()
{
    int length = 0;
    // copy the stack to a temporary vector
    std::vector<std::string> tempVector;
    while (!scopeStack.empty()) {
        tempVector.push_back(scopeStack.top());
        scopeStack.pop();
    }

    // calculate the required length for the composite scope string
    for (const auto& s : tempVector) 
        length += s.length() + 1; // Add 1 for '>'

    // restore the stack from the temporary vector
    for (auto it = tempVector.rbegin(); it != tempVector.rend(); ++it) 
        scopeStack.push(*it);

    // create a char array to store the composite scope string
    std::vector<char> to_return(length, 0);

    // concatenate all scopes with '>'
    for (const auto& s : tempVector) {
        strcat(to_return.data(), s.c_str());
        strcat(to_return.data(), ">");
    }

    return to_return.data();
}

void SymbolTable::setScope() {
    if (count_simb < MAX_SYMBOLS) {
        if (scopeStack.empty()) {
            Symbols[count_simb].expr_ptr->scope = "global"; // no scopes on the stack, so it's in the global scope
        } else {
            Symbols[count_simb].expr_ptr->scope = computeScope(); // compute the composite scope string
        }
    } else {
        std::cerr << "Exceeded maximum number of symbols." << std::endl;
    }
}

void SymbolTable::get_data() //print data to check if it's ok
{
    for (int i = 0; i < count_simb; i++)
    {
        std::cout << Symbols[i].expr_ptr->type_name << " " << Symbols[i].expr_ptr->name  << Symbols[i].expr_ptr->scope << "\n";
    }
}