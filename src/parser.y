%require "3.0.4"
%define parse.trace 
%define parse.error verbose
%define api.token.prefix {T_} 

%{
#include "SymbolTable.hpp"
#include "Utilities.hpp"
#define YYSTYPE Data
#include <iostream>
extern FILE *yyin;
extern int yylineno;  // z lex-a
int yylex();
int yyerror(char*);
int yyerror(const char*);

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
declarations: declarations PIDIDENTIFIER SEMICOLON {}
    | declarations PIDIDENTIFIER LEFT_BRACKET NUMBER TABLE_RANGE NUMBER RIGHT_BRACKET SEMICOLON {}
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
    | READ identifier SEMICOLON {}
    | WRITE value SEMICOLON {}
;
expression: value {}
    | value ADDITION value {}
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
identifier: PIDIDENTIFIER {}
    | PIDIDENTIFIER LEFT_BRACKET PIDIDENTIFIER RIGHT_BRACKET {}
    | PIDIDENTIFIER LEFT_BRACKET NUMBER RIGHT_BRACKET {}
;
%%

int yyerror(char *s)
{
    std::cerr << "Linia " << yylineno << ": " << s << std::endl;
    return 1;
}

int yyerror(const char *s)
{
    std::cerr << "Linia " << yylineno << ": " << s << std::endl;
    return 1;
}


