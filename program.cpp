#include "program.h"
using namespace std;

FILE * file_table;

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

node *AST::buildAST(root_data * root, node * left_tree, node* right_tree, int type)
{
    struct node* new_node = new struct node;
    new_node->root = root;
    new_node->expr_type = type;
    new_node->left = left_tree;
    new_node->right = right_tree;
    return new_node;
}

int AST::evalAST(node *ast)
{
    if (ast->expr_type == NUMBER)
        return ast->root->number;

    if (ast->expr_type == IDENTIFIER)
        return ast->root->expr_ptr->int_value;

    if (ast->expr_type == UNKNOWN)
        return 0;

    if (ast->expr_type == OP)
    {
        if (ast->root->op == '+')
            return evalAST(ast->left) + evalAST(ast->right);
        if (ast->root->op == '-')
            return evalAST(ast->left) - evalAST(ast->right);
        if (ast->root->op == '*')
            return evalAST(ast->left) * evalAST(ast->right);
        if (ast->root->op == '/')
            return evalAST(ast->left) / evalAST(ast->right);
        if (ast->root->op == '%')
            return evalAST(ast->left) % evalAST(ast->right);
    }
    return -1;
}

void AST::deallocateAST(node *root_data)
{
    if (root_data != nullptr)
    {
        if (root_data->root->unknown != NULL)
        {
            delete root_data->root->unknown;
        }
        delete root_data->root;
        deallocateAST(root_data->left);
        delete root_data->left ;
        deallocateAST(root_data->right);
        delete root_data->right ;
        nodes_stack.pop();
    }
}
void AST::deallocateStack()
{
    while (!nodes_stack.empty())
    {
        deallocateAST(nodes_stack.top());
        nodes_stack.pop();
    }
}
void AST::buildASTRoot(char op)
{
    root_data *data = new struct root_data;
    data->op = op;

    node *right = nodes_stack.top();
    nodes_stack.pop();
    node *left = nodes_stack.top();
    nodes_stack.pop();

    node *root = buildAST(data, left, right, OP);
    nodes_stack.push(root);
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

void free_expr(expr* expr){
	if(expr->string_value != NULL)
    	free(expr->string_value);
	free(expr);
}

void write_expr(struct expr* source){
	if(source->type == 1)
    	fprintf(file_table,"%d ", source->int_value);
	if(source->type == 2)
    	if(source->char_value>= 33 && source->char_value <=126)
        	fprintf(file_table, "%c ", source->char_value);
    	else
        	fprintf(file_table,"# ");
	if(source->type == 3)
    	fprintf(file_table,"%s ", source->string_value);
	if(source->type == 4)
    	fprintf(file_table,"%f ", source->float_value);
	if(source->type == 5)
    	fprintf(file_table,"%s ", source->int_value ? "true" : "false");
	if(source->type == 6)
    	fprintf(file_table,"{class}");
}

void SymbolTable::table_symbol_display(){
    file_table = fopen("symbol_table.txt","w");
    int cnt = get_count_simb();
    for(int i=0; i< cnt; i++)
    {
   	 
   	fprintf(file_table, "Nume: %s\t\t Tip: %s",Symbols[i].expr_ptr->name, Symbols[i].expr_ptr->type_name);
    	if(Symbols[i].expr_ptr->is_vec == 1)
        	fprintf(file_table, "[%d]", Symbols[i].expr_ptr->array_size);
   	fprintf(file_table, "\tConstant: %s\t",Symbols[i].const_flag ? "true" : "false");
    fprintf(file_table, "Scope: %s\t", Symbols[i].expr_ptr->scope);
    if(Symbols[i].expr_ptr->is_init == 1)
    {

        if(Symbols[i].expr_ptr->is_vec == 1){
            fprintf(file_table, "\n\tValori: ");
            for(int k = 0;k < Symbols[i].expr_ptr->array_size;k++)
                write_expr(Symbols[i].expr_ptr->vector[k]);
            fprintf(file_table,"\n");
        }
        else{
            fprintf(file_table, "Valoare: ");
            write_expr(Symbols[i].expr_ptr);
        }
    }
    else   	 
    fprintf(file_table,"Neinitializat");
   	fprintf(file_table,"\n");
    }
    fclose(file_table);
}

void SymbolTable::update_array_size(int new_size)
{
    if (new_size > 0)
        Symbols[count_simb].expr_ptr->array_size = new_size;
    else
        std::cerr << "Invalid vector size." << std::endl;
}


void SymbolTable::add_array(const char* name, const char* type_name, int new_array_size)
{
    if (count_simb < MAX_SYMBOLS) {
        if(new_array_size < 1)
            std::cerr << "Invalid vector size." << std::endl;

        //Symbols[count_simb].expr_ptr = (expr*)calloc(1, sizeof(expr));
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