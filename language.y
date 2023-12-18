%{
#include <iostream>
#include <string.h>
#include "program.h"
extern FILE* yyin;
extern char* yytext;
extern int yylineno;
extern int yylex();
void yyerror(const char * s);

bool is_error = false;

class SymbolTable symbolTable;
class AST myAST;

%}
%union {
     char* val_name;
     char* data_type;
     int int_val;
     float float_val;
     char char_val;
     char* string_val;
     struct expr *ptr_expr;
}

%token END ASSIGN EVAL TYPEOF RETURN CONST 
%token CLASS PUBLIC PRIVATE
%token IF ELSE WHILE FOR
%token LT LE GT GE EQ NEQ AND_OP OR_OP   
%token <int_val> INT_VAL BOOL_VAL
%token <float_val> FLOAT_VAL  
%token <char_val> CHAR_VAL
%token <string_val> STRING_VAL
%token <val_name> ID BGIN
%token <val_name> ID_FUNCT
%token <data_type> VOID INT FLOAT CHAR STRING BOOL

%type <ptr_expr> EXPRESSIONS EXPRESSION left_value right_value 
%type <data_type> type_var

%left OR_OP
%left AND_OP
%left EQ NEQ
%left LT LE GT GE
%left '+' '-' 
%left '*' '/' '%'
%left '(' ')' '[' ']' '{' '}'

%start progr
%%
progr          : declarations block          {std::cout << "The programme is correct!\n";}
               | block                       {std::cout << "The programme is correct!\n";}
     ;

type_var       : INT 
               | FLOAT 
               | CHAR 
               | STRING 
               | BOOL
               ;

declarations   : declaration ';'                       {is_error = false;}
               | declarations declaration ';'          {is_error = false;}
               ;

declaration    : type_var ID                           {    if(symbolTable.search_by_name($2) == nullptr)
                                                                 symbolTable.add_symbol($2, $1, nullptr);
                                                            else std::cout << "Error at line " << yylineno << " Variable " << $2 << " has already been declared\n";
                                                            free($2); free($1);
                                                       }
               | CONST type_var ID ASSIGN EXPRESSIONS  {    if(symbolTable.search_by_name($3) == nullptr) 
                                                            {
                                                                 if (!myAST.nodes_stack.empty())
                                                                 {
                                                                      myAST.deallocateAST(myAST.nodes_stack.top());
                                                                 }
                                                                 symbolTable.add_symbol($3, $2, $5);
                                                            }
                                                            else std::cout << "Error at line " << yylineno << " Variable " << $3 << " has already been declared\n";
                                                       }
               | type_var ID '[' INT_VAL ']'           {    if (!strcmp($1, "string"))
                                                            {
                                                                 yyerror("Wrong data type for declaration of the array!\n");
                                                            }
                                                            else if(symbolTable.search_by_name($2) == nullptr)
                                                            {
                                                                 if ($4 > 0)
                                                                      symbolTable.add_array($2, $1, $4);
                                                                 else yyerror("Invalid vector size!\n");
                                                            }
                                                            else std::cout << "Error at line " << yylineno << "Variable " << $2 << " has already been declared\n";
                                                            free($2); free($1); 
                                                       }
               ;

begin_scope    : BGIN                                  {symbolTable.pushScope($1);}
               ;

block          : begin_scope declarations list END     {symbolTable.popScope();}
               | begin_scope list END                  {symbolTable.popScope();}
               ;

list           : instruction ';'                       {is_error = false;}
               | list instruction  ';'                 {is_error = false;}
               ;

instruction    : left_value ASSIGN right_value    {    struct expr* the_left_val = $1;
                                                       bool dell = false;
                                                       if (the_left_val != nullptr)
                                                       {
                                                            if (!$1->is_const)
                                                            {
                                                                 if ($1->type == $3->type)
                                                                 {
                                                                      if ($3->type == 1) // integer
                                                                      {
                                                                           if (myAST.nodes_stack.size() == 1)
                                                                           {
                                                                                the_left_val->int_value = myAST.evalAST(myAST.nodes_stack.top());
                                                                                if (!myAST.nodes_stack.empty())
                                                                                     myAST.deallocateAST(myAST.nodes_stack.top());
                                                                           }
                                                                           else the_left_val->int_value = 0;
                                                                      }
                                                                      if ($3->type == 2) // char
                                                                      {
                                                                           the_left_val->char_value = $3->char_value;
                                                                      }
                                                                      if ($3->type == 3) // string
                                                                      {
                                                                           if ($3->string_value == NULL)
                                                                           {
                                                                                std::cout << "Error at line " << yylineno << " " << $3->name << " doesn't have a value\n";
                                                                           }
                                                                           else 
                                                                           {
                                                                                if (the_left_val->string_value != NULL)
                                                                                {
                                                                                     free(the_left_val->string_value);
                                                                                }
                                                                                the_left_val->string_value = strdup($3->string_value);
                                                                           }
                                                                      }
                                                                      if ($3->type == 4) //float 
                                                                      {
                                                                           the_left_val->float_value = $3->float_value;
                                                                      }
                                                                      if ($3->type == 5) // bool
                                                                      {
                                                                           the_left_val->int_value = $3->int_value;
                                                                      }
                                                                      the_left_val->is_init = 1;
                                                                 }
                                                                 else 
                                                                 {
                                                                      std::cout << "Error at line " << yylineno << " Incompatible types: " << $1->type_name << " " << $3->type_name << "\n";
                                                                      myAST.deallocateStack();
                                                                 }
                                                            }
                                                            else 
                                                            {
                                                                 std::cout << "Error at line " << yylineno << " " << $1->name << " is constant\n"; 
                                                                 myAST.deallocateStack();
                                                            }
                                                       }
                                                       myAST.deallocateStack();
                                                  }   

