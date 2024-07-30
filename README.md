# LFAC


## :ledger: Index
- [Description](#beginner-description)
- [Setup](#zap-setup)
- [Syntax](#scroll-syntax)
- [Functionality](#white_check_mark-functionality)
- [Team](#team)

## :beginner: Description

Welcome to our project! This is a compiler made using Yacc and C++, that will parse a text file, validate the syntax and compile the code.


## :zap: Setup
To run the compiler.sh shell script on a Linux system, you will need to have the following tools installed:
- Bison
- Flex
- G++

If you have a Debian-based system, run the following commands in order to install the latest versions:

```
sudo apt-get update
sudo apt-get install bison flex g++
```

compiler.sh is responsible with generating the executable. In order to accomplish that, run the following command:
```
./compiler.sh language
```

Afterwards, run the executable on the text file containing valid instructions.
```
./language file.txt
```

You can now edit the text file or create your own, and write your own input class.

## :scroll: Syntax
Statements’ ends are indicated using a semicolon.

Variable and class names start with an octothorpe (#). The next character will need to be a letter.


Function names begin with ```fn_```. The main function will need to begin with the following line: ```fn_main```, and end with ```fn_main_end```.

Function return types must have dollar signs to their left and right ($).




Valid programs are to be structured in the following order: declarations, classes, functions, block. Variables must be accessible in the current scope.

- Declarations: 
  - Available variable types: integer, char, float, boolean, string, and char.
  - Variables can be intialized in this part, but it's not required.
  - Declaring a variable: ```<const> <type> #<name>;```
  - Declaring an array: 
    - One-dimensional: ```#<name>[<size>]``` 
    - Two-dimensional: ```#<name>[<size1>][<size2>]```

- Classes:
  - Declaring a class (example):
  ```
  class #classtest {
  	public:
  	integer #memberint;
  	float #memberfloat;
  	integer #memberarray[4];
  	char #memberchar;
  	string #memberstring;
  	$integer$ fn_classtest_int(integer #a1)
  	{
  		char #testus;
  		#testus := 'v';
  		#memberint := 111;
  		return 8;
  	}
  }
  ```

- Functions:
  - Declaring a function:
    ```
    $<return type>$ fn_<name>(<arguments>)
    {
      <block>
      <return (if applies)>
    }
    ```


- Block:
  - initialization/ assignation: ```<name> := <value>;```
  - function calls: ```<fn_name>();```
  - accessing member variables: ```<class-name>.<name>```
  - binary and unary operations: +, -, /, *, <, >, ==, !=, <=, >=, &&, ||,  not 
  - evaluating and printing the value of an expression: ```Eval(<name>);```
  - loops:
      - For:
        ```
        for(<assign>:<condition>:<change>)
        {
          <block>
        }
        ```
    - While:
        ```
        while(<condition>)
        {
          <block>
        }
        ```
    - If:
      ```
      if(<condition>)
      {
        <block>
      }
      otherwise
      {
      <block>
      }
      ```
  

## :white_check_mark: Functionality

The grammar parser, language.y, defines the sequence of tokens.

The configuration file, such as language.l, defines the character sequences that are valid for our input.
The lex tool uses this configuration file to generate C source code. 

The shell script, compiler.sh, will first run yacc on our parser definition: ```bison -d $1.y```

Afterwards it will generate the source (lex.yy.c) by running lex on our lexical definition: ```lex $1.l```

The generated yacc and lex sources are then compiled using ```g++ program.cpp lex.yy.c  $1.tab.c -std=c++11 -o $1```



Whenever a valid file is compiled, our compiler will generate a symbol table for keeping track of all variables and classes: symbol_table.txt; as well as another table for functions.
Symbol_table keeps track of the name, type, whether it’s a constant, the scope, and the value.
Symbol_table_functions keeps track of function types, names, scopes, and parameters.

If the compilation occurs successfully, it will output ‘The programme is correct!’ in the console.

## Team

Rusu Andrei-Dudu,
Zloteanu Mircea

