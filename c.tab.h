
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
     ATIDENTIFIER = 264,
     SIZEOF = 265,
     TYPENAME = 266,
     TYPEOF = 267,
     TYPEDEF = 268,
     DEFINE = 269,
     DEFLOC = 270,
     DEFREC = 271,
     DEFSTX = 272,
     CONTAINEROF = 273,
     PTR_OP = 274,
     INC_OP = 275,
     DEC_OP = 276,
     LEFT_OP = 277,
     RIGHT_OP = 278,
     LE_OP = 279,
     GE_OP = 280,
     EQ_OP = 281,
     NE_OP = 282,
     AND_OP = 283,
     OR_OP = 284,
     MUL_ASSIGN = 285,
     DIV_ASSIGN = 286,
     MOD_ASSIGN = 287,
     ADD_ASSIGN = 288,
     SUB_ASSIGN = 289,
     LEFT_ASSIGN = 290,
     RIGHT_ASSIGN = 291,
     AND_ASSIGN = 292,
     XOR_ASSIGN = 293,
     OR_ASSIGN = 294,
     CAST_ASSIGN = 295,
     XCAST_ASSIGN = 296,
     GOTO = 297,
     GLOBAL = 298,
     LOCAL = 299,
     LAMBDA = 300,
     NAMES = 301,
     LET = 302,
     LAPPLY = 303,
     BOOL = 304,
     CHAR = 305,
     SHORT = 306,
     INT = 307,
     LONG = 308,
     SIGNED = 309,
     UNSIGNED = 310,
     FLOAT = 311,
     DOUBLE = 312,
     VOID = 313,
     STRUCT = 314,
     UNION = 315,
     ENUM = 316,
     ELLIPSIS = 317,
     IF = 318,
     ELSE = 319,
     SWITCH = 320,
     WHILE = 321,
     DO = 322,
     FOR = 323,
     CONTINUE = 324,
     BREAK = 325,
     RETURN = 326,
     CASE = 327,
     DEFAULT = 328,
     SYNTAX = 329,
     SYNTAXID = 330,
     SYNTAXQUOTE = 331,
     SYNTAXQUASI = 332,
     SYNTAXUNQUOTE = 333,
     SYNTAXSPLICE = 334,
     LPAIR = 335,
     RPAIR = 336,
     NOBIND_PRE = 337,
     MATCH = 338
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
#line 148 "c.tab.h"
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







