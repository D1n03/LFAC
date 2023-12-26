#include "program.h"
#include <fstream>
#include <iomanip>

std::ofstream file_table;

Symbol::Symbol() : const_flag(false) {}
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
    std::string strname = std::string(name);
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
    std::string type_name_str = std::string(type_name);
    if (count_simb < NMAX) {
        if (init_val == nullptr) {
            Symbols[count_simb].expr_ptr = new struct expr;
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

void SymbolTable::setScope() 
{
    if (count_simb < NMAX) 
    {
        if (scopeStack.empty()) {
            Symbols[count_simb].expr_ptr->scope = "global"; // no scopes on the stack, so it's in the global scope
        } else {
            Symbols[count_simb].expr_ptr->scope = computeScope(); // compute the composite scope string
        }
    } else {
        std::cerr << "Exceeded maximum number of symbols." << "\n";
    }
}

int SymbolTable::get_count_simb()
{
    return count_simb;
}

void SymbolTable::get_data() //print data to check if it's ok
{
    for (int i = 0; i < count_simb; i++)
    {
        std::cout << Symbols[i].expr_ptr->type_name << " " << Symbols[i].expr_ptr->name  << Symbols[i].expr_ptr->scope << " " << Symbols[i].expr_ptr->type << "\n";
    }
}

struct node *AST::buildAST(root_data * root, node * left_tree, node* right_tree, int type)
{
    struct node* new_node = new struct node;
    new_node->root = root;
    if  (left_tree != nullptr)
    {
        if (left_tree->expr_type == NUMBER_INT || left_tree->expr_type == IDENTIFIER_INT)
            new_node->type_node = 0;
        else if (left_tree->expr_type == NUMBER_FLOAT || left_tree->expr_type == IDENTIFIER_FLOAT) 
            new_node->type_node = 1;
    }
    if  (right_tree != nullptr)
    {
        if (right_tree->expr_type == NUMBER_INT || right_tree->expr_type == IDENTIFIER_INT)
            new_node->type_node = 0;
        else if (right_tree->expr_type == NUMBER_FLOAT || right_tree->expr_type == IDENTIFIER_FLOAT) 
            new_node->type_node = 1;
    }
    new_node->expr_type = type;
    new_node->left = left_tree;
    new_node->right = right_tree;
    return new_node;
}

int AST::evalAST(node *ast)
{
    if (ast->expr_type == NUMBER_INT)
        return ast->root->number_int;
  
    if (ast->expr_type == IDENTIFIER_INT)
        return ast->root->expr_ptr->int_value;


    if (ast->expr_type == UNKNOWN)
        return 0;

    if (ast->expr_type == OP)
    {
        if (ast->root->op == ADD)
            return evalAST(ast->left) + evalAST(ast->right);
        else if (ast->root->op == MINUS)
            return evalAST(ast->left) - evalAST(ast->right);
        else if (ast->root->op == MULTIPLY)
            return evalAST(ast->left) * evalAST(ast->right);
        else if (ast->root->op == DIVIDE)
            return evalAST(ast->left) / evalAST(ast->right);
        else if (ast->root->op == MOD)
            return evalAST(ast->left) % evalAST(ast->right);
        else return evalAST_b(ast);
    }
    return 0;
}

float AST::evalAST_f(node *ast)
{
    if (ast->expr_type == NUMBER_FLOAT)
        return ast->root->number_float;

    if (ast->expr_type == IDENTIFIER_FLOAT)
        return ast->root->expr_ptr->float_value;

    if (ast->expr_type == UNKNOWN)
        return 0;

    if (ast->expr_type == OP)
    {
        if (ast->root->op == ADD)
            return evalAST_f(ast->left) + evalAST_f(ast->right);
        else if (ast->root->op == MINUS)
            return evalAST_f(ast->left) - evalAST_f(ast->right);
        else if (ast->root->op == MULTIPLY)
            return evalAST_f(ast->left) * evalAST_f(ast->right);
        else if (ast->root->op == DIVIDE)
            return evalAST_f(ast->left) / evalAST_f(ast->right);
        else return evalAST_b(ast);
    }
    return 0;
}

int  AST::evalAST_b(node *ast) 
{
    if (ast->expr_type == NUMBER_BOOL)
        return ast->root->number_int;
    
    if (ast->expr_type == IDENTIFIER_BOOL)
        return ast->root->expr_ptr->int_value;

    if (ast->expr_type == OP)
    {
        if (ast->type_node == 0)
            return evaluate(ast->left, ast->right, ast->root->op);
        else if (ast->type_node == 1)
            return evaluate_f(ast->left, ast->right, ast->root->op);
        // if (((ast->left->expr_type == NUMBER_INT) || (ast->left->expr_type == IDENTIFIER_INT)) && ((ast->right->expr_type == NUMBER_INT) || (ast->right->expr_type == IDENTIFIER_INT)))
        // {
        // }
    }
    return 0;
}

int AST::evaluate(node *left, node *right, int type)
{
    if (type == LESSTHAN)
    {
        return (evalAST(left) < evalAST(right));
    }
    if (type == LESSEQTHAN)
    {
        return (evalAST(left) <= evalAST(right));
    }
    if (type == GREATERTHAN)
    {
        return (evalAST(left) > evalAST(right));
    }
    if (type== GREATEREQTHAN)
    {
        return (evalAST(left) >= evalAST(right));
    }
    if (type == EQUAL)
    {
        return (evalAST(left) == evalAST(right));
    } 
    if (type == NOTEQUAL)
    {
        return (evalAST(left) != evalAST(right));
    } 
    if (type == NOT)
    {
        if (right->expr_type == OP)
            return !evalAST_b(right);
        else return !evalAST(right);
    } 
    return 0;
}

int AST::evaluate_f(node *left, node *right, int type)
{
    if (type == LESSTHAN)
    {
        return (evalAST_f(left) < evalAST_f(right));
    }
    if (type == LESSEQTHAN)
    {
        return (evalAST_f(left) <= evalAST_f(right));
    }
    if (type == GREATERTHAN)
    {
        return (evalAST_f(left) > evalAST_f(right));
    }
    if (type== GREATEREQTHAN)
    {
        return (evalAST_f(left) >= evalAST_f(right));
    }
    if (type == EQUAL)
    {
        return (evalAST_f(left) == evalAST_f(right));
    } 
    if (type == NOTEQUAL)
    {
        return (evalAST_f(left) != evalAST_f(right));
    } 
    if (type == NOT)
    {
        if (right->expr_type == OP)
            return !evalAST_b(right);
        return !evalAST_f(right);
    }
    return 0;
}

int AST::evaluate_b(node *left, node *right, int type)
{
    if (type == EQUAL)
    {
        return (evalAST_b(left) == evalAST_b(right));
    } 
    if (type == NOTEQUAL)
    {
        return (evalAST_b(left) != evalAST_b(right));
    } 
    if (type == NOT)
    {
        if (right->expr_type == OP)
            return !evalAST_b(right);
        return !evalAST_b(right);
    }
    return 0;
}

void AST::deallocateAST(node *root_data)
{
    if (root_data != NULL)
    {
        if (root_data->root->unknown != NULL)
        {
            free(root_data->root->unknown);
        }
        if (root_data->root->unknown != NULL)
        {
            delete root_data->root->expr_ptr;
        }
        delete root_data->root;
    	deallocateAST(root_data->left);
    	delete root_data->left;
    	deallocateAST(root_data->right);
    	delete root_data->right;
    }
}

void AST::deallocateStack()
{
    while (nodes_stack_cnt > 0)
    {
        deallocateAST(nodes_stack[--nodes_stack_cnt]);
        nodes_stack[nodes_stack_cnt] = NULL;
    }
}

void AST::buildASTRoot(int op)
{
    if (nodes_stack_cnt < NMAX)
    {
        root_data *data = new struct root_data;
        data->op = op;

        nodes_stack_cnt--;
        node *right = nodes_stack[nodes_stack_cnt];
        node *left = nullptr;
        if (op != NOT)
        {
            nodes_stack_cnt--;
            left = nodes_stack[nodes_stack_cnt];
        }
        struct node *root_new = buildAST(data, left, right, OP);
        nodes_stack[nodes_stack_cnt++] = root_new;
    }
    else std::cerr << "Exceeded maximum number of nodes." << "\n";
}

expr* new_int_expr(int value)
{
	struct expr* new_expr = new struct expr;
	new_expr->int_value = value;
	new_expr->type = 1;
	strcpy(new_expr->type_name, "integer");
	return new_expr;
}

expr* new_char_expr(char value)
{
	struct expr* new_expr = new struct expr;
	new_expr->char_value = value;
	new_expr->type = 2;
	strcpy(new_expr->type_name, "char");
	return new_expr;
}

expr* new_string_expr(char* value)
{
	struct expr* new_expr = new struct expr;
	new_expr->string_value = value;
	new_expr->type = 3;
	strcpy(new_expr->type_name, "string");
	return new_expr;
}

expr* concat_string_expr(char* value1, char* value2)
{
    struct expr* new_expr = new struct expr;
    int len2 = value2 ? strlen(value2) : 0;
    new_expr->string_value = (char*) malloc(sizeof(char)*(strlen(value1) + len2 +1));
    strcpy(new_expr->string_value, value1);
    if(value2)
        strcat(new_expr->string_value, value2);
    new_expr->type = 3;
    return new_expr;
}

expr* new_float_expr(float value)
{    
	struct expr* new_expr = new struct expr;
	new_expr->float_value = value;
	new_expr->type = 4;
	strcpy(new_expr->type_name, "float");
	return new_expr;
}

expr* new_bool_expr(int value)
{
	struct expr* new_expr = new struct expr;
	new_expr->int_value = value;
	new_expr->type = 5;
	strcpy(new_expr->type_name, "boolean");
	return new_expr;
}

void free_expr(expr* expr)
{
	if(expr->string_value != NULL)
    	free(expr->string_value);
	free(expr);
}

void write_expr(struct expr* source) 
{
    if (source->type == 1)
        file_table << source->int_value << " ";
    if (source->type == 2)
        if (source->char_value >= 33 && source->char_value <= 126)
            file_table << source->char_value << " ";
        else file_table << "# ";
    if (source->type == 3)
        file_table << source->string_value << " ";
    if (source->type == 4)
        file_table << std::fixed << std::setprecision(6) << source->float_value << " ";
    if (source->type == 5)
        file_table << (source->int_value ? "true" : "false") << " ";
    if (source->type == 6)
        file_table << "{class}";
}

void SymbolTable::table_symbol_display()
{
    file_table.open("symbol_table.txt");
    int cnt = get_count_simb();
        for (int i = 0; i < cnt; i++) 
        {
            file_table << "Name: " << Symbols[i].expr_ptr->name << "\t\t Type: " << Symbols[i].expr_ptr->type_name;
            if (Symbols[i].expr_ptr->is_vec == 1)
                file_table << "[" << Symbols[i].expr_ptr->array_size << "]";
            file_table << "\tConstant: " << (Symbols[i].const_flag ? "true" : "false") << "\t";
            file_table << "Scope: " << Symbols[i].expr_ptr->scope << "\t";
            if (Symbols[i].expr_ptr->is_init == 1) 
            {
                if (Symbols[i].expr_ptr->is_vec == 1) 
                {
                    file_table << "\n\tValues: ";
                    for (int k = 0; k < Symbols[i].expr_ptr->array_size; k++)
                        write_expr(Symbols[i].expr_ptr->vector[k]);
                    file_table << "\n";
                } 
                else if (Symbols[i].expr_ptr->is_matrix == 1)
                {
                    file_table << "\n\tValues: ";
                    for (int k = 0; k < Symbols[i].expr_ptr->array_size; k++)
                    {
                        for (int l = 0; l < Symbols[i].expr_ptr->array_size_2; l++)
                        {
                            write_expr(Symbols[i].expr_ptr->matrix[k][l]);                           
                        }
                        file_table << "\n\t\t\t";
                    }
                }
                else
                {
                    file_table << "Value: ";
                    write_expr(Symbols[i].expr_ptr);
                }
            } 
            else file_table << "Uninitialized";
            file_table << "\n";
        }
        file_table.close();
}

void SymbolTable::update_array_size(int new_size1, int new_size2)
{
    Symbols[count_simb].expr_ptr->array_size = new_size1;
    Symbols[count_simb].expr_ptr->array_size_2 = new_size2;
}


void SymbolTable::add_array(const char* name, const char* type_name, int new_array_size)
{
    if (count_simb < NMAX) 
    {
        Symbols[count_simb].expr_ptr = new struct expr;

        update_array_size(new_array_size);
        int vsize = Symbols[count_simb].expr_ptr->array_size;

        strcpy(Symbols[count_simb].expr_ptr->name, name);
        Symbols[count_simb].const_flag = false;

        strcpy(Symbols[count_simb].expr_ptr->name, name);
        Symbols[count_simb].expr_ptr->type = find_type(type_name);
        strcpy(Symbols[count_simb].expr_ptr->type_name, type_name);
        Symbols[count_simb].expr_ptr->is_init = 1;


        Symbols[count_simb].expr_ptr->is_vec = 1;
        Symbols[count_simb].type_vec = 1;
    	Symbols[count_simb].expr_ptr->vector = (expr**)calloc(vsize, sizeof(expr*));

        // initialize array of pointers
    	for(int i = 0; i < vsize ; i++){
        	Symbols[count_simb].expr_ptr->vector[i] = new struct expr;
        	Symbols[count_simb].expr_ptr->vector[i]->type = find_type(type_name);
        	strcpy(Symbols[count_simb].expr_ptr->vector[i]->type_name, type_name);
    	}

        setScope();
        count_simb++;

    } else std::cerr << "Exceeded maximum number of symbols." << std::endl;
}

void SymbolTable::add_matrix(const char* name, const char* type_name, int size1, int size2)
{
    if (count_simb < NMAX)
    {
        Symbols[count_simb].expr_ptr = new struct expr;

        update_array_size(size1, size2);
        
        strcpy(Symbols[count_simb].expr_ptr->name, name);
        Symbols[count_simb].const_flag = false;

        strcpy(Symbols[count_simb].expr_ptr->name, name);
        Symbols[count_simb].expr_ptr->type = find_type(type_name);
        strcpy(Symbols[count_simb].expr_ptr->type_name, type_name);
        Symbols[count_simb].expr_ptr->is_init = 1;

        Symbols[count_simb].expr_ptr->is_matrix = 1;
        Symbols[count_simb].type_vec = 2;
        Symbols[count_simb].expr_ptr->matrix = (expr***)calloc(size1, sizeof(expr*));

        for (int i = 0; i < size1; i++)
        {
            Symbols[count_simb].expr_ptr->matrix[i] = (expr**)calloc(size2, sizeof(expr*));
            for (int j = 0; j < size2; j++)
            {
                Symbols[count_simb].expr_ptr->matrix[i][j] = (expr*)calloc(1, sizeof(expr));
            	Symbols[count_simb].expr_ptr->matrix[i][j]->type = find_type(type_name);
            	strcpy(Symbols[count_simb].expr_ptr->matrix[i][j]->type_name, type_name);
            }
        }
    	

        setScope();
        count_simb++;

    } else std::cerr << "Exceeded maximum number of symbols." << std::endl;
    

}

int AST::get_size()
{
    return nodes_stack_cnt;
}

void SymbolTable::dellocEverything()
{
    for (int i = 0; i < count_simb; i++)
    {
        if(Symbols[i].expr_ptr->is_vec == 1){
        	for(int k=0;k<Symbols[i].expr_ptr->array_size;k++)
            	delete(Symbols[i].expr_ptr->vector[k]);
    	}
        if (Symbols[i].expr_ptr != NULL)
        {
            if (Symbols[i].expr_ptr->string_value != NULL)
                free(Symbols[i].expr_ptr->string_value);
            free(Symbols[i].expr_ptr);
        }
    }
}



// int main()
// {
//     AST myAST;
//     SymbolTable symbolTable;
//     expr * ceva = new_float_expr(1.1);
//     symbolTable.add_symbol("ceva", "float", ceva);

//     expr * ceva2 = new_int_expr(1);
//     symbolTable.add_symbol("da", "integer", ceva2);

//     expr * ceva3 = new_char_expr('a');
//     symbolTable.add_symbol("d1", "char", ceva3);

//     char * st = "da";
//     expr * ceva4 = new_string_expr(st);
//     symbolTable.add_symbol("d2", "string", ceva4);

//     expr * ceva6 = new_int_expr(3);
//     symbolTable.add_symbol("#c", "da", ceva6);

//     symbolTable.get_data();
//     return 0;
// }