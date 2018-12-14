%option noyywrap
%option yylineno
%{
#include "Parser.hpp"
#include <string>
#include <iostream>
int yylex();
%}
%x COMMENT

%%

[ \t] ;
\\\n ;

^\# {
      BEGIN(COMMENT);
}

<COMMENT>\\\n ;
<COMMENT>.  ;
<COMMENT>\n {
      BEGIN(INITIAL);
}

\( {
      return T_LEFT_BRACKET;
}
\) {
      return T_RIGHT_BRACKET;
}

[0-9]+ {
      yylval = std::stoi(yytext);
      return T_NUMBER;
}

\+ {
      return T_ADDITION;
}

\- {
      return T_SUBTRACTION;
}

\% {
      return T_MODULO;
}

\* {
      return T_MULTIPLICATION;
}

\/ {
      return T_DIVISION;
}

\^ {
      return T_POWER;
}

\n	{ 
      return T_END; 
}

.	{ 
      return T_ERROR; 
   }


%%


void suppressor(){
      char c = 'a';
      yyunput(c, &c);
}
