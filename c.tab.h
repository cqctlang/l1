
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
     DATA = 264,
     ATIDENTIFIER = 265,
     SYNTAXID = 266,
     SIZEOF = 267,
     TYPENAME = 268,
     TYPEOF = 269,
     TYPEDEF = 270,
     DEFINE = 271,
     DEFLOC = 272,
     DEFREC = 273,
     DEFSTX = 274,
     CONTAINEROF = 275,
     PTR_OP = 276,
     INC_OP = 277,
     DEC_OP = 278,
     LEFT_OP = 279,
     RIGHT_OP = 280,
     LE_OP = 281,
     GE_OP = 282,
     EQ_OP = 283,
     NE_OP = 284,
     AND_OP = 285,
     OR_OP = 286,
     MUL_ASSIGN = 287,
     DIV_ASSIGN = 288,
     MOD_ASSIGN = 289,
     ADD_ASSIGN = 290,
     SUB_ASSIGN = 291,
     LEFT_ASSIGN = 292,
     RIGHT_ASSIGN = 293,
     AND_ASSIGN = 294,
     XOR_ASSIGN = 295,
     OR_ASSIGN = 296,
     CAST_ASSIGN = 297,
     XCAST_ASSIGN = 298,
     GOTO = 299,
     GLOBAL = 300,
     LOCAL = 301,
     LAMBDA = 302,
     NAMES = 303,
     LET = 304,
     LAPPLY = 305,
     BOOL = 306,
     CHAR = 307,
     SHORT = 308,
     INT = 309,
     LONG = 310,
     SIGNED = 311,
     UNSIGNED = 312,
     FLOAT = 313,
     DOUBLE = 314,
     VOID = 315,
     STRUCT = 316,
     UNION = 317,
     ENUM = 318,
     ELLIPSIS = 319,
     IF = 320,
     ELSE = 321,
     SWITCH = 322,
     WHILE = 323,
     DO = 324,
     FOR = 325,
     CONTINUE = 326,
     BREAK = 327,
     RETURN = 328,
     CASE = 329,
     DEFAULT = 330,
     SYNTAXQUOTE = 331,
     SYNTAXQUASI = 332,
     SYNTAXUNQUOTE = 333,
     SYNTAXSPLICE = 334,
     SYNTAXLIST = 335,
     LPAIR = 336,
     RPAIR = 337,
     NOBIND_PRE = 338,
     MATCH = 339,
     ATFILE = 340,
     ATLINE = 341
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
#line 151 "c.tab.h"
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