left_value     : ID                               {    $$ = symbolTable.search_by_name($1);
                                                       if ($$ == nullptr)
                                                            std::cout << "Error at line " << yylineno << " Variable " << $1 << " has not been declared\n";
                                                       free($1);
                                                  }  
               ;   

right_value    : EXPRESSIONS {$$ = $1;}                     
               ;

/* STATES         : STATES AND_OP STATES
               | STATES OR_OP STATES
               | STATE 
               | '('STATES')'
               ;

STATE          : EXPRESSIONS   EQ   EXPRESSIONS   {myAST.deallocateStack();}
               | EXPRESSIONS   NEQ  EXPRESSIONS   {myAST.deallocateStack();}
               | EXPRESSIONS   GT   EXPRESSIONS   {myAST.deallocateStack();}
               | EXPRESSIONS   GE   EXPRESSIONS   {myAST.deallocateStack();}
               | EXPRESSIONS   LT   EXPRESSIONS   {myAST.deallocateStack();}
               | EXPRESSIONS   LE   EXPRESSIONS   {myAST.deallocateStack();}
               ; */

EXPRESSIONS    : EXPRESSION
               | EXPRESSIONS EXPRESSION
               ;

EXPRESSION     : EXPRESSION '+' EXPRESSION   {    if ($1->type == 1 && $3->type == 1) // for integer
                                                  {
                                                       int val = $1->int_value+$3->int_value;
                                                       $$ = new_int_expr(val);
                                                       if (is_error == 0)
                                                            myAST.buildASTRoot('+');
                                                  }
                                                  else if ($1->type == 3 && $3 -> type == 3) // for string
                                                       $$ = concat_string_expr($1->string_value, $3->string_value);
                                                  else if ($1->type == 4 && $3 -> type == 4) // for float
                                                       $$ = new_float_expr($1->float_value + $3->float_value);
                                                  else {
                                                       is_error = true;
                                                       std::cout << "Error at line " << yylineno << " Incompatible types: " << $1->type_name << " " << $3->type_name << "\n";
                                                  }
                                             }  
               | EXPRESSION '-' EXPRESSION   {

                                             }  
               | EXPRESSION '*' EXPRESSION   {

                                             }  
               | EXPRESSION '/' EXPRESSION   {

                                             }  
               | EXPRESSION '%' EXPRESSION   {

                                             }    
               | '(' EXPRESSION ')'          {$$ = $2;}
               | INT_VAL                     {   
                                                  if(is_error == 0) 
                                                  {
                                                       struct root_data* r_data = new struct root_data;
                                                       r_data->number = $1;
                                                       struct node* current_node = myAST.buildAST(r_data, nullptr, nullptr, NUMBER);
                                                       myAST.nodes_stack.push(current_node);
                                                       $$ = new_int_expr($1);
                                                  }
                                             }
               | FLOAT_VAL 	               { $$ = new_float_expr($1); }
               | CHAR_VAL  	               { $$ = new_char_expr($1); }
               | STRING_VAL	               { $$ = new_string_expr($1); }
               | BOOL_VAL  	               { $$ = new_bool_expr($1); }
               | ID                          {    struct expr* get_id_data = symbolTable.search_by_name($1);
                                                  if (get_id_data != nullptr)
                                                  {
                                                       if (get_id_data->type == 1 && !is_error)
                                                       {
                                                            struct root_data* r_data = new struct root_data;
                                                            r_data->expr_ptr = get_id_data;
                                                            struct node* current_node =  myAST.buildAST(r_data, nullptr, nullptr, IDENTIFIER);
                                                            myAST.nodes_stack.push(current_node);
                                                       }
                                                       $$ = get_id_data;
                                                  }
                                                  else {
                                                       is_error = true;
                                                       std::cout << "Error at line " << yylineno << " Variable " << $1 << " has not been declared\n";
                                                  }
                                                  free($1);
                                             }
               ;



%%
void yyerror(const char * s){
printf("error: %s at line:%d\n",s,yylineno);
}

int main(int argc, char** argv){
     yyin=fopen(argv[1],"r");
     yyparse();
     symbolTable.table_symbol_display();
     std::cout << myAST.get_size();
} 