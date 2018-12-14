%require "3.0.4"
%define parse.trace 
%define parse.error verbose
%define api.token.prefix {T_} 

%{
#include <iostream>
#include <string>
#include <math.h>

#include "Logger.hpp"

extern int yylex();
extern int yylineno;

std::string reversePolishNotation;
std::string errorMessage;

int yyerror(std::string str)
{
    std::cout<<str<<" at line: "<<yylineno<<std::endl;
    return 0;
}

%}
%token NUMBER
%token LEFT_BRACKET
%token RIGHT_BRACKET
%token END
%token ERROR

%left ADDITION SUBTRACTION
%left MODULO MULTIPLICATION DIVISION
%precedence NEG
%right POWER


%%
input:
    | input line
;

line: exp END
    { 
        if (errorMessage.empty())
        {
            reversePolishNotation.pop_back();
            std::cout<<reversePolishNotation<<std::endl;
            std::cout<<"Wynik: "<<$$<<std::endl;
        }
        else
        {
            std::cout<<errorMessage<<std::endl;
            errorMessage= "";
        }
        reversePolishNotation.clear();
    }
    | error END
    { 
        reversePolishNotation.clear();
    }
;

exp:
    NUMBER
    {
        $$ = $1;
        reversePolishNotation += (std::to_string($1) + ' ');
    }
    | exp ADDITION exp
    {
        $$ = $1 + $3;
        reversePolishNotation += "+ ";
    }
    | exp SUBTRACTION exp
    {
        reversePolishNotation += "- ";
        $$ = $1 - $3;
    }
    | exp MULTIPLICATION exp
    {
        reversePolishNotation += "* ";
        $$ = $1 * $3;
    }
    | exp DIVISION exp
    {
        if ($3 == 0)
        {
            errorMessage = "ERROR: DIVISION BY 0";
        }
        else
        {
            $$ = (int)floor((float) $1 / $3);
            reversePolishNotation += "/ ";
        }
    }
    | exp POWER exp
    {
        reversePolishNotation += "^ ";        
        $$ = pow($1, $3);
    }
    | LEFT_BRACKET exp RIGHT_BRACKET
    {
        $$ = $2;
    }
    | SUBTRACTION exp %prec NEG
    {
        $$ = -$2;
        size_t position = reversePolishNotation.find_last_of(' ', reversePolishNotation.length());
        reversePolishNotation.insert(position, "~");
    }
    | exp MODULO exp
    {
        reversePolishNotation += "% "; 
        if ($3 == 0)
        {
            errorMessage = "ERROR: MODULO BY 0";
        }
        else if ($1 < 0)
        {
            int x = $1 % $3;
            if ($3 > 0)
            {
                x = abs(x);
                $$ = abs($3) - x;
            }
            else
            {
                $$ = x;
            }
        }
        else
        {
            $$ = $1 % $3;
        }
    }
;



%%
