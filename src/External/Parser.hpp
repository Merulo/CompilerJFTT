/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_SRC_EXTERNAL_PARSER_HPP_INCLUDED
# define YY_YY_SRC_EXTERNAL_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    T_DECLARE = 258,
    T_IN = 259,
    T_END = 260,
    T_LEFT_BRACKET = 261,
    T_RIGHT_BRACKET = 262,
    T_TABLE_RANGE = 263,
    T_NUMBER = 264,
    T_PIDIDENTIFIER = 265,
    T_ASSIGN = 266,
    T_IF = 267,
    T_THEN = 268,
    T_ELSE = 269,
    T_ENDIF = 270,
    T_WHILE = 271,
    T_DO = 272,
    T_ENDWHILE = 273,
    T_ENDDO = 274,
    T_FOR = 275,
    T_FROM = 276,
    T_TO = 277,
    T_ENDFOR = 278,
    T_DOWNTO = 279,
    T_READ = 280,
    T_WRITE = 281,
    T_ADDITION = 282,
    T_SUBTRACTION = 283,
    T_MULTIPLICATION = 284,
    T_DIVISION = 285,
    T_MODULO = 286,
    T_EQUAL = 287,
    T_NOT_EQUAL = 288,
    T_LESS = 289,
    T_MORE = 290,
    T_LESS_EQUAL = 291,
    T_MORE_EQUAL = 292,
    T_SEMICOLON = 293,
    T_ERROR = 294
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SRC_EXTERNAL_PARSER_HPP_INCLUDED  */
