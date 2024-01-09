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
class FunctionTable functionTable;

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
%token LT LE GT GE EQ NEQ AND_OP OR_OP NOT_OP
%token <int_val> INT_VAL BOOL_VAL
%token <float_val> FLOAT_VAL  
%token <char_val> CHAR_VAL
%token <string_val> STRING_VAL
%token <val_name> ID BGIN
%token <val_name> ID_FUNCT
%token <data_type> VOID INT FLOAT CHAR STRING BOOL

%type <ptr_expr> EXPRESSION left_value right_value array_id_label call_params call_function param
%type <data_type> type_var type_fn
%type <int_val> array_size
%type <val_name> fn_name

%left OR_OP
%left AND_OP
%left LT LE GT GE EQ NEQ
%left NOT_OP
%left '+' '-' 
%left '*' '/' '%'
%left '{' '}' '[' ']' '(' ')' 

%start progr
%%
progr          : declarations block          {std::cout << "The programme is correct!\n";}
               | block                       {std::cout << "The programme is correct!\n";}
               | functions block             {std::cout << "The programme is correct!\n";}
               | declarations functions block{std::cout << "The programme is correct!\n";}
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
                                                            else 
                                                            {
                                                                 std::cout << "Error at line " << yylineno << " .Variable " << $2 << " has already been declared\n";
                                                                 YYERROR;
                                                            }
                                                            free($2); free($1);
                                                       }
               | CONST type_var ID ASSIGN EXPRESSION  {    if(symbolTable.search_by_name($3) == nullptr) 
                                                            {
                                                                 if (myAST.nodes_stack_cnt > 0)
                                                                 {
                                                                      myAST.deallocateAST(myAST.nodes_stack[--myAST.nodes_stack_cnt]);
                                                                 }
                                                                 symbolTable.add_symbol($3, $2, $5);
                                                            }
                                                            else 
                                                            {
                                                                 std::cout << "Error at line " << yylineno << " .Variable " << $3 << " has already been declared\n";
                                                                 YYERROR;
                                                            }
                                                       }
               | type_var ID '[' INT_VAL ']'           {    if (!strcmp($1, "string"))
                                                            {
                                                                 std::cout << "Error at line " << yylineno <<  "Wrong data type for declaration of the array!\n";
                                                                 YYERROR;
                                                            }
                                                            else if(symbolTable.search_by_name($2) == nullptr)
                                                            {
                                                                 if ($4 > 0)
                                                                      symbolTable.add_array($2, $1, $4);
                                                                 else {
                                                                      std::cout << "Error at line " << yylineno << "Invalid vector size!\n";
                                                                      YYERROR;
                                                                 }
                                                            }
                                                            else 
                                                            {
                                                                 std::cout << "Error at line " << yylineno << " .Variable " << $2 << " has already been declared\n";
                                                                 YYERROR;
                                                            } 
                                                            free($2); free($1); 
                                                       }
               | type_var ID '[' INT_VAL ']' '[' INT_VAL ']'     {    if (!strcmp($1, "string"))
                                                                      {
                                                                           std::cout << "Error at line " << yylineno << "Wrong data type for declaration of the matrix!\n";
                                                                           YYERROR;
                                                                      }
                                                                      else if(symbolTable.search_by_name($2) == nullptr)
                                                                      {
                                                                           if (($4 > 0) && ($7 > 0))
                                                                                symbolTable.add_matrix($2, $1, $4, $7);
                                                                           else 
                                                                           {
                                                                                std::cout << "Error at line " << yylineno << "Invalid matrix size!\n";
                                                                                YYERROR;
                                                                           } 
                                                                      }
                                                                      else 
                                                                      {
                                                                           std::cout << "Error at line " << yylineno << " .Variable " << $2 << " has already been declared\n";
                                                                           YYERROR;
                                                                      }
                                                                      free($2); free($1); 

                                                                 }
                                                                 
               ;

