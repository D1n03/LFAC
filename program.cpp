#include "program.h"
#include <fstream>
#include <iomanip>

std::ofstream file_table;

std::ofstream file_table_fn;

std::stack<std::string> scopeStack;

Symbol::Symbol() : const_flag(false) {}
Symbol::Symbol(expr* ptr) : expr_ptr(ptr) {}

SymbolTable::SymbolTable() : symbols_amount(0) {}
expr* SymbolTable::exists(const char* name, const char* type) {
    for (int i = 0; i < symbols_amount; i++) {
        if (!strcmp(Symbols[i].expr_ptr->name,name) && !strcmp(Symbols[i].expr_ptr->type_name,type))
            return Symbols[i].expr_ptr;
    }
    return nullptr;
}

expr* SymbolTable::search_by_name(const char* name) {
    std::string strname = std::string(name);
    for (int i = 0; i < symbols_amount; i++) {
        if (Symbols[i].expr_ptr->name == strname)
            return Symbols[i].expr_ptr;
    }
    return nullptr;
}

int find_type(const std::string& type_name)
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
    if (type_name == "class")
        return 6;
    return -1;
}

void SymbolTable::add_symbol(const char* name, const char * type_name, struct expr* init_val) 
{
    if (symbols_amount < NMAX) {
        if (init_val == nullptr) {
            Symbols[symbols_amount].expr_ptr = new struct expr;
            Symbols[symbols_amount].expr_ptr->is_init = 0;
            Symbols[symbols_amount].expr_ptr->type = find_type(type_name);
            strcpy(Symbols[symbols_amount].expr_ptr->name, name);
            strcpy(Symbols[symbols_amount].expr_ptr->type_name, type_name);
            Symbols[symbols_amount].const_flag = false;
            Symbols[symbols_amount].expr_ptr->is_const = 0;
            Symbols[symbols_amount].expr_ptr->is_vec = 0;
            Symbols[symbols_amount].type_vec = -1;
        } else {
            Symbols[symbols_amount].expr_ptr = init_val;
            Symbols[symbols_amount].const_flag = true;
            Symbols[symbols_amount].expr_ptr->is_init = 1;
            strcpy(Symbols[symbols_amount].expr_ptr->name, name);
            Symbols[symbols_amount].expr_ptr->is_const = 1;
        }
        setScope();
        symbols_amount++;
    } else std::cerr << "Exceeded maximum number of symbols." << std::endl;
}

void SymbolTable::add_class(const char *name)
{
    if (symbols_amount < NMAX)
    {
        Symbols[symbols_amount].expr_ptr = new struct expr;
        Symbols[symbols_amount].expr_ptr->is_init = 0;
        Symbols[symbols_amount].expr_ptr->type = 6;
        strcpy(Symbols[symbols_amount].expr_ptr->name, name);
        strcpy(Symbols[symbols_amount].expr_ptr->type_name, "class");
        Symbols[symbols_amount].const_flag = false;
        Symbols[symbols_amount].expr_ptr->is_const = 0;
        Symbols[symbols_amount].expr_ptr->is_class = 1;
        setScope();
        symbols_amount++;
    }
    else std::cerr << "Exceeded maximum number of symbols." << std::endl;
}

void pushScope(const char* scope) 
{
    scopeStack.push(scope);
}

void popScope() 
{
    if (!scopeStack.empty())
        scopeStack.pop();
}

char* computeScope()
{
    int length = 0;
    std::stack<std::string> tempStack = scopeStack;
    std::vector<std::string> tempVector;

    while (!tempStack.empty()) {
        tempVector.push_back(tempStack.top());
        tempStack.pop();
    }

    for (const auto& s : tempVector) 
        length += s.length() + 1; // Add 1 for '~'

    char* to_return = new char[length + 1];

    // Concatenate all scopes with '~'
    strcpy(to_return, "");
    for (auto it = tempVector.rbegin(); it != tempVector.rend(); ++it) {
        strcat(to_return, it->c_str());
        strcat(to_return, "~");
    }

    return to_return;
}

