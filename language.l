%{
#include <stdio.h>
#include <string.h>
#include "language.tab.h"
%}
%option noyywrap
%%
"fn_main"                           {yylval.val_name = strdup(yytext); return BGIN;}
"fn_main_end"                       {return END;}

"integer"                           {yylval.data_type = strdup(yytext); return INT;}
"float"                             {yylval.data_type = strdup(yytext); return FLOAT;}
"char"                              {yylval.data_type = strdup(yytext); return CHAR;}
"string"                            {yylval.data_type = strdup(yytext); return STRING;}
"boolean"                           {yylval.data_type = strdup(yytext); return BOOL;}
"void"                              {yylval.data_type = strdup(yytext); return VOID;}

"class"                             {return CLASS;}
"public"                            {return PUBLIC;}
"private"                           {return PRIVATE;}

"Eval"                              {return EVAL;}
"TypeOf"                            {return TYPEOF;}

"return"                            {return RETURN;}
"const"                             {return CONST;}

"if"                                {return IF;}
"otherwise"                         {return ELSE;}
"while"                             {return WHILE;}
"for"                               {return FOR;}
"<"                                 {return LT;}
"<="                                {return LE;}
">"                                 {return GT;}
">="                                {return GE;}
"=="                                {return EQ;}
"!="                                {return NEQ;}
"&&"                                {return AND_OP;}
"||"                                {return OR_OP;}
":="                                {return ASSIGN;}

#[_a-zA-Z][_a-zA-Z0-9]*             {yylval.val_name = strdup(yytext); return ID;}
fn_[_a-zA-Z][_a-zA-Z0-9]*           {yylval.val_name = strdup(yytext);	return ID_FUNCT;}

([-]?0|[-]?([1-9][0-9]*))           {yylval.int_val = atoi(yytext); 	return INT_VAL;}
f[-]?(0|([1-9][0-9]*)).[0-9]*       {char* tmp = strdup(yytext); tmp++;  yylval.float_val = atof(tmp);   	return FLOAT_VAL;}
\'.\'                               {yylval.char_val = yytext[1]; 	return CHAR_VAL;} 
\"([^"])*\"                         {   char* new_string = (char*)malloc(strlen(yytext)-2);
                                        char* iter = yytext + 1;
                                        char* dr = strrchr(yytext,'"');
                                        int cnt = 0;
                                        while(iter != dr){
                                            new_string[cnt] = *iter;
                                            cnt++;
                                            iter++;
                                        }
                                        new_string[cnt] = '\0';
                                        yylval.string_val = new_string;
                                        return STRING_VAL;
                                    }

"true"|"false"                      {   if(strcmp(yytext, "true") == 0)
                                            yylval.int_val = 1;
                                        else yylval.int_val = 0;
                                        return BOOL_VAL; 
                                    }
[ \t] ;
\n                                  {yylineno++;}
.                                   {return yytext[0];}