call_function  : ID_FUNCT '(' call_list_fn ')'         {    struct expr** val = functionTable.exists_fn($1, functionTable.call_cnt);
                                                            if (val != nullptr)
                                                            {
                                                                 for (int i = 0; i < functionTable.call_cnt; i++)
                                                                 {
                                                                      if (val[i]->type != functionTable.call_function_list[i]->type)
                                                                      {
                                                                           is_error = true;
                                                                           std::cout << "Error at line " << yylineno << " Wrong arguments: " << val[i]->type_name << " " << functionTable.call_function_list[i]->type_name << "\n";
                                                                           YYERROR;
                                                                      }
                                                                 }
                                                                 $$ = functionTable.get_expr_fn($1);
                                                            }
                                                            else 
                                                            {
                                                                 is_error = true;
                                                                 std::cout << "Error at line " << yylineno << " Function " << $1 << " was not declared\n";
                                                            }
                                                            functionTable.call_cnt = 0;
                                                       }
               | ID_FUNCT '(' ')'                      {    if (functionTable.empty_fn($1))
                                                                 $$ = functionTable.get_expr_fn($1);
                                                            else 
                                                            {
                                                                 is_error = true;
                                                                 std::cout << "Error at line " << yylineno << " Function " << $1 << " was not declared\n";
                                                                 YYERROR;
                                                            }
                                                       }
               ;

call_list_fn   : call_list_fn ',' call_params          {    functionTable.call_function_list[functionTable.call_cnt] = $3;
                                                            functionTable.call_cnt++;
                                                       }
               | call_params                           {    functionTable.call_function_list[functionTable.call_cnt] = $1;
                                                            functionTable.call_cnt++;
                                                       }
               ;

call_params    : EXPRESSION                           {     $$ = $1;
                                                            if ($1->type == 1 && myAST.nodes_stack_cnt > 0)
                                                                 myAST.deallocateAST(myAST.nodes_stack[--myAST.nodes_stack_cnt]);   
                                                       }  
               ; 

begin_scope    : BGIN                                  {pushScope($1);}
               ;

block          : begin_scope declarations list END     {popScope();}
               | begin_scope list END                  {popScope();}
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
                                                                           if (myAST.nodes_stack_cnt == 1)
                                                                           {
                                                                                the_left_val->int_value = myAST.evalAST(myAST.nodes_stack[--myAST.nodes_stack_cnt]);
                                                                                myAST.deallocateAST(myAST.nodes_stack[myAST.nodes_stack_cnt]);
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
                                                                                YYERROR;
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
                                                                           if (myAST.nodes_stack_cnt == 1)
                                                                           {
                                                                                the_left_val->float_value = myAST.evalAST_f(myAST.nodes_stack[--myAST.nodes_stack_cnt]);
                                                                                myAST.deallocateAST(myAST.nodes_stack[myAST.nodes_stack_cnt]);
                                                                           }
                                                                           else the_left_val->float_value = 0;
                                                                      }
                                                                      if ($3->type == 5) // bool
                                                                      {
                                                                           if (myAST.nodes_stack_cnt == 1)
                                                                           {
                                                                                the_left_val->int_value = myAST.evalAST_b(myAST.nodes_stack[--myAST.nodes_stack_cnt]);
                                                                                myAST.deallocateAST(myAST.nodes_stack[myAST.nodes_stack_cnt]);
                                                                           }
                                                                           else the_left_val->int_value = 0;
                                                                      }
                                                                      the_left_val->is_init = 1;
                                                                 }
                                                                 else 
                                                                 {
                                                                      std::cout << "Error at line " << yylineno << " .Incompatible types: " << $1->type_name << " " << $3->type_name << "\n";
                                                                      myAST.deallocateStack();
                                                                      YYERROR;
                                                                 }
                                                            }
                                                            else 
                                                            {
                                                                 std::cout << "Error at line " << yylineno << " " << $1->name << " is constant\n"; 
                                                                 myAST.deallocateStack();
                                                                 YYERROR;
                                                            }
                                                       }
                                                       myAST.deallocateStack();
                                                  }   
               | eval_state
               | typeof_state
               | control_state
               | call_function
               | return_state
               ;
               
left_value     : ID                               {    $$ = symbolTable.search_by_name($1);
                                                       if ($$ == nullptr) 
                                                       {
                                                            std::cout << "Error at line " << yylineno << ". Variable " << $1 << " has not been declared\n";
                                                            YYERROR;
                                                       }
                                                       free($1);
                                                  }  
               | array_id_label                   { $$ = $1; }
               ;  

