%{
#include <iostream>
#include <string>
#include <vector>
#include "program.h"
extern FILE* yyin;
extern char* yytext;
extern int yylineno;
extern int yylex();
void yyerror(const char * s);

const int NMAX = 1024;

bool is_error = false;

class Symbol symbols[NMAX];

%}
%union {
     std::string val_name;
     std::string data_type;
     int int_val;
     bool bool_val;
     float float_val;
     char char_val;
     std::string string_val;
     struct expr *ptr_expr;
}
%token BGIN END ASSIGN RETURN CONST 
%token IF ELSE WHILE FOR
%token LT LE GT GE EQ NEQ AND_OP OR_OP   
%token <int_val> INT_VAL
%token <bool_val> BOOL_VAL
%token <float_val> FLOAT_VAL  
%token <char_val> CHAR_VAL
%token <string_val> STRING_VAL
%token <val_name> ID
%token <val_name> ID_FUNCT
%token <data_type> VOID INT FLOAT CHAR STRING BOOL

%left OR_OP
%left AND_OP
%left EQ NEQ
%left LT LE GT GE
%left '+' '-' 
%left '*' '/' '%'
%left '(' ')' '[' ']' '{' '}'

%start progr

%%
progr: declarations block {printf("The programme is correct!\n");}
     ;

type_var       : INT 
               | FLOAT 
               | CHAR 
               | STRING 
               | BOOL
               ;

declarations   : declaration ';'                  {is_error = false;}
               | declarations declaration ';'     {is_error = false;}
               ;

declaration    : type_var ID
               ;


%%
void yyerror(const char * s){
printf("error: %s at line:%d\n",s,yylineno);
}

int main(int argc, char** argv){
     yyin=fopen(argv[1],"r");
     yyparse();
} 