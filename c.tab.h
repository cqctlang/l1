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
     SYMBOL = 259,
     CONSTANT = 260,
     STRING_LITERAL = 261,
     CONST = 262,
     VOLATILE = 263,
     SIZEOF = 264,
     TYPENAME = 265,
     TYPEOF = 266,
     TYPEDEF = 267,
     DEFINE = 268,
     DEFLOC = 269,
     DEFREC = 270,
     CONTAINEROF = 271,
     PTR_OP = 272,
     INC_OP = 273,
     DEC_OP = 274,
     LEFT_OP = 275,
     RIGHT_OP = 276,
     LE_OP = 277,
     GE_OP = 278,
     EQ_OP = 279,
     NE_OP = 280,
     AND_OP = 281,
     OR_OP = 282,
     MUL_ASSIGN = 283,
     DIV_ASSIGN = 284,
     MOD_ASSIGN = 285,
     ADD_ASSIGN = 286,
     SUB_ASSIGN = 287,
     LEFT_ASSIGN = 288,
     RIGHT_ASSIGN = 289,
     AND_ASSIGN = 290,
     XOR_ASSIGN = 291,
     OR_ASSIGN = 292,
     CAST_ASSIGN = 293,
     XCAST_ASSIGN = 294,
     GOTO = 295,
     GLOBAL = 296,
     LOCAL = 297,
     LAMBDA = 298,
     NAMES = 299,
     LET = 300,
     LAPPLY = 301,
     BOOL = 302,
     CHAR = 303,
     SHORT = 304,
     INT = 305,
     LONG = 306,
     SIGNED = 307,
     UNSIGNED = 308,
     FLOAT = 309,
     DOUBLE = 310,
     VOID = 311,
     STRUCT = 312,
     UNION = 313,
     ENUM = 314,
     ELLIPSIS = 315,
     IF = 316,
     ELSE = 317,
     SWITCH = 318,
     WHILE = 319,
     DO = 320,
     FOR = 321,
     CONTINUE = 322,
     BREAK = 323,
     RETURN = 324,
     CASE = 325,
     DEFAULT = 326,
     QUOTE = 327,
     SYNTAXQUOTE = 328,
     SYNTAXQUASI = 329,
     SYNTAXUNQUOTE = 330,
     SYNTAXSPLICE = 331,
     LPAIR = 332,
     RPAIR = 333
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
/* Line 2616 of glr.c.  */
#line 150 "c.tab.h"
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



