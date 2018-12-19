%require "3.0.4"
%define parse.trace 
%define parse.error verbose
%define api.token.prefix {T_} 

%{
#include <iostream>
#include "Driver.hpp"
#include "Utilities.hpp"
#define YYSTYPE Data

extern FILE *yyin;
extern int yylineno;
int yylex();
int yyerror(char*);
int yyerror(const char*);
Driver d;

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
program: DECLARE declarations IN commands END
    {
        d.TAC.addNewCode("HALT");
    }
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
        d.TAC.handleMathOperation($1.name);
    }
    | IF condition THEN commands elseProduction commands ENDIF 
    {
        d.TAC.endIf();
    }
    | IF condition THEN commands ENDIF 
    {
        d.TAC.endIf();
    }
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
elseProduction: ELSE
{
    d.TAC.addJump();
    d.TAC.endIf();
    d.TAC.swap();
};

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
            handleOperation(d, "ADD", $1, $3);           
        }
    | value SUBTRACTION value
        {
            handleOperation(d, "SUB", $1, $3);           
        }    
    | value MULTIPLICATION value
        {
            handleOperation(d, "MUL", $1, $3);            
        }
    | value DIVISION value 
        {
            handleOperation(d, "DIV", $1, $3);            
        }
    | value MODULO value 
        {
            handleOperation(d, "MOD", $1, $3);            
        }
;
condition: value EQUAL value 
    {    
        handleConditionOperation(d, "JNE", $1, $3);
    }
    | value NOT_EQUAL value 
    {
        handleConditionOperation(d, "JEQ", $1, $3);
    }
    | value LESS value 
    {
        handleConditionOperation(d, "JME", $1, $3);
    }
    | value MORE value     
    {
        handleConditionOperation(d, "JLE", $1, $3);
    }
    | value LESS_EQUAL value 
    {
        handleConditionOperation(d, "JMR", $1, $3);
    }
    | value MORE_EQUAL value 
    {
        handleConditionOperation(d, "JLS", $1, $3);
    }
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


