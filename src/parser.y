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

void checkForErrors(const std::string& str)
{
    if (!str.empty())
    {
        std::cout << "Error at line " << yylineno << ": " << str << std::endl;
        exit(1);
    }
}

%}
//programm
%token DECLARE IN END

//declarations
%token LEFT_BRACKET RIGHT_BRACKET TABLE_RANGE NUMBER PIDIDENTIFIER

//commands
%token ASSIGN IF THEN ELSE ENDIF WHILE DO ENDWHILE ENDDO FOR FROM TO ENDFOR DOWNTO READ WRITE

//expression
%token ADDITION SUBTRACTION MULTIPLICATION DIVISION MODULO

//condition
%token EQUAL NOT_EQUAL LESS MORE LESS_EQUAL MORE_EQUAL

//rest
%token SEMICOLON ERROR

%%
program: DECLARE declarations IN commands END {}
;
declarations: declarations PIDIDENTIFIER SEMICOLON 
    {
        checkForErrors(driver.symbolTable.addVariable($2.name));
    }
    | declarations PIDIDENTIFIER LEFT_BRACKET NUMBER TABLE_RANGE NUMBER RIGHT_BRACKET SEMICOLON
    {
        checkForErrors(driver.symbolTable.addTable($2.name, $4.value, $6.value));
    }
    |
;
commands: commands command {}
    | command {}
;
command: identifier ASSIGN expression SEMICOLON 
    {
        driver.symbolTable.setInitialized($1.name);
        if ($3.name.empty())
        {
            driver.threeAddressCode.addNewCode("CONST", $1.name, std::to_string($3.value));
        }
        else
        {
            driver.threeAddressCode.addAssignCode($1.name);
        }
    }
    | IF condition THEN commands ELSE commands ENDIF {}
    | IF condition THEN commands ENDIF {}
    | WHILE condition DO commands ENDWHILE {}
    | DO commands WHILE condition ENDDO {}
    | FOR PIDIDENTIFIER FROM value TO value DO commands ENDFOR {}
    | FOR PIDIDENTIFIER FROM value DOWNTO value DO commands ENDFOR {}
    | READ identifier SEMICOLON 
    {
        checkForErrors(driver.symbolTable.checkVariableExists($2.name));
        driver.threeAddressCode.addNewCode("READ", $2.name);
    }
    | WRITE value SEMICOLON
    {
        checkForErrors(driver.symbolTable.checkVariableExistsAndIsInitialized($1.name));
        driver.threeAddressCode.addNewCode("WRITE", $2.name);
    }
;
expression: value 
    {
        if (!$1.name.empty())
        {
            checkForErrors(driver.symbolTable.checkVariableExistsAndIsInitialized($1.name));
        }
        else
        {
            driver.threeAddressCode.loadLocalParameters(std::to_string($1.value));
        }
    }
    | value ADDITION value 
        {
            checkForErrors(driver.symbolTable.checkVariableExistsAndIsInitialized($1.name));
            checkForErrors(driver.symbolTable.checkVariableExistsAndIsInitialized($3.name));
            driver.threeAddressCode.loadLocalParameters("ADD", $1.name, $3.name);                
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
        checkForErrors(driver.symbolTable.checkVariableIsVariable($1.name));   
    }
    | PIDIDENTIFIER LEFT_BRACKET PIDIDENTIFIER RIGHT_BRACKET 
    {
        checkForErrors(driver.symbolTable.checkVariableIsTable($1.name));     
    }
    | PIDIDENTIFIER LEFT_BRACKET NUMBER RIGHT_BRACKET 
    {
        checkForErrors(driver.symbolTable.checkVariableIsTable($1.name));   
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


