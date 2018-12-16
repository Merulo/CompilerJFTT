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
Driver d;

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
        checkForErrors(d.ST.addVariable($2.name));
    }
    | declarations PIDIDENTIFIER LEFT_BRACKET NUMBER TABLE_RANGE NUMBER RIGHT_BRACKET SEMICOLON
    {
        checkForErrors(d.ST.addTable($2.name, $4.value, $6.value));
    }
    |
;
commands: commands command {}
    | command {}
;
command: identifier ASSIGN expression SEMICOLON 
    {
        d.ST.setInitialized($1.name);
        d.TAC.addAssignCode($1.name);
    }
    | IF condition THEN commands ELSE commands ENDIF {}
    | IF condition THEN commands ENDIF {}
    | WHILE condition DO commands ENDWHILE {}
    | DO commands WHILE condition ENDDO {}
    | FOR PIDIDENTIFIER FROM value TO value DO commands ENDFOR {}
    | FOR PIDIDENTIFIER FROM value DOWNTO value DO commands ENDFOR {}
    | READ identifier SEMICOLON 
    {
        checkForErrors(d.ST.checkVariableExists($2.name));
        d.TAC.addNewCode("READ", $2.name);
    }
    | WRITE value SEMICOLON
    {
        checkForErrors(d.ST.checkVariableExistsAndIsInitialized($1.name));
        d.TAC.addNewCode("WRITE", $2.name);
    }
;
expression: value 
    {
        if (!$1.name.empty())
        {
            checkForErrors(d.ST.checkVariableExistsAndIsInitialized($1.name));
            d.TAC.setOperation("COPY");
            d.TAC.setFirstExtraParameter($1.name);
        }
        else
        {
            d.TAC.setFirstExtraParameter(std::to_string($1.value));
        }
    }
    | value ADDITION value 
        {
            d.TAC.setOperation("ADD");
            if (!$1.name.empty())
            {
                checkForErrors(d.ST.checkVariableExistsAndIsInitialized($1.name));
                d.TAC.setFirstExtraParameter($1.name);
            }
            else
            {
                std::string reg = d.TAC.getRegister();
                d.TAC.addNewCode("CONST", reg , std::to_string($1.value));
                d.TAC.setFirstExtraParameter(reg);
            }
            if (!$3.name.empty())
            {
                checkForErrors(d.ST.checkVariableExistsAndIsInitialized($3.name));
                d.TAC.setSecondExtraParameter($3.name);
            }
            else
            {
                std::string reg = d.TAC.getRegister();
                d.TAC.addNewCode("CONST", reg , std::to_string($3.value));
                d.TAC.setSecondExtraParameter(reg);
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
        checkForErrors(d.ST.checkVariableIsVariable($1.name));   
    }
    | PIDIDENTIFIER LEFT_BRACKET PIDIDENTIFIER RIGHT_BRACKET 
    {
        checkForErrors(d.ST.checkVariableIsTable($1.name));     
    }
    | PIDIDENTIFIER LEFT_BRACKET NUMBER RIGHT_BRACKET 
    {
        checkForErrors(d.ST.checkVariableIsTable($1.name));   
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