void SymbolTable::setScope() 
{
    if (symbols_amount < NMAX) 
    {
        if (scopeStack.empty()) {
            Symbols[symbols_amount].expr_ptr->scope = "global"; // no scopes on the stack, so it's in the global scope
        } else {
            Symbols[symbols_amount].expr_ptr->scope = computeScope(); // compute the composite scope string
        }
    } else {
        std::cerr << "Exceeded maximum number of symbols." << "\n";
    }
}

int SymbolTable::get_symbols_amount()
{
    return symbols_amount;
}

void SymbolTable::get_data() //print data to check if it's ok
{
    for (int i = 0; i < symbols_amount; i++)
    {
        std::cout << Symbols[i].expr_ptr->type_name << " " << Symbols[i].expr_ptr->name  << Symbols[i].expr_ptr->scope << " " << Symbols[i].expr_ptr->type << "\n";
    }
}

struct node *AST::build_AST(root_data * root, node * left_tree, node* right_tree, int type)
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

int AST::evaluate_AST(node *ast)
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
            return evaluate_AST(ast->left) + evaluate_AST(ast->right);
        else if (ast->root->op == MINUS)
            return evaluate_AST(ast->left) - evaluate_AST(ast->right);
        else if (ast->root->op == MULTIPLY)
            return evaluate_AST(ast->left) * evaluate_AST(ast->right);
        else if (ast->root->op == DIVIDE)
            return evaluate_AST(ast->left) / evaluate_AST(ast->right);
        else if (ast->root->op == MOD)
            return evaluate_AST(ast->left) % evaluate_AST(ast->right);
        else return evaluate_AST_bool(ast);
    }
    return 0;
}

float AST::evaluate_AST_float(node *ast)
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
            return evaluate_AST_float(ast->left) + evaluate_AST_float(ast->right);
        else if (ast->root->op == MINUS)
            return evaluate_AST_float(ast->left) - evaluate_AST_float(ast->right);
        else if (ast->root->op == MULTIPLY)
            return evaluate_AST_float(ast->left) * evaluate_AST_float(ast->right);
        else if (ast->root->op == DIVIDE)
            return evaluate_AST_float(ast->left) / evaluate_AST_float(ast->right);
        else return evaluate_AST_bool(ast);
    }
    return 0;
}

int  AST::evaluate_AST_bool(node *ast) 
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
            return evaluate_float(ast->left, ast->right, ast->root->op);
    }
    return 0;
}

int AST::evaluate(node *left, node *right, int type)
{
    if (type == LESSTHAN)
    {
        return (evaluate_AST(left) < evaluate_AST(right));
    }
    if (type == LESSEQTHAN)
    {
        return (evaluate_AST(left) <= evaluate_AST(right));
    }
    if (type == GREATERTHAN)
    {
        return (evaluate_AST(left) > evaluate_AST(right));
    }
    if (type== GREATEREQTHAN)
    {
        return (evaluate_AST(left) >= evaluate_AST(right));
    }
    if (type == EQUAL)
    {
        return (evaluate_AST(left) == evaluate_AST(right));
    } 
    if (type == NOTEQUAL)
    {
        return (evaluate_AST(left) != evaluate_AST(right));
    } 
    if (type == NOT)
    {
        if (right->expr_type == OP)
            return !evaluate_AST_bool(right);
        else return !evaluate_AST(right);
    } 
    return 0;
}

int AST::evaluate_float(node *left, node *right, int type)
{
    if (type == LESSTHAN)
    {
        return (evaluate_AST_float(left) < evaluate_AST_float(right));
    }
    if (type == LESSEQTHAN)
    {
        return (evaluate_AST_float(left) <= evaluate_AST_float(right));
    }
    if (type == GREATERTHAN)
    {
        return (evaluate_AST_float(left) > evaluate_AST_float(right));
    }
    if (type== GREATEREQTHAN)
    {
        return (evaluate_AST_float(left) >= evaluate_AST_float(right));
    }
    if (type == EQUAL)
    {
        return (evaluate_AST_float(left) == evaluate_AST_float(right));
    } 
    if (type == NOTEQUAL)
    {
        return (evaluate_AST_float(left) != evaluate_AST_float(right));
    } 
    if (type == NOT)
    {
        if (right->expr_type == OP)
            return !evaluate_AST_bool(right);
        return !evaluate_AST_float(right);
    }
    return 0;
}

