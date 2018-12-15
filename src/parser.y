%require "3.0.4"
%define parse.trace 
%define parse.error verbose
%define api.token.prefix {T_} 

%{
#include "Driver.hpp"
#include "Utilities.hpp"
#define YYSTYPE Data
#include <iostream>

extern FILE *yyin;
extern int yylineno;  // z lex-a
int yylex();
int yyerror(char*);
int yyerror(const char*);
Driver driver;

%}
//programm
%token DECLARE
%token IN
%token END

//declarations
%token LEFT_BRACKET
%token RIGHT_BRACKET
%token TABLE_RANGE
%token NUMBER
%token PIDIDENTIFIER

//commands
%token ASSIGN
%token IF
%token THEN
%token ELSE
%token ENDIF
%token WHILE
%token DO
%token ENDWHILE
%token ENDDO
%token FOR
%token FROM
%token TO
%token ENDFOR
%token DOWNTO
%token READ
%token WRITE

//expression
%token ADDITION
%token SUBTRACTION
%token MULTIPLICATION
%token DIVISION
%token MODULO

//condition
%token EQUAL
%token NOT_EQUAL
%token LESS
%token MORE
%token LESS_EQUAL
%token MORE_EQUAL

//rest
%token SEMICOLON
%token ERROR

%%
program: DECLARE declarations IN commands END {}
;
declarations: declarations PIDIDENTIFIER SEMICOLON 
    {
        std::string str = driver.symbolTable.addVariable($2.name);
        if (!str.empty())
        {
            std::cout << "Error at line " << yylineno << ": " << str << std::endl;
            return 1;
        }
    }
    | declarations PIDIDENTIFIER LEFT_BRACKET NUMBER TABLE_RANGE NUMBER RIGHT_BRACKET SEMICOLON
    {
        std::string str = driver.symbolTable.addTable($2.name, $4.value, $6.value);
        if (!str.empty())
        {
            std::cout << "Error at line " << yylineno << ": " << str << std::endl;
            return 1;
        }        
    }
    |
;
commands: commands command {}
    | command {}
;
command: identifier ASSIGN expression SEMICOLON {}
    | IF condition THEN commands ELSE commands ENDIF {}
    | IF condition THEN commands ENDIF {}
    | WHILE condition DO commands ENDWHILE {}
    | DO commands WHILE condition ENDDO {}
    | FOR PIDIDENTIFIER FROM value TO value DO commands ENDFOR {}
    | FOR PIDIDENTIFIER FROM value DOWNTO value DO commands ENDFOR {}
    | READ identifier SEMICOLON 
    {
        std::string str = driver.symbolTable.checkVariableExists($2.name);
        if (!str.empty())
        {
            std::cout << "Error at line " << yylineno << ": " << str << std::endl;
            return 1;
        }  
    }
    | WRITE value SEMICOLON
    {
        std::string str = driver.symbolTable.checkVariableExistsAndIsInitialized($1.name);
        if (!str.empty())
        {
            std::cout << "Error at line " << yylineno << ": " << str << std::endl;
            return 1;
        }
    }
;
expression: value 
    {
        std::string str = driver.symbolTable.checkVariableExistsAndIsInitialized($1.name);
        if (!str.empty())
        {
            std::cout << "Error at line " << yylineno << ": " << str << std::endl;
            return 1;
        }
    }
    | value ADDITION value 
        {
            std::string str = driver.symbolTable.checkVariableExistsAndIsInitialized($1.name);
            if (!str.empty())
            {
                std::cout << "Error at line " << yylineno << ": " << str << std::endl;
                return 1;
            }
            str = driver.symbolTable.checkVariableExistsAndIsInitialized($3.name);
            if (!str.empty())
            {
                std::cout << "Error at line " << yylineno << ": " << str << std::endl;
                return 1;
            }                     
        }
    | value SUBTRACTION value {}
    | value MULTIPLICATION value {}
    | value DIVISION value {}
    | value MODULO value {}
;
condition: value EQUAL value {}
    | value NOT_EQUAL value {}
    | value LESS value {}
    | value MORE value {}
    | value LESS_EQUAL value {}
    | value MORE_EQUAL value {}
;
value: NUMBER {}
    | identifier {}
;
identifier: PIDIDENTIFIER 
    {
        std::string str = driver.symbolTable.checkVariableIsVariable($1.name);
        if (!str.empty())
        {
            std::cout << "Error at line " << yylineno << ": " << str << std::endl;
            return 1;
        }     
    }
    | PIDIDENTIFIER LEFT_BRACKET PIDIDENTIFIER RIGHT_BRACKET 
    {
        std::string str = driver.symbolTable.checkVariableIsTable($1.name);
        if (!str.empty())
        {
            std::cout << "Error at line " << yylineno << ": " << str << std::endl;
            return 1;
        }       
    }
    | PIDIDENTIFIER LEFT_BRACKET NUMBER RIGHT_BRACKET 
    {
        std::string str = driver.symbolTable.checkVariableIsTable($1.name);
        if (!str.empty())
        {
            std::cout << "Error at line " << yylineno << ": " << str << std::endl;
            return 1;
        }     
    }
;
%%

int yyerror(char *s)
{
    std::cout << "Line: " << yylineno << ": " << s << std::endl;
    return 1;
}

int yyerror(const char *s)
{
    std::cout << "Line: " << yylineno << ": " << s << std::endl;
    return 1;
}


