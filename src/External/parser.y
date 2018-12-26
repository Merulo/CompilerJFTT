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
        d.FIR->addNewCode("HALT");
    }
;
declarations: declarations PIDIDENTIFIER SEMICOLON 
    {
        checkForErrors(d.ST->addVariable($2.name));
    }
    | declarations PIDIDENTIFIER LEFT_BRACKET NUMBER TABLE_RANGE NUMBER RIGHT_BRACKET SEMICOLON
    {
        checkForErrors(d.ST->addTable($2.name, $4.value, $6.value));
    }
    |
;
commands: commands command {}
    | command {}
;
command: identifier ASSIGN expression SEMICOLON 
    {
        d.ST->setInitialized($1.name);
        d.FIR->handleMathOperation($1.name);
    }
    | IF condition THEN commands elseProduction commands ENDIF 
    {
        d.FIR->endIf();
    }
    | IF condition THEN commands ENDIF 
    {
        d.FIR->endIf();
    }
    | WHILE condition DO commands ENDWHILE 
    {
        d.FIR->endWhileDo();
    }
    | DO {d.FIR->closeConditionBlock();} commands WHILE condition ENDDO 
    {
        d.FIR->endDoWhile();
    }
    | forProduction FROM value TO value doProduction commands ENDFOR 
    {
        d.FIR->insertFor($2.name, $4, $6, true);
        d.ST->removeIterator($2.name);
    }
    | forProduction FROM value DOWNTO value doProduction commands ENDFOR 
    {
        d.FIR->insertFor($2.name, $4, $6, false);
        d.ST->removeIterator($2.name);
    }
    | READ identifier SEMICOLON 
    {
        checkForErrors(d.ST->checkVariableExists($2.name));
        d.FIR->addNewCode("READ", $2.name);
    }
    | WRITE value SEMICOLON
    {
        if (!$2.name.empty())
        {
            checkForErrors(d.ST->checkVariableExistsAndIsInitialized($2.name));
            d.FIR->addNewCode("WRITE", $2.name);
        }
        else
        {
            d.FIR->addNewCode("WRITE", std::to_string($2.value));
        }
    }
;
doProduction: DO
{
    d.FIR->closeForBlock();
};

forProduction: FOR PIDIDENTIFIER
{
    if (d.ST->isNameTaken($2.name))
    {
        checkForErrors("For iterator shadows declarations");
    }
    d.ST->addToIterators($2.name);
};

elseProduction: ELSE
{
    d.FIR->endElse();
};

expression: value 
    {
        if (!$1.name.empty())
        {
            checkForErrors(d.ST->checkVariableExistsAndIsInitialized($1.name));
            d.FIR->setOperation("COPY");
            d.FIR->setFirstExtraParameter($1.name);
        }
        else
        {
            d.FIR->setFirstExtraParameter(std::to_string($1.value));
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
condition: value EQUAL value {d.FIR->closeConditionBlock();}
    {    
        handleConditionOperation(d, "JEQ", $1, $3);
    }
    | value NOT_EQUAL value {d.FIR->closeConditionBlock();}
    {
        handleConditionOperation(d, "JNE", $1, $3);
    }
    | value LESS value {d.FIR->closeConditionBlock();}
    {
        handleConditionOperation(d, "JLS", $1, $3);
    }
    | value MORE value {d.FIR->closeConditionBlock();}
    {
        handleConditionOperation(d, "JMR", $1, $3);
    }
    | value LESS_EQUAL value {d.FIR->closeConditionBlock();}
    {
        handleConditionOperation(d, "JLE", $1, $3);
    }
    | value MORE_EQUAL value {d.FIR->closeConditionBlock();}
    {
        handleConditionOperation(d, "JME", $1, $3);
    }
;
value: NUMBER {}
    | identifier {}
;
identifier: PIDIDENTIFIER 
    {
        checkForErrors(d.ST->checkVariableIsVariable($1.name));
    }
    | PIDIDENTIFIER LEFT_BRACKET PIDIDENTIFIER RIGHT_BRACKET 
    {
        checkForErrors(d.ST->checkVariableIsTable($1.name));
        $$.name = $1.name + "(" + $3.name + ")";
        checkForErrors(d.ST->checkVariableExistsAndIsInitialized($3.name));   
    }
    | PIDIDENTIFIER LEFT_BRACKET NUMBER RIGHT_BRACKET 
    {
        checkForErrors(d.ST->checkVariableIsTable($1.name));  
        $$.name = $1.name + "(" + std::to_string($3.value) + ")";       
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