int AST::evaluate_bool(node *left, node *right, int type)
{
    if (type == EQUAL)
    {
        return (evaluate_AST_bool(left) == evaluate_AST_bool(right));
    } 
    if (type == NOTEQUAL)
    {
        return (evaluate_AST_bool(left) != evaluate_AST_bool(right));
    } 
    if (type == NOT)
    {
        if (right->expr_type == OP)
            return !evaluate_AST_bool(right);
        return !evaluate_AST_bool(right);
    }
    return 0;
}

void AST::deallocate_AST(node *root_data)
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
    	deallocate_AST(root_data->left);
    	delete root_data->left;
    	deallocate_AST(root_data->right);
    	delete root_data->right;
    }
}

void AST::deallocateStack()
{
    while (nodes_stack_cnt > 0)
    {
        deallocate_AST(nodes_stack[--nodes_stack_cnt]);
        nodes_stack[nodes_stack_cnt] = NULL;
    }
}

void AST::build_AST_root(int op)
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
        struct node *root_new = build_AST(data, left, right, OP);
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

expr* new_bool_expr(int value)
{
	struct expr* new_expr = new struct expr;
	new_expr->int_value = value;
	new_expr->type = 5;
	strcpy(new_expr->type_name, "boolean");
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
    int cnt = get_symbols_amount();
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
    Symbols[symbols_amount].expr_ptr->array_size = new_size1;
    Symbols[symbols_amount].expr_ptr->array_size_2 = new_size2;
}


void SymbolTable::add_array(const char* name, const char* type_name, int new_array_size)
{
    if (symbols_amount < NMAX) 
    {
        Symbols[symbols_amount].expr_ptr = new struct expr;

        update_array_size(new_array_size);
        int vsize = Symbols[symbols_amount].expr_ptr->array_size;

        strcpy(Symbols[symbols_amount].expr_ptr->name, name);
        Symbols[symbols_amount].const_flag = false;

        strcpy(Symbols[symbols_amount].expr_ptr->name, name);
        Symbols[symbols_amount].expr_ptr->type = find_type(type_name);
        strcpy(Symbols[symbols_amount].expr_ptr->type_name, type_name);
        Symbols[symbols_amount].expr_ptr->is_init = 1;


        Symbols[symbols_amount].expr_ptr->is_vec = 1;
        Symbols[symbols_amount].type_vec = 1;
    	Symbols[symbols_amount].expr_ptr->vector = (expr**)calloc(vsize, sizeof(expr*));

        // initialize array of pointers
    	for(int i = 0; i < vsize ; i++){
        	Symbols[symbols_amount].expr_ptr->vector[i] = new struct expr;
        	Symbols[symbols_amount].expr_ptr->vector[i]->type = find_type(type_name);
        	strcpy(Symbols[symbols_amount].expr_ptr->vector[i]->type_name, type_name);
    	}

        setScope();
        symbols_amount++;

    } else std::cerr << "Exceeded maximum number of symbols." << std::endl;
}