array_id_label : ID '[' array_size ']'                      {    struct expr* the_left_val = symbolTable.search_by_name($1);
                                                                 int index = $3;
                                                                 if (the_left_val != nullptr)
                                                                 {
                                                                      if(the_left_val->is_vec)
                                                                      {
                                                                           if(index < the_left_val->array_size)
                                                                           {
                                                                                $$ = the_left_val->vector[index];
                                                                           }
                                                                           else
                                                                           {
                                                                                std::cout << "Error at line " << yylineno << " index value is greater than the array size. \n"; 
                                                                                YYERROR;
                                                                           }    
                                                                      }
                                                                      else
                                                                      {
                                                                           std::cout << "Error at line " << yylineno << ". Variable " << $1 << " is not an array. \n"; 
                                                                           YYERROR;
                                                                      }    
                                                                 }
                                                                 else
                                                                 {
                                                                      std::cout << "Error at line " << yylineno << ". Variable " << $1 << " has not been declared\n";
                                                                      YYERROR;
                                                                 } 
                                                                 
                                                            }
               | ID '[' array_size ']' '[' array_size ']'   {    struct expr* the_left_val = symbolTable.search_by_name($1);
                                                                 int index1 = $3, index2 = $6;
                                                                 if (the_left_val != nullptr)
                                                                 {
                                                                      if(the_left_val->is_matrix)
                                                                      {
                                                                           if((index1 < the_left_val->array_size)&&(index2 < the_left_val->array_size_2))
                                                                           {
                                                                                $$ = the_left_val->matrix[index1][index2];
                                                                           }
                                                                           else
                                                                           {
                                                                                std::cout << "Error at line " << yylineno << " index value is greater than the matrix size. \n";     
                                                                                YYERROR;
                                                                           }
                                                                      }
                                                                      else
                                                                      {
                                                                           std::cout << "Error at line " << yylineno << ". Variable " << $1 << " is not an array. \n";     
                                                                           YYERROR;
                                                                      }
                                                                 }
                                                                 else 
                                                                 {
                                                                      std::cout << "Error at line " << yylineno << ". Variable " << $1 << " has not been declared\n";
                                                                      YYERROR;
                                                                 }
                                                            }
               ;

array_size     : INT_VAL                          {    $$ = $1;  }
               | ID                               {    struct expr* the_left_val = symbolTable.search_by_name($1);
                                                       if(the_left_val != nullptr)
                                                            if (the_left_val->int_value >= 0)
                                                                 $$ = the_left_val->int_value;
                                                            else {
                                                                 std::cout << "Error at line " << yylineno << ". Variable " << $1 << " has an integer value lesser than 1, therefore cannot be used as an array/matrix index.\n";     
                                                                 YYERROR;
                                                            }
                                                       else {
                                                            std::cout << "Error at line " << yylineno << ". Variable " << $1 << " has not been declared\n";
                                                            YYERROR;
                                                       }
                                                  }
               ;  

return_state   : RETURN right_value               {myAST.deallocateStack();}
               | RETURN
               |
               ;

right_value    : EXPRESSION                         { $$ = $1; }  
               ;

eval_identif   : EVAL                             {myAST.deallocateStack();}
               ;
eval_state     : eval_identif '('right_value')'     {  if (myAST.nodes_stack_cnt)
                                                            myAST.nodes_stack_cnt--;
                                                       if (myAST.nodes_stack_cnt > 0 || myAST.nodes_stack[0] == nullptr)
                                                       {
                                                            is_error = true;
                                                            if (myAST.nodes_stack_cnt > 0)
                                                                 std::cout << "Error at line " << yylineno << " .Wrong tree" << "\n";
                                                            else 
                                                                 std::cout << "Error at line " << yylineno << " .Tree not found. No arithmetic expression" << "\n";
                                                            myAST.deallocateStack();
                                                            YYERROR;
                                                       }
                                                       else {
                                                            if (!is_error)
                                                            {
                                                                 if ($3->type == 1)
                                                                      std::cout << "The value of the expression on line " << yylineno << " is " << myAST.evalAST(myAST.nodes_stack[0]) << "\n";
                                                                 if ($3->type == 4) 
                                                                      std::cout << "The value of the expression on line " << yylineno << " is " << myAST.evalAST_f(myAST.nodes_stack[0]) << "\n"; 
                                                                 if ($3->type == 5) 
                                                                 {
                                                                      int return_bool = myAST.evalAST_b(myAST.nodes_stack[0]);
                                                                      if (return_bool)
                                                                      {
                                                                           std::cout << "The value of the expression on line " << yylineno << " is true" << "\n"; 
                                                                      }
                                                                      else std::cout << "The value of the expression on line " << yylineno << " is false" << "\n"; 
                                                                 }
                                                            }
                                                            else
                                                            {
                                                                 std::cout << "Error on the line " << yylineno << " .Eval cannot by called" << "\n";
                                                                 myAST.deallocateStack();
                                                                 YYERROR;
                                                            }
                                                            myAST.deallocateStack();
                                                       }
                                                  }
               ;

