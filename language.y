%{
#include <iostream>
#include <string.h>
#include "program.h"
extern FILE* yyin;
extern char* yytext;
extern int line_nr;
extern int ch_nr;
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

%type <ptr_expr> EXPRESSIONS EXPRESSION
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
progr: declarations {std::cout << "The programme is correct!\n";}
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
                                                            else std::cout << "Error at line " << line_nr + 1 << " " << ch_nr << " " << "Variable " << $2 << " has already been declared\n";
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
                                                            else std::cout << "Error at line " << line_nr + 1 << " " << ch_nr << " " << "Variable " << $3 << " has already been declared\n";
                                                       }
               
               ;

EXPRESSIONS    : EXPRESSION
               | EXPRESSIONS EXPRESSION
               ;

EXPRESSION     : INT_VAL                     {   
                                                  if(is_error == 0) 
                                                  {
                                                       struct root_data* data = new struct root_data;
                                                       data->number = $1;
                                                       struct node* current_node = myAST.buildAST(data, nullptr, nullptr, NUMBER);
                                                       myAST.nodes_stack.push(current_node);
                                                       $$ = new_int_expr($1);
                                                  }
                                             }
               | FLOAT_VAL 	               { $$ = new_float_expr($1); }
               | CHAR_VAL  	               { $$ = new_char_expr($1); }
               | STRING_VAL	               { $$ = new_string_expr($1); }
               | BOOL_VAL  	               { $$ = new_bool_expr($1); }
               ;



%%
void yyerror(const char * s){
printf("error: %s at line:%d\n",s,yylineno);
}


int main(int argc, char** argv){
     yyin=fopen(argv[1],"r");
     yyparse();
} 