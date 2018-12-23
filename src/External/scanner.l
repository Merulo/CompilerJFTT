%option noyywrap
%option yylineno
%option nounput
%{
#include "Utilities.hpp"
#define YYSTYPE Data
#include "Parser.hpp"
int yylex();
%}

%x COMMENT

%%
DECLARE     { return T_DECLARE; }
IN          { return T_IN; }
END         { return T_END; }

\(          { return T_LEFT_BRACKET; }
\)          { return T_RIGHT_BRACKET; }
\:          { return T_TABLE_RANGE; }
[0-9]+      {
                Data d;
                d.value = atoll(yytext);
                yylval = d;
                return T_NUMBER;
            }
[_a-z]+     {
                Data d;
                d.name = yytext;
                yylval = d;
                return T_PIDIDENTIFIER;
            }

\:\=        { return T_ASSIGN; }
IF          { return T_IF; }
THEN        { return T_THEN; }
ELSE        { return T_ELSE; }
ENDIF       { return T_ENDIF; }
WHILE       { return T_WHILE; }
DO          { return T_DO; }
ENDWHILE    { return T_ENDWHILE; }
ENDDO       { return T_ENDDO; }
FOR         { return T_FOR; }
FROM        { return T_FROM; }
TO          { return T_TO; }
ENDFOR      { return T_ENDFOR; }
DOWNTO      { return T_DOWNTO; }
READ        { return T_READ; }
WRITE       { return T_WRITE; }

\+          { return T_ADDITION; }
\-          { return T_SUBTRACTION; }
\*          { return T_MULTIPLICATION; }
\/          { return T_DIVISION; }
\%          { return T_MODULO; }

\=          { return T_EQUAL; }
\!\=        { return T_NOT_EQUAL; }
\<          { return T_LESS; }
\>          { return T_MORE; }
\<\=        { return T_LESS_EQUAL; }
\>\=        { return T_MORE_EQUAL; }

\;          { return T_SEMICOLON; }

[ \t]       ;
\n          ;

\[          { BEGIN(COMMENT); }
<COMMENT>\] { BEGIN(INITIAL); }
<COMMENT>.  ;

.           { 
                Data d;
                d.name = yytext;
                yylval = d;
                return T_ERROR; 
            };

%%