typeof_state   : TYPEOF '('right_value')'         {    if (!is_error)
                                                            std::cout << "Data's type on the line " << yylineno << " is " << $3->type_name << "\n";
                                                       else 
                                                       {
                                                            std::cout << "Error at line" << yylineno << " .Typeof cannot be called" << "\n";
                                                            myAST.deallocateStack();
                                                            YYERROR;
                                                       }
                                                       myAST.deallocateStack();
                                                  }
               ;

block_instr    : declarations
               | list
               | declarations list
               ;

control_state  : if_instruction
               | while_instruction
               | for_instruction
               ;

if_instruction_id : IF             {pushScope("if");}
               ;

if_instruction : if_instruction_id '(' STATES ')' '{' block_instr '}'                          {popScope();} 
               | if_instruction_id '(' STATES ')' '{' block_instr '}' ELSE '{' block_instr '}' {popScope();} 
               ;

while_instruction_id : WHILE       {pushScope("while");}
               ;

while_instruction : while_instruction_id '(' STATES ')' '{' block_instr '}'                    {popScope();} 
               ;

for_instruction_id : FOR           {pushScope("for");}

for_instruction: for_instruction_id '(' assign_for ':' condition_for ':' change_assign ')' '{' block_instr '}' {popScope();}     
               ;

assign_for     : left_value ASSIGN right_value    {myAST.deallocateStack();}
               ;

condition_for  : STATES
               ;

change_assign  : left_value ASSIGN right_value    {myAST.deallocateStack();}
               ;

STATES         : STATES AND_OP STATES
               | '(' STATES AND_OP STATES ')'
               | STATES OR_OP STATES
               | '(' STATES OR_OP STATES ')'
               | STATE
               ;

STATE          : EXPRESSION
               ;

