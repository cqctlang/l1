/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison GLR parsers in C

   Copyright (C) 2002, 2003, 2004, 2005, 2006 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDENTIFIER = 258,
     CONSTANT = 259,
     STRING_LITERAL = 260,
     CONST = 261,
     VOLATILE = 262,
     SIZEOF = 263,
     TYPEOF = 264,
     TYPEDEF = 265,
     DEFINE = 266,
     DEFLOCAL = 267,
     DEFREC = 268,
     CONTAINEROF = 269,
     PTR_OP = 270,
     INC_OP = 271,
     DEC_OP = 272,
     LEFT_OP = 273,
     RIGHT_OP = 274,
     LE_OP = 275,
     GE_OP = 276,
     EQ_OP = 277,
     NE_OP = 278,
     AND_OP = 279,
     OR_OP = 280,
     MUL_ASSIGN = 281,
     DIV_ASSIGN = 282,
     MOD_ASSIGN = 283,
     ADD_ASSIGN = 284,
     SUB_ASSIGN = 285,
     LEFT_ASSIGN = 286,
     RIGHT_ASSIGN = 287,
     AND_ASSIGN = 288,
     XOR_ASSIGN = 289,
     OR_ASSIGN = 290,
     CAST_ASSIGN = 291,
     XCAST_ASSIGN = 292,
     GOTO = 293,
     GLOBAL = 294,
     LOCAL = 295,
     LAMBDA = 296,
     NAMES = 297,
     LET = 298,
     LAPPLY = 299,
     CHAR = 300,
     SHORT = 301,
     INT = 302,
     LONG = 303,
     SIGNED = 304,
     UNSIGNED = 305,
     FLOAT = 306,
     DOUBLE = 307,
     VOID = 308,
     STRUCT = 309,
     UNION = 310,
     ENUM = 311,
     ELLIPSIS = 312,
     DEFCONST = 313,
     IF = 314,
     ELSE = 315,
     SWITCH = 316,
     WHILE = 317,
     DO = 318,
     FOR = 319,
     CONTINUE = 320,
     BREAK = 321,
     RETURN = 322,
     CASE = 323,
     DEFAULT = 324,
     OCONTAINEROF = 325,
     ODEFINE = 326,
     OLAMBDA = 327,
     OTYPEOF = 328
   };
#endif


/* Copy the first part of user declarations.  */
#line 1 "c.y"

#include "sys.h"
#include "util.h"
#include "syscqct.h"

extern int yylex();
extern char *yytext;



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE 
#line 11 "c.y"
{
	Expr *expr;
	struct {
		/* GLR may pick tokens from input stream more than
		   once; remembering length in char-based tokens
		   ensures that we get the same token each time,
		   regardless of state of yytext. */
		char *p;
		unsigned long len;
	} chars;
	int kind;
}
/* Line 2604 of glr.c.  */
#line 145 "c.tab.h"
	YYSTYPE;
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{

  char yydummy;

} YYLTYPE;
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;



