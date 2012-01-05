
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
     SYNTAXID = 265,
     SIZEOF = 266,
     TYPENAME = 267,
     TYPEOF = 268,
     TYPEDEF = 269,
     DEFINE = 270,
     DEFLOC = 271,
     DEFREC = 272,
     DEFSTX = 273,
     CONTAINEROF = 274,
     PTR_OP = 275,
     INC_OP = 276,
     DEC_OP = 277,
     LEFT_OP = 278,
     RIGHT_OP = 279,
     LE_OP = 280,
     GE_OP = 281,
     EQ_OP = 282,
     NE_OP = 283,
     AND_OP = 284,
     OR_OP = 285,
     MUL_ASSIGN = 286,
     DIV_ASSIGN = 287,
     MOD_ASSIGN = 288,
     ADD_ASSIGN = 289,
     SUB_ASSIGN = 290,
     LEFT_ASSIGN = 291,
     RIGHT_ASSIGN = 292,
     AND_ASSIGN = 293,
     XOR_ASSIGN = 294,
     OR_ASSIGN = 295,
     CAST_ASSIGN = 296,
     XCAST_ASSIGN = 297,
     GOTO = 298,
     GLOBAL = 299,
     LOCAL = 300,
     LAMBDA = 301,
     NAMES = 302,
     LET = 303,
     LAPPLY = 304,
     BOOL = 305,
     CHAR = 306,
     SHORT = 307,
     INT = 308,
     LONG = 309,
     SIGNED = 310,
     UNSIGNED = 311,
     FLOAT = 312,
     DOUBLE = 313,
     VOID = 314,
     STRUCT = 315,
     UNION = 316,
     ENUM = 317,
     ELLIPSIS = 318,
     IF = 319,
     ELSE = 320,
     SWITCH = 321,
     WHILE = 322,
     DO = 323,
     FOR = 324,
     CONTINUE = 325,
     BREAK = 326,
     RETURN = 327,
     CASE = 328,
     DEFAULT = 329,
     SYNTAXQUOTE = 330,
     SYNTAXQUASI = 331,
     SYNTAXUNQUOTE = 332,
     SYNTAXSPLICE = 333,
     LPAIR = 334,
     RPAIR = 335,
     NOBIND_PRE = 336,
     MATCH = 337
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
#line 147 "c.tab.h"
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