EXPRESSION     : EXPRESSION '+' EXPRESSION   {    if ($1->type == 1 && $3->type == 1) // for integer
                                                  {
                                                       int val = $1->int_value+$3->int_value;
                                                       $$ = new_int_expr(val);
                                                       if (!is_error)
                                                            myAST.buildASTRoot(OPS::ADD);
                                                  }
                                                  else if ($1->type == 3 && $3 -> type == 3) // for string
                                                       $$ = concat_string_expr($1->string_value, $3->string_value);
                                                  else if ($1->type == 4 && $3 -> type == 4) // for float
                                                  {
                                                       float val = $1->float_value+$3->float_value;
                                                       $$ = new_float_expr(val);
                                                       if (!is_error)
                                                            myAST.buildASTRoot(OPS::ADD);
                                                  }
                                                  else {
                                                       is_error = true;
                                                       std::cout << "Error at line " << yylineno << " .Incompatible types: " << $1->type_name << " " << $3->type_name << "\n";
                                                       YYERROR;
                                                  }
                                             }  
               | EXPRESSION '-' EXPRESSION   {    if ($1->type == 1 && $3->type == 1) // for integer
                                                  {
                                                       int val = $1->int_value-$3->int_value;
                                                       $$ = new_int_expr(val);
                                                       if (!is_error)
                                                            myAST.buildASTRoot(OPS::MINUS);
                                                  }
                                                  else if ($1->type == 4 && $3 -> type == 4) // for float
                                                  {
                                                       float val = $1->float_value-$3->float_value;
                                                       $$ = new_float_expr(val);
                                                       if (!is_error)
                                                            myAST.buildASTRoot(OPS::MINUS);
                                                  }
                                                  else {
                                                       is_error = true;
                                                       std::cout << "Error at line " << yylineno << " .Incompatible types: " << $1->type_name << " " << $3->type_name << "\n";
                                                       YYERROR;
                                                  }
                                             }  
               | EXPRESSION '*' EXPRESSION   {    if ($1->type == 1 && $3->type == 1) // for integer
                                                  {
                                                       int val = $1->int_value*$3->int_value;
                                                       $$ = new_int_expr(val);
                                                       if (!is_error)
                                                            myAST.buildASTRoot(OPS::MULTIPLY);
                                                  }
                                                  else if ($1->type == 4 && $3 -> type == 4) // for float
                                                  {
                                                       float val = $1->float_value*$3->float_value;
                                                       $$ = new_float_expr(val);
                                                       if (!is_error)
                                                            myAST.buildASTRoot(OPS::MULTIPLY);
                                                  }
                                                  else {
                                                       is_error = true;
                                                       std::cout << "Error at line " << yylineno << " .Incompatible types: " << $1->type_name << " " << $3->type_name << "\n";
                                                       YYERROR;
                                                  }

                                             }  
               | EXPRESSION '/' EXPRESSION   {    if($1->type == 1 && $3->type == 1){ //int
                                                       if($3->int_value != 0){
                                                            int val = $1->int_value/$3->int_value;
                                                            $$ = new_int_expr(val);
                                                            
                                                            if(!is_error)
                                                                 myAST.buildASTRoot(OPS::DIVIDE);                                   	 
                                                       }
                                                       else {
                                                            is_error = true;
                                                            std::cout << "Error at line " << yylineno << " .Divison by zero" << "\n";
                                                            if(myAST.nodes_stack_cnt > 0)
                                                                 myAST.deallocateAST(myAST.nodes_stack[--myAST.nodes_stack_cnt]);
                                                            myAST.nodes_stack[myAST.nodes_stack_cnt] = NULL;
                                                            YYERROR;
                                                       }
                                                  }
                                                  else if($1->type == 4 && $3->type == 4){ //float
                                                       if($3->float_value != 0){
                                                            int val = $1->float_value/$3->float_value;
                                                            $$ = new_float_expr(val);
                                                            if(!is_error)
                                                                 myAST.buildASTRoot(OPS::DIVIDE);                                    	 
                                                       }
                                                       else {
                                                            is_error = true;
                                                            std::cout << "Error at line " << yylineno << " .Divison by zero" << "\n";
                                                            if(myAST.nodes_stack_cnt > 0)
                                                                 myAST.deallocateAST(myAST.nodes_stack[--myAST.nodes_stack_cnt]);
                                                            myAST.nodes_stack[myAST.nodes_stack_cnt] = NULL;
                                                            YYERROR;
                                                       }
                                                  }
                                                  else {
                                                       is_error = true;
                                                       std::cout << "Error at line " << yylineno << " Incompatible types: " << $1->type_name << " " << $3->type_name << "\n";
                                                       YYERROR;
                                                  }
                                             }  
               | EXPRESSION '%' EXPRESSION   {    if ($1->type == 1 && $3->type == 1)
                                                  {
                                                       int val = $1->int_value % $3->int_value;
                                                       $$ = new_int_expr(val);
                                                       if (!is_error)
                                                            myAST.buildASTRoot(OPS::MOD);    
                                                  }
                                                  else 
                                                  {
                                                       is_error = 1;
                                                       std::cout << "Error at line " << yylineno << " Incompatible types: " << $1->type_name << " " << $3->type_name << "\n";
                                                       YYERROR;
                                                  }
                                             }    
               | '(' EXPRESSION ')'               {$$ = $2;}
               | EXPRESSION   LT   EXPRESSION     {
                                                       if ($1->type == 1 && $3->type == 1)
                                                       {
                                                            int val = ($1->int_value < $3->int_value);
                                                            $$ = new_bool_expr(val);
                                                            if (!is_error)
                                                                 myAST.buildASTRoot(OPS::LESSTHAN);
                                                       }
                                                       else if ($1->type == 4 && $3->type == 4)
                                                       {
                                                            int val = ($1->float_value < $3->float_value);
                                                            $$ = new_bool_expr(val);
                                                            if (!is_error)
                                                                 myAST.buildASTRoot(OPS::LESSTHAN);
                                                       }
                                                       else 
                                                       {
                                                            is_error = 1;
                                                            std::cout << "Error at line " << yylineno << " Incompatible types: " << $1->type_name << " " << $3->type_name << "\n";
                                                            YYERROR;
                                                       }
                                                  }  
               | EXPRESSION   LE   EXPRESSION     {
                                                       if ($1->type == 1 && $3->type == 1)
                                                       {
                                                            int val = ($1->int_value <= $3->int_value);
                                                            $$ = new_bool_expr(val);
                                                            if (!is_error)
                                                                 myAST.buildASTRoot(OPS::LESSEQTHAN);
                                                       }
                                                       else if ($1->type == 4 && $3->type == 4)
                                                       {
                                                            int val = ($1->float_value <= $3->float_value);
                                                            $$ = new_bool_expr(val);
                                                            if (!is_error)
                                                                 myAST.buildASTRoot(OPS::LESSEQTHAN);
                                                       }
                                                       else 
                                                       {
                                                            is_error = 1;
                                                            std::cout << "Error at line " << yylineno << " Incompatible types: " << $1->type_name << " " << $3->type_name << "\n";
                                                            YYERROR;
                                                       }
                                                  } 
               | EXPRESSION   GT   EXPRESSION     {
                                                       if ($1->type == 1 && $3->type == 1)
                                                       {
                                                            int val = ($1->int_value > $3->int_value);
                                                            $$ = new_bool_expr(val);
                                                            if (!is_error)
                                                                 myAST.buildASTRoot(OPS::GREATERTHAN);
                                                       }
                                                       else if ($1->type == 4 && $3->type == 4)
                                                       {
                                                            int val = ($1->float_value > $3->float_value);
                                                            $$ = new_bool_expr(val);
                                                            if (!is_error)
                                                                 myAST.buildASTRoot(OPS::GREATERTHAN);
                                                       }
                                                       else 
                                                       {
                                                            is_error = 1;
                                                            std::cout << "Error at line " << yylineno << " Incompatible types: " << $1->type_name << " " << $3->type_name << "\n";
                                                            YYERROR;
                                                       }
                                                  } 
               | EXPRESSION   GE   EXPRESSION     {
                                                       if ($1->type == 1 && $3->type == 1)
                                                       {
                                                            int val = ($1->int_value >= $3->int_value);
                                                            $$ = new_bool_expr(val);
                                                            if (!is_error)
                                                                 myAST.buildASTRoot(OPS::GREATEREQTHAN);
                                                       }
                                                       else if ($1->type == 4 && $3->type == 4)
                                                       {
                                                            int val = ($1->float_value >= $3->float_value);
                                                            $$ = new_bool_expr(val);
                                                            if (!is_error)
                                                                 myAST.buildASTRoot(OPS::GREATEREQTHAN);
                                                       }
                                                       else 
                                                       {
                                                            is_error = 1;
                                                            std::cout << "Error at line " << yylineno << " Incompatible types: " << $1->type_name << " " << $3->type_name << "\n";
                                                            YYERROR;
                                                       }
                                                  }  
               | EXPRESSION   EQ   EXPRESSION     {
                                                       if ($1->type == 1 && $3->type == 1)
                                                       {
                                                            int val = ($1->int_value == $3->int_value);
                                                            $$ = new_bool_expr(val);
                                                            if (!is_error)
                                                                 myAST.buildASTRoot(OPS::EQUAL);
                                                       }
                                                       else if ($1->type == 4 && $3->type == 4)
                                                       {
                                                            int val = ($1->float_value == $3->float_value);
                                                            $$ = new_bool_expr(val); 
                                                            if (!is_error)
                                                                 myAST.buildASTRoot(OPS::EQUAL);
                                                       }
                                                       else if ($1->type == 5 && $3->type == 5)
                                                       {
                                                            int val = ($1->int_value == $3->int_value);
                                                            $$ = new_bool_expr(val);
                                                            if (!is_error)
                                                                 myAST.buildASTRoot(OPS::EQUAL);
                                                       }
                                                       else 
                                                       {
                                                            is_error = 1;
                                                            std::cout << "Error at line " << yylineno << " Incompatible types: " << $1->type_name << " " << $3->type_name << "\n";
                                                            YYERROR;
                                                       }
                                                  }
               | EXPRESSION   NEQ   EXPRESSION    {
                                                       if ($1->type == 1 && $3->type == 1)
                                                       {
                                                            int val = ($1->int_value != $3->int_value);
                                                            $$ = new_bool_expr(val);
                                                            if (!is_error)
                                                                 myAST.buildASTRoot(OPS::NOTEQUAL);
                                                       }
                                                       else if ($1->type == 4 && $3->type == 4)
                                                       {
                                                            int val = ($1->float_value != $3->float_value);
                                                            $$ = new_bool_expr(val);
                                                            if (!is_error)
                                                                 myAST.buildASTRoot(OPS::NOTEQUAL);
                                                       }
                                                       else if ($1->type == 5 && $3->type == 5)
                                                       {
                                                            int val = ($1->int_value != $3->int_value);
                                                            $$ = new_bool_expr(val);
                                                            if (!is_error)
                                                                 myAST.buildASTRoot(OPS::NOTEQUAL);
                                                       }
                                                       else 
                                                       {
                                                            is_error = 1;
                                                            std::cout << "Error at line " << yylineno << " Incompatible types: " << $1->type_name << " " << $3->type_name << "\n";
                                                            YYERROR;
                                                       }
                                                  }   
               | NOT_OP EXPRESSION                { 
                                                       if ($2->type == 5)
                                                       {
                                                            int val = !($2->int_value);
                                                            $$ = new_bool_expr(val);
                                                            if (!is_error)
                                                                 myAST.buildASTRoot(OPS::NOT);
                                                       }
                                                       else 
                                                       {
                                                            is_error = 1;
                                                            std::cout << "Error at line " << yylineno << " Incompatible type: " << $2->type_name << "\n";
                                                            YYERROR;
                                                       }
                                                  }        
               | INT_VAL                          {   
                                                       if(!is_error) 
                                                       {
                                                            struct root_data* r_data = new struct root_data;
                                                            r_data->number_int = $1;
                                                            struct node* current_node = myAST.buildAST(r_data, nullptr, nullptr, NUMBER_INT);
                                                            myAST.nodes_stack[myAST.nodes_stack_cnt++] = current_node;
                                                            $$ = new_int_expr($1);
                                                       }
                                                  }
               | FLOAT_VAL 	                    {    
                                                       if(!is_error) 
                                                       {
                                                            struct root_data* r_data = new struct root_data;
                                                            r_data->number_float = $1;
                                                            struct node* current_node = myAST.buildAST(r_data, nullptr, nullptr, NUMBER_FLOAT);
                                                            myAST.nodes_stack[myAST.nodes_stack_cnt++] = current_node;
                                                            $$ = new_float_expr($1); 
                                                       }
                                                  }
               | CHAR_VAL  	                    { $$ = new_char_expr($1); }
               | STRING_VAL	                    { $$ = new_string_expr($1); }
               | BOOL_VAL  	                    { 
                                                       if(!is_error) 
                                                       {
                                                            struct root_data* r_data = new struct root_data;
                                                            r_data->number_int = $1;
                                                            struct node* current_node = myAST.buildAST(r_data, nullptr, nullptr, NUMBER_BOOL);
                                                            myAST.nodes_stack[myAST.nodes_stack_cnt++] = current_node;
                                                            $$ = new_bool_expr($1);  
                                                       }
                                                  }
               | ID                               {    struct expr* get_id_data = symbolTable.search_by_name($1);
                                                       if (get_id_data != nullptr)
                                                       {
                                                            if (get_id_data->type == 1 && !is_error)
                                                            {
                                                                 struct root_data* r_data = new struct root_data;
                                                                 r_data->expr_ptr = get_id_data;
                                                                 struct node* current_node =  myAST.buildAST(r_data, nullptr, nullptr, IDENTIFIER_INT);
                                                                 myAST.nodes_stack[myAST.nodes_stack_cnt++] = current_node;
                                                            }
                                                            if (get_id_data->type == 4 && !is_error)
                                                            {
                                                                 struct root_data* r_data = new struct root_data;
                                                                 r_data->expr_ptr = get_id_data;
                                                                 struct node* current_node =  myAST.buildAST(r_data, nullptr, nullptr, IDENTIFIER_FLOAT);
                                                                 myAST.nodes_stack[myAST.nodes_stack_cnt++] = current_node;
                                                            }
                                                            if (get_id_data->type == 5 && !is_error)
                                                            {
                                                                 struct root_data* r_data = new struct root_data;
                                                                 r_data->expr_ptr = get_id_data;
                                                                 struct node* current_node =  myAST.buildAST(r_data, nullptr, nullptr, IDENTIFIER_BOOL);
                                                                 myAST.nodes_stack[myAST.nodes_stack_cnt++] = current_node;
                                                            }
                                                            $$ = get_id_data;
                                                       }
                                                       else {
                                                            is_error = true;
                                                            std::cout << "Error at line " << yylineno << " Variable " << $1 << " has not been declared\n";
                                                            YYERROR;
                                                       }
                                                       free($1);
                                                  }
               | call_function                    {   if (!is_error) 
                                                       {
                                                            struct root_data* r_data = new struct root_data;
                                                            r_data->unknown = strdup($1->name);
                                                            struct node* current_node =  myAST.buildAST(r_data, nullptr, nullptr, UNKNOWN);
                                                            myAST.nodes_stack[myAST.nodes_stack_cnt++] = current_node;
                                                       }
                                                       $$ = $1;
                                                  }
               | array_id_label                   {    if($1 != nullptr) {
                                                            if (!is_error) {
                                                                 struct root_data* r_data = new struct root_data;
                                                                 r_data->expr_ptr = $1;
                                                                 struct node* current_node;
                                                                 switch ($1->type)
                                                                 {
                                                                      case 1:
                                                                           current_node =  myAST.buildAST(r_data, nullptr, nullptr, IDENTIFIER_INT);
                                                                           break;
                                                                      case 4:
                                                                           current_node =  myAST.buildAST(r_data, nullptr, nullptr, IDENTIFIER_FLOAT);
                                                                           break;
                                                                      case 5:
                                                                           current_node =  myAST.buildAST(r_data, nullptr, nullptr, IDENTIFIER_BOOL);
                                                                           break;
                                                                 }
                                                                 myAST.nodes_stack[myAST.nodes_stack_cnt++] = current_node;
                                                            }
                                                            $$ = $1;
                                                       }
                                                  }
               ;

