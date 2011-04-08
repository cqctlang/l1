
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison GLR parsers in C
   
      Copyright (C) 2002, 2003, 2004, 2005, 2006 Free Software Foundation, Inc.
   
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
     TYPEOF = 265,
     TYPEDEF = 266,
     DEFINE = 267,
     DEFLOC = 268,
     DEFREC = 269,
     CONTAINEROF = 270,
     PTR_OP = 271,
     INC_OP = 272,
     DEC_OP = 273,
     LEFT_OP = 274,
     RIGHT_OP = 275,
     LE_OP = 276,
     GE_OP = 277,
     EQ_OP = 278,
     NE_OP = 279,
     AND_OP = 280,
     OR_OP = 281,
     MUL_ASSIGN = 282,
     DIV_ASSIGN = 283,
     MOD_ASSIGN = 284,
     ADD_ASSIGN = 285,
     SUB_ASSIGN = 286,
     LEFT_ASSIGN = 287,
     RIGHT_ASSIGN = 288,
     AND_ASSIGN = 289,
     XOR_ASSIGN = 290,
     OR_ASSIGN = 291,
     CAST_ASSIGN = 292,
     XCAST_ASSIGN = 293,
     GOTO = 294,
     GLOBAL = 295,
     LOCAL = 296,
     LAMBDA = 297,
     NAMES = 298,
     LET = 299,
     LAPPLY = 300,
     BOOL = 301,
     CHAR = 302,
     SHORT = 303,
     INT = 304,
     LONG = 305,
     SIGNED = 306,
     UNSIGNED = 307,
     FLOAT = 308,
     DOUBLE = 309,
     VOID = 310,
     STRUCT = 311,
     UNION = 312,
     ENUM = 313,
     ELLIPSIS = 314,
     IF = 315,
     ELSE = 316,
     SWITCH = 317,
     WHILE = 318,
     DO = 319,
     FOR = 320,
     CONTINUE = 321,
     BREAK = 322,
     RETURN = 323,
     CASE = 324,
     DEFAULT = 325,
     QUOTE = 326,
     SYNTAXQUOTE = 327,
     SYNTAXQUASI = 328,
     SYNTAXUNQUOTE = 329,
     SYNTAXSPLICE = 330
   };
#endif


#ifndef YYSTYPE
typedef union YYSTYPE
{

/* Line 2638 of glr.c  */
#line 11 "c.y"

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



/* Line 2638 of glr.c  */
#line 140 "c.tab.h"
} YYSTYPE;
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