void SymbolTable::add_matrix(const char* name, const char* type_name, int size1, int size2)
{
    if (symbols_amount < NMAX)
    {
        Symbols[symbols_amount].expr_ptr = new struct expr;

        update_array_size(size1, size2);
        
        strcpy(Symbols[symbols_amount].expr_ptr->name, name);
        Symbols[symbols_amount].const_flag = false;

        strcpy(Symbols[symbols_amount].expr_ptr->name, name);
        Symbols[symbols_amount].expr_ptr->type = find_type(type_name);
        strcpy(Symbols[symbols_amount].expr_ptr->type_name, type_name);
        Symbols[symbols_amount].expr_ptr->is_init = 1;

        Symbols[symbols_amount].expr_ptr->is_matrix = 1;
        Symbols[symbols_amount].type_vec = 2;
        Symbols[symbols_amount].expr_ptr->matrix = (expr***)calloc(size1, sizeof(expr*));

        for (int i = 0; i < size1; i++)
        {
            Symbols[symbols_amount].expr_ptr->matrix[i] = (expr**)calloc(size2, sizeof(expr*));
            for (int j = 0; j < size2; j++)
            {
                Symbols[symbols_amount].expr_ptr->matrix[i][j] = (expr*)calloc(1, sizeof(expr));
            	Symbols[symbols_amount].expr_ptr->matrix[i][j]->type = find_type(type_name);
            	strcpy(Symbols[symbols_amount].expr_ptr->matrix[i][j]->type_name, type_name);
            }
        }
    	

        setScope();
        symbols_amount++;

    } else std::cerr << "Exceeded maximum number of symbols." << std::endl;
    

}

int AST::get_size()
{
    return nodes_stack_cnt;
}

void SymbolTable::dellocEverything()
{
    for (int i = 0; i < symbols_amount; i++)
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

FunctionTable::FunctionTable() : call_cnt(0), params_cnt(0), cnt_fn(0) {}

struct expr** FunctionTable::exists_fn(const char* name, int cnt_param)
{
    for (int i = 0; i < cnt_fn; i++) {
        if (!strcmp(Functions[i].name,name) && Functions[i].count_params == cnt_param)
            return Functions[i].array_params;
    }
    return nullptr;
}

struct expr* FunctionTable::get_expr_fn(const char * name)
{
    for (int i = 0; i < cnt_fn; i++) {
        if (!strcmp(Functions[i].name,name))
            return Functions[i].expr_ptr;
    }
    return nullptr;
}

int FunctionTable::empty_fn(const char * name)
{
    for (int i = 0; i < cnt_fn; i++) {
        if (!strcmp(Functions[i].name,name) && Functions[i].count_params == 0)
            return 1;
    }
    return 0;
}

void FunctionTable::create_fn(const char* name, char return_type[], int is_empty)
{
    strcpy(Functions[cnt_fn].name, name);
    strcpy(Functions[cnt_fn].return_type, return_type);
    if (is_empty)
    {
        Functions[cnt_fn].count_params = 0;
        Functions[cnt_fn].array_params = nullptr;
    }
    else 
    {
        Functions[cnt_fn].count_params = params_cnt;
        Functions[cnt_fn].array_params = (struct expr**)malloc(params_cnt* sizeof(struct expr*));
        for (int i = 0; i < params_cnt; i++)
            Functions[cnt_fn].array_params[i] = params[i];
    }
    Functions[cnt_fn].expr_ptr = new struct expr;
    strcpy(Functions[cnt_fn].expr_ptr->name, name);
    strcpy(Functions[cnt_fn].expr_ptr->type_name, return_type);
    Functions[cnt_fn].expr_ptr->type = find_type(return_type);
    popScope();
    if (scopeStack.empty())
        Functions[cnt_fn].expr_ptr->scope = strdup("global");
    else Functions[cnt_fn].expr_ptr->scope = computeScope();
    cnt_fn++;
}

void FunctionTable::table_function_display()
{
    file_table_fn.open("symbol_table_functions.txt");
    for (int i = 0; i < cnt_fn; i++) {
        file_table_fn << "Type: " << Functions[i].return_type << "\tName: " << Functions[i].name << "\tScope: " << Functions[i].expr_ptr->scope << "\n\tParams:\n";

        for (int j = 0; j < Functions[i].count_params; j++) {
            if (Functions[i].array_params[j]->is_const == 1)
                file_table_fn << "\t" << "const";
            file_table_fn << "\t" << Functions[i].array_params[j]->type_name << "\t" << Functions[i].array_params[j]->name << "\n";
        }

        file_table_fn << "\n";
    }
    file_table_fn.close();
}