functions      : functions function 
               | function 
               ;

type_fn        : type_var                         { $$ = $1;}
               | VOID                             {$$ = $1;}
               ;

fn_name        : ID_FUNCT                         {$$ = $1; pushScope($1);}
               ;

function       : type_fn fn_name   '('')' '{' block_instr '}'              {    if (!functionTable.empty_fn($2))
                                                                                     functionTable.create_fn($2, $1, 1);
                                                                                else
                                                                                {
                                                                                     std::cout << "Error at line " << yylineno << " The function " << $1 << "has already been declared\n";
                                                                                     YYERROR;
                                                                                }
                                                                                popScope();
                                                                           } 

               | type_fn fn_name '(' params_list ')' '{' block_instr '}'   {    if (functionTable.exists_fn($2, functionTable.call_cnt) == nullptr)
                                                                                     functionTable.create_fn($2, $1, 0);
                                                                                else
                                                                                {
                                                                                     std::cout << "Error at line " << yylineno << " The function " << $1 << "has already been declared\n";
                                                                                     YYERROR;
                                                                                }
                                                                                functionTable.params_cnt = 0;
                                                                                popScope();
                                                                           }
               ;

params_list    : param                                                     {    functionTable.params[functionTable.params_cnt] = $1;
                                                                                functionTable.params_cnt++;
                                                                           }
               | params_list ',' param                                     {    functionTable.params[functionTable.params_cnt] = $3;
                                                                                functionTable.params_cnt++;
                                                                           }
               ;

param          : type_var ID                                               {    struct expr* val = new struct expr;
                                                                                strcpy(val->name, $2);
                                                                                strcpy(val->type_name, $1);
                                                                                val->type = find_type($1);
                                                                                $$ = val;

                                                                           }



%%
void yyerror(const char * s){
printf("error: %s at line:%d\n",s,yylineno);
}

int main(int argc, char** argv){
     yyin=fopen(argv[1],"r");
     yyparse();
     symbolTable.table_symbol_display();
     functionTable.table_function_display();
     std::cout << myAST.get_size();
     symbolTable.dellocEverything();
} 