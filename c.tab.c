
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison GLR parsers in C
   
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

/* C GLR parser skeleton written by Paul Hilfinger.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "glr.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 0




/* Copy the first part of user declarations.  */

/* Line 172 of glr.c  */
#line 1 "c.y"

#include "sys.h"
#include "util.h"
#include "syscqct.h"

extern int yylex();
extern char *yytext;



/* Line 172 of glr.c  */
#line 70 "c.tab.c"



#include "c.tab.h"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* Default (constant) value used for initialization for null
   right-hand sides.  Unlike the standard yacc.c template,
   here we set the default value of $$ to a zeroed-out value.
   Since the default value is undefined, this behavior is
   technically correct.  */
static YYSTYPE yyval_default;

/* Copy the second part of user declarations.  */

/* Line 243 of glr.c  */
#line 82 "c.y"

	static void yyerror(U *ctx, const char *s);
	static Expr* castmerge(YYSTYPE e1, YYSTYPE e2);
	static Expr* mulmerge(YYSTYPE e1, YYSTYPE e2);
	static Expr* andmerge(YYSTYPE e1, YYSTYPE e2);
	static Expr* ofmerge(YYSTYPE e1, YYSTYPE e2);


/* Line 243 of glr.c  */
#line 114 "c.tab.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#ifndef YYFREE
# define YYFREE free
#endif
#ifndef YYMALLOC
# define YYMALLOC malloc
#endif
#ifndef YYREALLOC
# define YYREALLOC realloc
#endif

#define YYSIZEMAX ((size_t) -1)

#ifdef __cplusplus
   typedef bool yybool;
#else
   typedef unsigned char yybool;
#endif
#define yytrue 1
#define yyfalse 0

#ifndef YYSETJMP
# include <setjmp.h>
# define YYJMP_BUF jmp_buf
# define YYSETJMP(env) setjmp (env)
# define YYLONGJMP(env, val) longjmp (env, val)
#endif

/*-----------------.
| GCC extensions.  |
`-----------------*/

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5) || __STRICT_ANSI__)
#  define __attribute__(Spec) /* empty */
# endif
#endif


#ifdef __cplusplus
# define YYOPTIONAL_LOC(Name) /* empty */
#else
# define YYOPTIONAL_LOC(Name) Name __attribute__ ((__unused__))
#endif

#ifndef YYASSERT
# define YYASSERT(condition) ((void) ((condition) || (abort (), 0)))
#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  213
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2556

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  105
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  100
/* YYNRULES -- Number of rules.  */
#define YYNRULES  319
/* YYNRULES -- Number of states.  */
#define YYNSTATES  577
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule.  */
#define YYMAXRHS 9
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule.  */
#define YYMAXLEFT 0

/* YYTRANSLATE(X) -- Bison symbol number corresponding to X.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   333

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    94,     2,     2,     2,    96,    89,     2,
      80,    81,    90,    91,    84,    92,    88,    95,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    85,   103,
      97,   102,    98,   101,   104,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    86,     2,    87,    99,     2,    79,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    82,   100,    83,    93,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     5,     7,     8,    12,    14,    20,    25,
      31,    38,    43,    46,    50,    54,    58,    62,    66,    70,
      74,    78,    83,    88,    92,    94,    98,   100,   102,   104,
     106,   108,   112,   118,   121,   125,   130,   135,   139,   143,
     148,   150,   152,   154,   156,   158,   163,   167,   172,   176,
     180,   183,   186,   195,   197,   201,   203,   206,   209,   212,
     215,   220,   223,   228,   233,   235,   237,   239,   241,   243,
     245,   247,   252,   257,   259,   263,   267,   271,   273,   277,
     281,   283,   287,   291,   293,   297,   301,   305,   309,   311,
     315,   319,   321,   325,   327,   331,   333,   337,   339,   343,
     345,   349,   351,   357,   359,   363,   365,   367,   369,   371,
     373,   375,   377,   379,   381,   383,   385,   387,   391,   393,
     396,   398,   401,   403,   405,   408,   410,   416,   421,   428,
     433,   435,   437,   439,   443,   445,   450,   453,   454,   457,
     463,   467,   469,   473,   475,   477,   479,   481,   483,   485,
     487,   489,   491,   493,   495,   498,   500,   502,   504,   506,
     513,   519,   524,   527,   529,   531,   533,   535,   537,   539,
     542,   548,   553,   557,   566,   569,   572,   576,   578,   580,
     582,   585,   587,   588,   590,   593,   596,   598,   602,   604,
     610,   617,   622,   628,   631,   633,   637,   639,   643,   646,
     648,   650,   654,   659,   663,   669,   674,   676,   678,   681,
     685,   688,   692,   696,   701,   705,   710,   716,   718,   721,
     724,   728,   730,   734,   736,   740,   744,   748,   751,   753,
     756,   758,   761,   764,   766,   770,   774,   777,   780,   785,
     787,   791,   793,   797,   800,   803,   805,   807,   810,   813,
     815,   817,   819,   821,   824,   827,   829,   831,   834,   838,
     841,   845,   849,   854,   858,   863,   869,   872,   874,   876,
     880,   885,   889,   894,   898,   900,   902,   904,   906,   908,
     910,   912,   914,   918,   922,   924,   927,   930,   934,   938,
     943,   945,   948,   950,   953,   956,   962,   970,   976,   980,
     985,   989,   991,   992,   998,  1006,  1016,  1020,  1023,  1026,
    1029,  1033,  1035,  1037,  1044,  1050,  1052,  1053,  1055,  1058
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const short int yyrhs[] =
{
     202,     0,    -1,     3,    -1,   106,    -1,    -1,   106,    79,
     106,    -1,    43,    -1,   109,    80,   136,    81,   192,    -1,
     109,    80,    81,   192,    -1,    15,   106,    82,   135,    83,
      -1,    15,   106,    82,   135,    84,    83,    -1,    15,   106,
      82,    83,    -1,    45,   192,    -1,    73,   188,    83,    -1,
      73,   142,    83,    -1,    74,   188,    83,    -1,    74,   142,
      83,    -1,    75,   188,    83,    -1,    75,   142,    83,    -1,
      76,   188,    83,    -1,    76,   142,    83,    -1,    72,    82,
     188,    83,    -1,    72,    82,   142,    83,    -1,   141,    85,
     141,    -1,   114,    -1,   115,    84,   114,    -1,   106,    -1,
     108,    -1,     4,    -1,     5,    -1,     6,    -1,    80,   142,
      81,    -1,    77,   141,    84,   141,    78,    -1,    86,    87,
      -1,    86,   118,    87,    -1,    86,   118,    84,    87,    -1,
      86,   118,    60,    87,    -1,    86,    85,    87,    -1,    86,
     115,    87,    -1,    86,   115,    84,    87,    -1,   110,    -1,
     111,    -1,   112,    -1,   113,    -1,   116,    -1,   117,    86,
     142,    87,    -1,   117,    80,    81,    -1,   117,    80,   118,
      81,    -1,   117,    88,   106,    -1,   117,    17,   106,    -1,
     117,    18,    -1,   117,    19,    -1,    16,    80,   142,    84,
     174,    84,   106,    81,    -1,   141,    -1,   118,    84,   141,
      -1,   117,    -1,    18,   119,    -1,    19,   119,    -1,   120,
     121,    -1,     9,   119,    -1,     9,    80,   174,    81,    -1,
      11,   119,    -1,    11,    80,   174,    81,    -1,    10,    80,
     174,    81,    -1,    89,    -1,    90,    -1,    91,    -1,    92,
      -1,    93,    -1,    94,    -1,   119,    -1,    80,   174,    81,
     121,    -1,    82,   142,    83,   121,    -1,   121,    -1,   122,
      90,   121,    -1,   122,    95,   121,    -1,   122,    96,   121,
      -1,   122,    -1,   123,    91,   122,    -1,   123,    92,   122,
      -1,   123,    -1,   124,    20,   123,    -1,   124,    21,   123,
      -1,   124,    -1,   125,    97,   124,    -1,   125,    98,   124,
      -1,   125,    22,   124,    -1,   125,    23,   124,    -1,   125,
      -1,   126,    24,   125,    -1,   126,    25,   125,    -1,   126,
      -1,   127,    89,   126,    -1,   127,    -1,   128,    99,   127,
      -1,   128,    -1,   129,   100,   128,    -1,   129,    -1,   130,
      26,   129,    -1,   130,    -1,   131,    27,   130,    -1,   131,
      -1,   131,   101,   142,    85,   132,    -1,   132,    -1,   119,
     134,   141,    -1,   102,    -1,    28,    -1,    29,    -1,    30,
      -1,    31,    -1,    32,    -1,    33,    -1,    34,    -1,    35,
      -1,    36,    -1,    37,    -1,   106,    -1,   135,    84,   106,
      -1,   135,    -1,   135,    60,    -1,   138,    -1,   137,   138,
      -1,   146,    -1,   144,    -1,     1,   103,    -1,   133,    -1,
      44,   142,    82,   137,    83,    -1,    44,   142,    82,    83,
      -1,    46,    80,   141,    84,   118,    81,    -1,    46,    80,
     141,    81,    -1,   139,    -1,   140,    -1,   141,    -1,   142,
      84,   141,    -1,   132,    -1,    12,   160,   147,   103,    -1,
     104,   143,    -1,    -1,   160,   103,    -1,   104,   143,   160,
     147,   103,    -1,   160,   147,   103,    -1,   166,    -1,   147,
      84,   166,    -1,    56,    -1,    47,    -1,    48,    -1,    49,
      -1,    50,    -1,    51,    -1,    54,    -1,    55,    -1,    52,
      -1,    53,    -1,   148,    -1,   149,   148,    -1,   149,    -1,
     106,    -1,   151,    -1,   163,    -1,   152,   107,    82,   154,
     156,    83,    -1,   152,   107,    82,   156,    83,    -1,   152,
     107,    82,    83,    -1,   152,   106,    -1,    57,    -1,    58,
      -1,    57,    -1,    58,    -1,    59,    -1,   155,    -1,   154,
     155,    -1,   104,   143,   160,   161,   103,    -1,   104,   143,
     160,   103,    -1,   160,   161,   103,    -1,   104,   104,   143,
     160,   162,    85,   143,   103,    -1,   106,    85,    -1,     1,
     103,    -1,   104,   143,   103,    -1,     7,    -1,     8,    -1,
     157,    -1,   157,   158,    -1,   158,    -1,    -1,   150,    -1,
     158,   150,    -1,   150,   158,    -1,   162,    -1,   161,    84,
     162,    -1,   166,    -1,    59,   107,    82,   164,    83,    -1,
      59,   107,    82,   164,    84,    83,    -1,    59,   107,    82,
      83,    -1,    59,   107,    82,    84,    83,    -1,    59,   106,
      -1,   165,    -1,   164,    84,   165,    -1,   106,    -1,   106,
     102,   143,    -1,   170,   167,    -1,   167,    -1,   106,    -1,
      80,   166,    81,    -1,   167,    86,   143,    87,    -1,   167,
      86,    87,    -1,   167,    80,   171,    81,   159,    -1,   167,
      80,    81,   159,    -1,   170,    -1,   169,    -1,   170,   169,
      -1,    80,   168,    81,    -1,    86,    87,    -1,    86,   143,
      87,    -1,   169,    86,    87,    -1,   169,    86,   143,    87,
      -1,    80,    81,   159,    -1,   169,    80,    81,   159,    -1,
     169,    80,   171,    81,   159,    -1,    90,    -1,    90,   158,
      -1,    90,   170,    -1,    90,   158,   170,    -1,   172,    -1,
     172,    84,    60,    -1,   173,    -1,   172,    84,   173,    -1,
     145,   160,   166,    -1,   145,   160,   168,    -1,   145,   160,
      -1,   175,    -1,   175,   184,    -1,   176,    -1,   158,   176,
      -1,   176,   158,    -1,   149,    -1,   106,    79,   149,    -1,
     106,    79,   106,    -1,    79,   106,    -1,   153,   106,    -1,
     153,   106,    79,   106,    -1,   178,    -1,   178,    84,    60,
      -1,   179,    -1,   178,    84,   179,    -1,   180,   186,    -1,
     180,   184,    -1,   180,    -1,   181,    -1,   181,   158,    -1,
     158,   181,    -1,   149,    -1,   106,    -1,   182,    -1,   183,
      -1,   152,   106,    -1,    59,   106,    -1,   170,    -1,   185,
      -1,   170,   185,    -1,    80,   184,    81,    -1,    86,    87,
      -1,    86,   143,    87,    -1,   185,    86,    87,    -1,   185,
      86,   143,    87,    -1,    80,    81,   159,    -1,   185,    80,
      81,   159,    -1,   185,    80,   177,    81,   159,    -1,   170,
     187,    -1,   187,    -1,   106,    -1,    80,   186,    81,    -1,
     187,    86,   143,    87,    -1,   187,    86,    87,    -1,   187,
      80,   177,    81,    -1,   187,    80,    81,    -1,   192,    -1,
     194,    -1,   195,    -1,   198,    -1,   199,    -1,   201,    -1,
     196,    -1,   189,    -1,    41,   135,   103,    -1,    42,   135,
     103,    -1,   190,    -1,   191,   190,    -1,    82,    83,    -1,
      82,   191,    83,    -1,    82,   193,    83,    -1,    82,   191,
     193,    83,    -1,   188,    -1,   193,   188,    -1,   103,    -1,
     142,   103,    -1,     1,   103,    -1,    61,    80,   142,    81,
     188,    -1,    61,    80,   142,    81,   188,    62,   188,    -1,
      63,    80,   142,    81,   192,    -1,   106,    85,   188,    -1,
      70,   142,    85,   188,    -1,    71,    85,   188,    -1,   142,
      -1,    -1,    64,    80,   142,    81,   188,    -1,    65,   188,
      64,    80,   142,    81,   103,    -1,    66,    80,   197,   103,
     197,   103,   197,    81,   188,    -1,    40,   106,   103,    -1,
      67,   103,    -1,    68,   103,    -1,    69,   103,    -1,    69,
     142,   103,    -1,    13,    -1,    14,    -1,   200,   106,    80,
     136,    81,   192,    -1,   200,   106,    80,    81,   192,    -1,
     203,    -1,    -1,   204,    -1,   203,   204,    -1,   188,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,    92,    92,    96,    98,   102,   107,   111,   113,   118,
     120,   122,   127,   132,   134,   136,   138,   140,   142,   144,
     146,   148,   150,   155,   165,   167,   172,   173,   174,   176,
     178,   180,   182,   187,   189,   191,   193,   198,   200,   202,
     204,   205,   206,   207,   211,   212,   214,   216,   218,   220,
     222,   224,   226,   231,   233,   238,   239,   241,   243,   245,
     247,   249,   251,   253,   258,   260,   262,   264,   266,   268,
     273,   274,   276,   281,   282,   284,   286,   291,   292,   294,
     299,   300,   302,   307,   308,   310,   312,   314,   319,   320,
     322,   327,   328,   333,   334,   339,   340,   345,   346,   351,
     352,   357,   358,   363,   364,   373,   375,   377,   379,   381,
     383,   385,   387,   389,   391,   393,   398,   400,   405,   407,
     415,   417,   422,   423,   424,   429,   430,   432,   437,   439,
     444,   445,   449,   450,   455,   459,   464,   467,   471,   473,
     475,   480,   482,   487,   489,   491,   493,   495,   497,   499,
     501,   503,   505,   509,   511,   516,   518,   520,   521,   525,
     527,   529,   531,   536,   538,   543,   545,   547,   552,   561,
     572,   574,   576,   578,   581,   583,   588,   594,   595,   599,
     600,   604,   605,   609,   610,   612,   617,   619,   624,   628,
     630,   632,   634,   636,   641,   643,   650,   652,   657,   659,
     663,   665,   667,   669,   671,   673,   678,   679,   680,   685,
     687,   689,   691,   693,   695,   697,   699,   704,   706,   708,
     710,   715,   717,   722,   724,   729,   731,   733,   738,   740,
     745,   746,   748,   753,   755,   758,   761,   764,   766,   771,
     773,   778,   780,   785,   787,   789,   794,   795,   797,   802,
     804,   806,   807,   811,   816,   821,   822,   823,   828,   830,
     832,   834,   836,   838,   840,   842,   847,   849,   853,   855,
     857,   859,   861,   863,   868,   869,   870,   871,   872,   873,
     874,   875,   879,   884,   889,   891,   896,   906,   916,   929,
     945,   947,   952,   954,   956,   961,   963,   965,   971,   973,
     975,   980,   982,   986,   988,   990,   995,   997,   999,  1001,
    1003,  1008,  1010,  1015,  1017,  1022,  1023,  1027,  1028,  1032
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENTIFIER", "SYMBOL", "CONSTANT",
  "STRING_LITERAL", "CONST", "VOLATILE", "SIZEOF", "TYPENAME", "TYPEOF",
  "TYPEDEF", "DEFINE", "DEFLOC", "DEFREC", "CONTAINEROF", "PTR_OP",
  "INC_OP", "DEC_OP", "LEFT_OP", "RIGHT_OP", "LE_OP", "GE_OP", "EQ_OP",
  "NE_OP", "AND_OP", "OR_OP", "MUL_ASSIGN", "DIV_ASSIGN", "MOD_ASSIGN",
  "ADD_ASSIGN", "SUB_ASSIGN", "LEFT_ASSIGN", "RIGHT_ASSIGN", "AND_ASSIGN",
  "XOR_ASSIGN", "OR_ASSIGN", "CAST_ASSIGN", "XCAST_ASSIGN", "GOTO",
  "GLOBAL", "LOCAL", "LAMBDA", "NAMES", "LET", "LAPPLY", "BOOL", "CHAR",
  "SHORT", "INT", "LONG", "SIGNED", "UNSIGNED", "FLOAT", "DOUBLE", "VOID",
  "STRUCT", "UNION", "ENUM", "ELLIPSIS", "IF", "ELSE", "SWITCH", "WHILE",
  "DO", "FOR", "CONTINUE", "BREAK", "RETURN", "CASE", "DEFAULT", "QUOTE",
  "SYNTAXQUOTE", "SYNTAXQUASI", "SYNTAXUNQUOTE", "SYNTAXSPLICE", "LPAIR",
  "RPAIR", "'`'", "'('", "')'", "'{'", "'}'", "','", "':'", "'['", "']'",
  "'.'", "'&'", "'*'", "'+'", "'-'", "'~'", "'!'", "'/'", "'%'", "'<'",
  "'>'", "'^'", "'|'", "'?'", "'='", "';'", "'@'", "$accept", "id",
  "maybeid", "tickid", "lambda", "lambda_expression", "defrec_expression",
  "let_expression", "quote_expression", "table_init", "table_init_list",
  "primary_expression", "postfix_expression", "argument_expression_list",
  "unary_expression", "unary_operator", "cast_expression",
  "multiplicative_expression", "additive_expression", "shift_expression",
  "relational_expression", "equality_expression", "and_expression",
  "exclusive_or_expression", "inclusive_or_expression",
  "logical_and_expression", "logical_or_expression",
  "conditional_expression", "assignment_expression", "assignment_operator",
  "identifier_list", "arg_id_list", "names_declaration_list",
  "names_declaration", "names_expression", "lapply_expression",
  "root_expression", "expression", "constant_expression", "typedef",
  "maybe_attr", "declaration", "declarator_list", "base", "base_list",
  "type_specifier", "struct_or_union_specifier", "struct_or_union",
  "struct_or_union_or_enum", "struct_declaration_list",
  "struct_declaration", "struct_size", "type_qualifier",
  "type_qualifier_list", "maybe_type_qualifier", "specifier_list",
  "struct_declarator_list", "struct_declarator", "enum_specifier",
  "enumerator_list", "enumerator", "declarator", "direct_declarator",
  "abstract_declarator", "direct_abstract_declarator", "pointer",
  "parameter_type_list", "parameter_list", "parameter_declaration",
  "type_name", "tn_type_qual_specifier", "tn_type_specifier_tick",
  "tn_parameter_type_list", "tn_parameter_list",
  "tn_parameter_declaration", "tn_param_type_qual_specifier",
  "tn_param_type_specifier", "tn_param_struct_or_union_specifier",
  "tn_param_enum_specifier", "tn_abstract_declarator",
  "tn_direct_abstract_declarator", "tn_declarator", "tn_direct_declarator",
  "statement", "global_statement", "local", "local_list",
  "compound_statement", "statement_list", "expression_statement",
  "selection_statement", "labeled_statement", "maybe_expression",
  "iteration_statement", "jump_statement", "define", "define_statement",
  "translation_unit_seq", "translation_unit", "external_declaration", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   105,   106,   107,   107,   108,   109,   110,   110,   111,
     111,   111,   112,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   114,   115,   115,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   118,   118,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   120,   120,   120,   120,   120,   120,
     121,   121,   121,   122,   122,   122,   122,   123,   123,   123,
     124,   124,   124,   125,   125,   125,   125,   125,   126,   126,
     126,   127,   127,   128,   128,   129,   129,   130,   130,   131,
     131,   132,   132,   133,   133,   134,   134,   134,   134,   134,
     134,   134,   134,   134,   134,   134,   135,   135,   136,   136,
     137,   137,   138,   138,   138,   139,   139,   139,   140,   140,
     141,   141,   142,   142,   143,   144,   145,   145,   146,   146,
     146,   147,   147,   148,   148,   148,   148,   148,   148,   148,
     148,   148,   148,   149,   149,   150,   150,   150,   150,   151,
     151,   151,   151,   152,   152,   153,   153,   153,   154,   154,
     155,   155,   155,   155,   155,   155,   156,   157,   157,   158,
     158,   159,   159,   160,   160,   160,   161,   161,   162,   163,
     163,   163,   163,   163,   164,   164,   165,   165,   166,   166,
     167,   167,   167,   167,   167,   167,   168,   168,   168,   169,
     169,   169,   169,   169,   169,   169,   169,   170,   170,   170,
     170,   171,   171,   172,   172,   173,   173,   173,   174,   174,
     175,   175,   175,   176,   176,   176,   176,   176,   176,   177,
     177,   178,   178,   179,   179,   179,   180,   180,   180,   181,
     181,   181,   181,   182,   183,   184,   184,   184,   185,   185,
     185,   185,   185,   185,   185,   185,   186,   186,   187,   187,
     187,   187,   187,   187,   188,   188,   188,   188,   188,   188,
     188,   188,   189,   190,   191,   191,   192,   192,   192,   192,
     193,   193,   194,   194,   194,   195,   195,   195,   196,   196,
     196,   197,   197,   198,   198,   198,   199,   199,   199,   199,
     199,   200,   200,   201,   201,   202,   202,   203,   203,   204
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     1,     0,     3,     1,     5,     4,     5,
       6,     4,     2,     3,     3,     3,     3,     3,     3,     3,
       3,     4,     4,     3,     1,     3,     1,     1,     1,     1,
       1,     3,     5,     2,     3,     4,     4,     3,     3,     4,
       1,     1,     1,     1,     1,     4,     3,     4,     3,     3,
       2,     2,     8,     1,     3,     1,     2,     2,     2,     2,
       4,     2,     4,     4,     1,     1,     1,     1,     1,     1,
       1,     4,     4,     1,     3,     3,     3,     1,     3,     3,
       1,     3,     3,     1,     3,     3,     3,     3,     1,     3,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     5,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     1,     2,
       1,     2,     1,     1,     2,     1,     5,     4,     6,     4,
       1,     1,     1,     3,     1,     4,     2,     0,     2,     5,
       3,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     1,     6,
       5,     4,     2,     1,     1,     1,     1,     1,     1,     2,
       5,     4,     3,     8,     2,     2,     3,     1,     1,     1,
       2,     1,     0,     1,     2,     2,     1,     3,     1,     5,
       6,     4,     5,     2,     1,     3,     1,     3,     2,     1,
       1,     3,     4,     3,     5,     4,     1,     1,     2,     3,
       2,     3,     3,     4,     3,     4,     5,     1,     2,     2,
       3,     1,     3,     1,     3,     3,     3,     2,     1,     2,
       1,     2,     2,     1,     3,     3,     2,     2,     4,     1,
       3,     1,     3,     2,     2,     1,     1,     2,     2,     1,
       1,     1,     1,     2,     2,     1,     1,     2,     3,     2,
       3,     3,     4,     3,     4,     5,     2,     1,     1,     3,
       4,     3,     4,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     1,     2,     2,     3,     3,     4,
       1,     2,     1,     2,     2,     5,     7,     5,     3,     4,
       3,     1,     0,     5,     7,     9,     3,     2,     2,     2,
       3,     1,     1,     6,     5,     1,     0,     1,     2,     1
};

/* YYDPREC[RULE-NUM] -- Dynamic precedence of rule #RULE-NUM (0 if none).  */
static const unsigned char yydprec[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     2,     1,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0
};

/* YYMERGER[RULE-NUM] -- Index of merging function for rule #RULE-NUM.  */
static const unsigned char yymerger[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     1,
       1,     1,     1,     0,     0,     0,     0,     0,     0,     0,
       2,     2,     0,     3,     3,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     4,     4,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error.  */
static const unsigned short int yydefact[] =
{
       0,     0,     2,    28,    29,    30,     0,     0,     0,   311,
     312,     0,     0,     0,     0,     0,     0,     6,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      64,    65,    66,    67,    68,    69,   292,    26,    27,     0,
      40,    41,    42,    43,    44,    55,    70,     0,    73,    77,
      80,    83,    88,    91,    93,    95,    97,    99,   101,   103,
     125,   130,   131,   132,     0,   319,   281,   274,   275,   276,
     280,   277,   278,     0,   279,     0,     0,   317,   294,     0,
      26,    59,     0,     0,    61,     0,     0,     0,    56,    57,
       0,   116,     0,     0,     0,     0,    12,     0,     0,     0,
       0,     0,   302,   307,   308,   309,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   177,
     178,   144,   145,   146,   147,   148,   151,   152,   149,   150,
     143,   165,   166,   167,     0,    26,     0,   153,   233,     0,
     179,     0,     0,   228,   230,     0,   286,     0,   290,   284,
       0,     0,     0,    33,    24,     0,     0,    53,     0,     0,
       0,     0,    50,    51,     0,     0,     0,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   105,     0,    70,
      58,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   293,     0,     1,   318,     0,     0,     0,     0,     0,
       0,   306,     0,   282,     0,     0,     0,     0,     0,     0,
       0,   301,     0,   310,     0,   300,     0,     0,    14,    13,
      16,    15,    18,    17,    20,    19,     0,   236,     0,    31,
     154,   237,   180,   231,     0,     0,     0,   217,   255,   229,
     256,   232,     0,     0,   287,   285,     0,   288,   291,    37,
       0,    38,     0,     0,    34,     0,     5,   298,     0,   118,
       0,    49,    46,     0,    53,     0,    48,   104,    74,    75,
      76,    78,    79,    81,    82,    86,    87,    84,    85,    89,
      90,    92,    94,    96,    98,   100,     0,   133,     0,    60,
       0,    63,    62,    11,     0,     0,   117,     0,     0,   163,
     164,     4,   127,     0,   156,     0,   120,   123,   122,   155,
     183,   157,     4,     0,     0,   158,   129,     0,     0,     0,
       0,     0,   302,   299,    22,    21,     0,     5,   234,     0,
      71,   182,     0,   259,   134,     0,   218,   219,   257,     0,
       0,   283,    72,   289,    39,    25,     0,    36,    35,    54,
      23,     8,   119,     0,    47,     0,    45,     0,     0,     0,
     235,     9,     0,     0,   124,     0,   193,     0,     0,   126,
     121,   185,   162,     0,   184,     0,   138,   200,     0,   141,
     199,     0,     0,   295,   297,   303,     0,     0,    32,   238,
     181,   263,   258,   260,   220,     0,   182,   250,   249,     0,
       0,     0,   239,   241,   245,   246,   251,   252,   261,     0,
       7,   102,   314,     0,    10,     0,     0,     0,     0,     0,
       0,     0,   140,   137,     0,   198,   128,     0,     0,   302,
     254,   264,   253,   248,   182,     0,     0,   268,   255,   244,
     243,   267,   247,   262,   313,     0,   135,   191,     0,   196,
       0,   194,     0,     0,   161,     0,   156,     0,   168,     0,
       0,   201,   142,   182,     0,     0,     0,   221,   223,   203,
       0,   296,   304,     0,   265,   240,   242,     0,   266,     0,
       0,    52,   192,     0,   189,     0,   139,   175,     0,     0,
     174,   169,     0,   160,     0,   186,   188,   205,   136,   227,
     182,   137,   202,     0,   269,   273,     0,   271,     0,   197,
     190,   195,     0,   176,     0,   159,     0,   172,     0,     0,
     225,   226,   207,   206,   204,   222,   224,   305,   272,   270,
       0,   171,     0,   187,   182,     0,   210,     0,   137,     0,
     208,     0,   170,   214,   209,   211,   182,     0,   212,     0,
       0,   215,   182,   213,     0,   216,   173
};

/* YYPDEFGOTO[NTERM-NUM].  */
static const short int yydefgoto[] =
{
      -1,    90,   387,    48,    49,    50,    51,    52,    53,   164,
     165,    54,    55,   166,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,   188,
     279,   280,   325,   326,    71,    72,    73,    74,   355,   327,
     485,   328,   398,   147,   329,   330,   331,   332,   149,   477,
     478,   479,   150,   333,   411,   334,   514,   515,   335,   470,
     471,   516,   400,   541,   542,   401,   486,   487,   488,   152,
     153,   154,   421,   422,   423,   424,   425,   426,   427,   352,
     260,   460,   461,   158,    76,   159,   160,    77,   161,    78,
      79,    80,   232,    81,    82,    83,    84,    85,    86,    87
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -490
static const short int yypact[] =
{
     653,   -41,  -490,  -490,  -490,  -490,  2251,     8,  2343,  -490,
    -490,   108,    40,  2366,  2366,   108,   108,  -490,  1791,    61,
      72,    97,   101,   124,  1319,   126,   112,   153,  1437,  1791,
     173,   194,  1319,  1319,  1319,  1319,  1791,  1538,   939,  1561,
    -490,  -490,  -490,  -490,  -490,  -490,  -490,   129,  -490,   131,
    -490,  -490,  -490,  -490,  -490,   378,   573,  2228,  -490,   279,
      75,    65,     1,   334,   196,   191,   192,   290,    48,  -490,
    -490,  -490,  -490,  -490,   -20,  -490,  -490,  -490,  -490,  -490,
    -490,  -490,  -490,   108,  -490,   305,   748,  -490,  -490,  1538,
     228,  -490,  1658,  1538,  -490,   269,  1791,  1791,  -490,  -490,
     257,  -490,   -11,  1791,   -24,   939,  -490,  1791,  1791,  1791,
    1791,   301,  1791,  -490,  -490,  -490,    73,   292,  1319,  1319,
     100,   298,   133,   310,   136,   317,   144,   319,   326,  -490,
    -490,  -490,  -490,  -490,  -490,  -490,  -490,  -490,  -490,  -490,
    -490,  -490,  -490,  -490,   108,   333,   234,  -490,   594,   108,
     413,   728,   335,   208,   413,   108,  -490,   170,  -490,  -490,
    1034,  1129,   359,  -490,  -490,   296,    -3,   363,   108,  1319,
      43,   108,  -490,  -490,  1653,  1791,   108,  -490,  -490,  -490,
    -490,  -490,  -490,  -490,  -490,  -490,  -490,  -490,  1791,  -490,
    -490,  2228,  2228,  2228,  2228,  2228,  2228,  2228,  2228,  2228,
    2228,  2228,  2228,  2228,  2228,  2228,  2228,  2228,  2228,  1791,
    1791,  -490,   374,  -490,  -490,   376,   384,   387,   389,    19,
     381,  -490,   108,  -490,   343,   842,   304,   306,   330,   341,
     392,   404,   390,  -490,  1319,  -490,   199,   407,  -490,  -490,
    -490,  -490,  -490,  -490,  -490,  -490,  1791,  -490,   565,  -490,
    -490,   416,  -490,  -490,  2228,   266,  1883,    25,   165,  -490,
     207,  -490,    76,  2228,  -490,  -490,  1224,  -490,  -490,  -490,
    1676,  -490,   409,  1768,  -490,  1791,  -490,  -490,    61,   -35,
     417,  -490,  -490,   353,  -490,   360,  -490,  -490,  -490,  -490,
    -490,   279,   279,    75,    75,    65,    65,    65,    65,     1,
       1,   334,   196,   191,   192,   290,   351,  -490,    53,  -490,
     565,  -490,  -490,  -490,   391,  1538,  -490,   396,  1888,  -490,
    -490,   108,  -490,  2228,  -490,   855,  -490,  -490,  -490,   594,
     413,  -490,   108,  1014,    15,  -490,  -490,  1791,  1319,    61,
    1319,  1791,  1791,  -490,  -490,  -490,   423,   323,   594,   108,
    -490,   413,   424,  -490,  -490,   419,   414,  -490,   207,  2418,
    1906,  -490,  -490,  -490,  -490,  -490,   363,  -490,  -490,  -490,
    -490,  -490,  -490,    61,  -490,  1791,  -490,  2228,    61,   426,
    -490,  -490,    31,   427,  -490,    51,   428,   430,  1888,  -490,
    -490,  -490,   428,   432,  -490,    51,  -490,  -490,   134,  -490,
     246,    71,   369,   446,  -490,  -490,   371,   412,  -490,  -490,
    -490,  -490,  -490,  -490,  -490,   108,   413,  -490,   594,   108,
    1109,   436,   434,  -490,    14,   413,  -490,  -490,  -490,   433,
    -490,  -490,  -490,    61,  -490,   108,   158,    62,    51,   914,
     438,    51,  -490,   -51,  1998,   246,  -490,  1319,   418,  1791,
    -490,  -490,  -490,  -490,   413,  1773,    26,  -490,    52,  -490,
    -490,   281,  -490,  -490,  -490,   441,  -490,  -490,   443,   422,
     393,  -490,   171,   425,  -490,  1414,   444,   578,  -490,   447,
      51,  -490,  -490,   413,  2228,  1888,   450,   448,  -490,  -490,
     452,  -490,  -490,   453,  -490,  -490,  -490,   454,   281,  2475,
    2021,  -490,  -490,  2228,  -490,    67,  -490,  -490,  2228,   824,
    -490,  -490,   460,  -490,   183,  -490,  -490,  -490,  -490,    49,
     413,   -15,  -490,  1319,  -490,  -490,   456,  -490,   458,  -490,
    -490,  -490,  1888,  -490,    23,  -490,    51,  -490,    47,  2113,
    -490,  -490,   293,    56,  -490,  -490,  -490,  -490,  -490,  -490,
      51,  -490,   197,  -490,   413,   465,  -490,   461,   -33,  2136,
     293,   466,  -490,  -490,  -490,  -490,   413,   469,  -490,   468,
    2228,  -490,   413,  -490,   449,  -490,  -490
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -490,     4,   224,  -490,  -490,  -490,  -490,  -490,  -490,   288,
    -490,  -490,  -490,  -169,    -6,  -490,   -22,   285,   286,   232,
     284,   355,   356,   354,   358,   361,  -490,    41,  -490,  -490,
      27,   254,  -490,   238,  -490,  -490,    33,   231,  -204,  -490,
    -490,  -490,  -358,  -117,   -26,   237,  -490,  -338,  -490,  -490,
      94,    98,  -490,   141,  -105,  -164,    42,  -489,  -490,  -490,
      69,  -294,  -385,    39,    35,  -147,    24,  -490,    63,   -80,
    -490,   437,    84,  -490,   132,  -490,   169,  -490,  -490,  -152,
    -248,   135,   139,    44,  -490,   439,  -490,   -16,   462,  -490,
    -490,  -490,  -328,  -490,  -490,  -490,  -490,  -490,  -490,   504
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -317
static const short int yytable[] =
{
      91,   259,    94,   106,    47,   283,   258,    98,    99,   215,
     358,   148,   217,   218,   407,    95,   445,     2,     2,   100,
     101,   419,     2,   198,   199,   372,     2,   436,    47,     2,
     483,   250,   129,   130,     2,   190,    47,    47,    47,    47,
     399,   145,    47,   102,    75,   545,     2,   553,   566,   222,
       2,   189,     2,   484,     2,     2,     2,   272,   225,     2,
     210,   561,    88,   148,   210,     2,   148,   148,   111,   128,
       2,   484,   167,   222,     2,   208,   121,   123,   125,   127,
     472,   273,   419,   211,   274,   196,   197,   212,    92,   484,
      47,   399,   223,   145,   456,   395,   216,   145,   200,   201,
     256,   440,   313,   395,   257,   257,   456,   351,   258,    47,
     357,     2,   256,   257,   434,   257,   257,   419,   396,   388,
      96,   493,    47,    47,   278,   148,   551,   538,   554,   538,
      75,   395,   456,   539,   378,   539,   538,   257,   256,   257,
     226,   257,   539,   105,   399,   467,   468,   482,   247,   209,
     530,   395,   107,   251,   385,   216,   429,   210,   445,   101,
     222,   419,   235,   237,    47,    47,   194,   195,   402,   288,
     289,   290,   276,    47,   101,   281,   233,   108,   151,   361,
     286,   109,   262,   238,   210,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   211,   110,   268,   112,   284,   168,   414,
     358,   170,   250,   277,   169,   113,   240,   210,   441,   242,
     210,   287,   348,   101,   438,   540,   316,   244,   210,   324,
     151,   250,   350,   151,   151,   383,   211,   442,    47,   211,
     490,   362,   441,   307,   440,   255,   314,   211,   189,   104,
     189,   256,   347,   263,   210,   441,   114,   189,   118,   116,
     117,   466,   371,   120,   122,   124,   126,   536,   146,   157,
      47,   509,   459,   211,   506,   480,   119,   458,   343,   346,
     518,   536,   344,   210,   348,   204,   537,   359,   255,   148,
     205,   252,   206,   360,   256,   261,   528,   354,   257,   529,
     562,   250,   211,   366,   532,   213,   369,   168,   370,   458,
     268,   451,   101,   480,   380,   249,   207,   189,   210,   145,
     146,   519,   324,   404,   146,   386,   443,   220,   146,   324,
    -235,  -235,   444,   418,   224,   557,   392,   324,   397,   227,
     228,   229,    47,   231,    47,   534,   255,   351,   307,   494,
     236,   219,   256,   409,   189,   569,   257,   430,   202,   203,
     221,   499,   432,   417,   354,   230,   574,   500,   550,   191,
     284,   189,   543,   558,   192,   193,   210,   234,   517,   559,
     270,   239,   403,   271,   405,   336,   316,   338,   337,   397,
     210,   543,   324,   241,   418,   171,   172,   173,   356,   397,
     243,   354,   245,    -5,    -5,   397,   285,    -5,   369,    -5,
     246,   339,   248,    -5,   210,   544,   254,   464,   431,   450,
     129,   130,   340,   452,   417,   210,   263,   210,   457,   418,
     295,   296,   297,   298,   374,   210,   377,   375,   189,   465,
     306,   469,   397,   476,   210,   397,   269,   376,   275,   563,
     446,    47,   448,   375,   308,   210,   151,   309,   174,   417,
     457,   571,   457,   310,   175,   315,   176,   575,   311,   189,
     312,   391,   341,   418,   381,   382,   504,   505,   189,   291,
     292,   476,   293,   294,   397,   354,   299,   300,   210,   324,
     345,   491,   410,   342,   189,   349,   367,   189,   373,   384,
     420,   408,   189,   417,   257,   412,   413,   433,   447,   469,
      -3,   435,   437,   324,   439,   449,   354,   454,   455,   481,
     463,   492,   501,   397,   503,   354,   502,    47,   507,   510,
     513,   520,   521,   189,   523,   524,   324,   548,   397,   522,
     397,   354,   397,   535,   354,   549,   564,   397,   565,   354,
     572,   570,   576,   189,   397,   573,   393,   410,   365,   301,
     303,   302,   379,   390,   189,   304,   462,   547,     2,   305,
     394,   511,   406,   231,   531,   512,   552,   555,   560,   473,
     354,     2,   567,   526,   546,   129,   130,   496,   253,   453,
     214,   497,     0,     0,     0,   410,   420,   498,     0,   265,
     354,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   354,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   266,     0,   410,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   319,   320,   321,     0,     0,
     420,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,     0,     0,  -316,     1,     0,     2,     3,     4,     5,
       0,   410,     6,     7,     8,     0,     9,    10,    11,    12,
       0,    13,    14,     0,     0,   187,     0,     0,     0,     0,
     231,     0,   475,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    15,    16,   410,    17,    18,    19,    20,
       0,     0,     0,     0,     0,     0,     0,   410,     0,     0,
       0,     0,     0,   410,    21,     0,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,     2,     0,    37,     0,    38,     0,     0,     0,    39,
       0,     0,    40,    41,    42,    43,    44,    45,  -315,     1,
       0,     2,     3,     4,     5,     0,    46,     6,     7,     8,
       0,     9,    10,    11,    12,     0,    13,    14,     0,     0,
       0,     0,     0,     0,     0,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,    15,    16,
       0,    17,    18,    19,    20,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   144,     0,    21,
       0,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,     0,     2,    37,     0,
      38,   129,   130,     0,    39,     0,     0,    40,    41,    42,
      43,    44,    45,   317,     0,     2,     0,     0,     0,   129,
     130,    46,     0,     0,   318,     0,   317,     0,     2,     0,
       0,     0,   129,   130,     0,     0,     0,   318,     0,     0,
       0,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   319,   320,   321,     0,     0,     0,     0,     0,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   319,
     320,   321,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   319,   320,   321,   473,     0,     2,     0,     0,
       0,   129,   130,     0,     0,   322,     0,   533,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   389,     0,
       1,     0,     2,     3,     4,     5,   323,     0,     6,     7,
       8,     0,     9,    10,    11,    12,     0,    13,    14,   323,
       0,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   319,   320,   321,     0,     0,     0,     0,     0,    15,
      16,   155,    17,    18,    19,    20,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   474,     0,     0,
      21,     0,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,     2,   475,    37,
       0,    38,   156,     0,     0,    39,     0,     0,    40,    41,
      42,    43,    44,    45,     0,     1,     0,     2,     3,     4,
       5,     0,    46,     6,     7,     8,     0,     9,    10,    11,
      12,     0,    13,    14,     0,     0,     0,     0,     0,     0,
       0,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   319,   320,   321,    15,    16,   155,    17,    18,    19,
      20,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    21,     0,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,     2,     0,    37,     0,    38,   264,     0,     0,
      39,     0,     0,    40,    41,    42,    43,    44,    45,     0,
       1,     0,     2,     3,     4,     5,     0,    46,     6,     7,
       8,     0,     9,    10,    11,    12,     0,    13,    14,     0,
       0,     0,     0,     0,     0,     0,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   319,   320,   415,    15,
      16,     0,    17,    18,    19,    20,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      21,     0,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,     0,     0,    37,
       0,    38,   267,     0,     0,    39,     0,     0,    40,    41,
      42,    43,    44,    45,     0,     1,     0,     2,     3,     4,
       5,     0,    46,     6,     7,     8,     0,     9,    10,    11,
      12,     0,    13,    14,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    15,    16,     0,    17,    18,    19,
      20,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    21,     0,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,     0,     0,    37,     0,    38,   363,     0,     0,
      39,     0,     0,    40,    41,    42,    43,    44,    45,     0,
       1,     0,     2,     3,     4,     5,     0,    46,     6,     7,
       8,     0,     9,    10,    11,    12,     0,    13,    14,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    15,
      16,     0,    17,    18,    19,    20,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      21,     0,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,     0,     0,    37,
       0,    38,     0,     0,     0,    39,     0,     0,    40,    41,
      42,    43,    44,    45,     0,     0,     0,     2,     3,     4,
       5,     0,    46,     6,     7,     8,     0,     0,     0,    11,
      12,     0,    13,    14,     0,     0,     0,     0,     0,     0,
       2,     3,     4,     5,     0,     0,     6,     7,     8,     0,
       0,     0,    11,    12,     0,    13,    14,    17,     0,    19,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      17,    18,    19,    20,     0,     0,    31,    32,    33,    34,
      35,    36,     0,     0,    37,     0,   103,     0,     0,     0,
      39,     0,     0,    40,    41,    42,    43,    44,    45,    31,
      32,    33,    34,    35,    36,     0,     0,    37,   508,   103,
       0,     0,     0,    39,     0,     0,    40,    41,    42,    43,
      44,    45,     0,     0,     0,     0,     0,     0,     0,     0,
     115,     2,     3,     4,     5,   129,   130,     6,     7,     8,
       0,     0,     0,    11,    12,     0,    13,    14,     0,     0,
       0,     0,     0,     0,     2,     3,     4,     5,     0,     0,
       6,     7,     8,     0,     0,     0,    11,    12,     0,    13,
      14,    17,    18,    19,    20,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,     0,     0,
       0,     0,     0,     0,    17,    18,    19,    20,     0,     0,
      31,    32,    33,    34,    35,    36,     0,   144,    37,     0,
     103,     0,     0,     0,    39,     0,     0,    40,    41,    42,
      43,    44,    45,    31,    32,    33,    34,    35,    36,     0,
       0,    37,     0,   103,     0,     0,   162,    39,   163,     0,
      40,    41,    42,    43,    44,    45,     2,     3,     4,     5,
       0,     2,     6,     7,     8,   129,   130,     0,    11,    12,
       0,    13,    14,     0,     0,     0,     0,     0,     0,     2,
       3,     4,     5,     0,     0,     6,     7,     8,     0,     0,
       0,    11,    12,     0,    13,    14,    17,    18,    19,    20,
       0,     0,     0,     0,     0,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,     0,    17,
      18,    19,    20,     0,     0,    31,    32,    33,    34,    35,
      36,     0,     0,    37,   282,   103,     0,   144,     0,    39,
       0,     0,    40,    41,    42,    43,    44,    45,    31,    32,
      33,    34,    35,    36,     0,     0,    37,     0,   103,     0,
       0,     0,    39,   364,     0,    40,    41,    42,    43,    44,
      45,     2,     3,     4,     5,     0,     2,     6,     7,     8,
     129,   130,     0,    11,    12,     0,    13,    14,     0,     0,
       0,     0,     0,     0,     2,     3,     4,     5,     0,     0,
       6,     7,     8,     0,     0,     0,    11,    12,     0,    13,
      14,    17,    18,    19,    20,     0,     0,     0,     0,     0,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     319,   320,   415,   495,    17,    18,    19,    20,     0,     0,
      31,    32,    33,    34,    35,    36,     0,     0,    37,     0,
     103,     0,     0,     0,    39,   368,     0,    40,    41,    42,
      43,    44,    45,    31,    32,    33,    34,    35,    36,     0,
       0,    37,     0,   103,     0,     0,     0,    39,     0,     0,
      40,    41,    42,    43,    44,    45,     2,     3,     4,     5,
       0,     2,     6,     7,     8,   129,   130,     0,    11,    12,
       0,    13,    14,     0,     0,     0,     0,     0,     0,     2,
       3,     4,     5,     0,     0,     6,     7,     8,     0,     0,
       0,    11,    12,     0,    13,    14,    17,     0,    19,     0,
       0,     0,     0,     0,     0,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   319,   320,   321,     0,    17,
       0,    19,     0,     0,     0,    31,    32,    33,    34,    35,
      36,     0,     0,    37,     0,   103,     0,     0,     0,    39,
     353,     0,    40,    41,    42,    43,    44,    45,    31,    32,
      33,    34,    35,    36,     0,     0,    37,     0,   103,     0,
       0,     0,    39,   428,     0,    40,    41,    42,    43,    44,
      45,     2,     3,     4,     5,     0,     0,     6,     7,     8,
       0,     0,     0,    11,    12,     0,    13,    14,     0,     0,
       0,     0,     0,     0,     2,     3,     4,     5,     0,     0,
       6,     7,     8,     0,     0,     0,    11,    12,     0,    13,
      14,    17,     0,    19,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    17,     0,    19,     0,     0,     0,
      31,    32,    33,    34,    35,    36,     0,     0,    37,     0,
     103,     0,     0,     0,    39,   489,     0,    40,    41,    42,
      43,    44,    45,    31,    32,    33,    34,    35,    36,     0,
       0,    37,     0,   103,     0,     0,     0,    39,   527,     0,
      40,    41,    42,    43,    44,    45,     2,     3,     4,     5,
       0,     0,     6,     7,     8,     0,     0,     0,    11,    12,
       0,    13,    14,     0,     0,     0,     0,     0,     0,     2,
       3,     4,     5,     0,     0,     6,     7,     8,     0,     0,
       0,    11,    12,     0,    13,    14,    17,     0,    19,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    17,
       0,    19,     0,     0,     0,    31,    32,    33,    34,    35,
      36,     0,     0,    37,     0,   103,     0,     0,     0,    39,
     556,     0,    40,    41,    42,    43,    44,    45,    31,    32,
      33,    34,    35,    36,     0,     0,    37,     0,   103,     0,
       0,     0,    39,   568,     0,    40,    41,    42,    43,    44,
      45,     2,     3,     4,     5,     0,     0,     6,     7,     8,
       0,     0,     0,    11,    12,     0,    13,    14,     0,     0,
       0,     0,     0,     0,     2,     3,     4,     5,     0,     0,
       6,     7,     8,     0,     0,     0,    11,    12,     0,    13,
      14,    17,     0,    19,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    17,     0,    19,     0,     0,     0,
      31,    32,    33,    34,    35,    36,     0,     0,    37,     0,
     103,     0,     0,     0,    39,     0,     0,    40,    41,    42,
      43,    44,    45,    31,    32,    33,    34,    35,    36,     0,
       0,    89,     0,     0,     0,     0,     0,    39,     0,     0,
      40,    41,    42,    43,    44,    45,     2,     3,     4,     5,
       0,     0,     6,     7,     8,     0,     0,     0,    11,    12,
       0,    13,    14,     0,     0,     0,     0,     0,     0,     2,
       3,     4,     5,     0,     0,     6,     7,     8,     0,     0,
       0,    11,    12,     0,    13,    14,    17,     0,    19,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    17,
       0,    19,     0,     0,     0,    31,    32,    33,    34,    35,
      36,     2,     0,    93,     0,   129,   130,     0,     0,    39,
       0,     0,    40,    41,    42,    43,    44,    45,    31,    32,
      33,    34,    35,    36,     0,     0,    97,     0,     0,     0,
       0,     0,    39,     0,     0,    40,    41,    42,    43,    44,
      45,     0,     0,     0,     0,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   319,   320,   415,     2,     0,
       0,     0,   129,   130,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   416,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   319,   320,   415,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   525
};

/* YYCONFLP[YYPACT[STATE-NUM]] -- Pointer into YYCONFL of start of
   list of conflicting reductions corresponding to action entry for
   state STATE-NUM in yytable.  0 means no conflicts.  The list in
   yyconfl is terminated by a rule number of 0.  */
static const unsigned char yyconflp[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     1,     3,     0,     0,     5,     0,     7,
       0,     0,     0,     9,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    11,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0
};

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short int yyconfl[] =
{
       0,   235,     0,   235,     0,   235,     0,   235,     0,   235,
       0,   295,     0
};

static const short int yycheck[] =
{
       6,   153,     8,    19,     0,   174,   153,    13,    14,    89,
     258,    37,    92,    93,   342,    11,   401,     3,     3,    15,
      16,   359,     3,    22,    23,    60,     3,   385,    24,     3,
      81,   148,     7,     8,     3,    57,    32,    33,    34,    35,
     334,    37,    38,    16,     0,    60,     3,   536,    81,    84,
       3,    57,     3,   104,     3,     3,     3,    60,    82,     3,
      84,   550,   103,    89,    84,     3,    92,    93,    24,    36,
       3,   104,    39,    84,     3,    27,    32,    33,    34,    35,
     438,    84,   420,   103,    87,    20,    21,    83,    80,   104,
      86,   385,   103,    89,    80,    80,    92,    93,    97,    98,
      86,   395,    83,    80,    90,    90,    80,    81,   255,   105,
     257,     3,    86,    90,    83,    90,    90,   455,   103,   323,
      80,   449,   118,   119,    81,   151,   103,    80,    81,    80,
      86,    80,    80,    86,    81,    86,    80,    90,    86,    90,
     107,    90,    86,    82,   438,    83,    84,   441,   144,   101,
      83,    80,    80,   149,   318,   151,   360,    84,   543,   155,
      84,   499,   118,   119,   160,   161,    91,    92,   337,   191,
     192,   193,   168,   169,   170,   171,   103,    80,    37,   103,
     176,    80,   155,    83,    84,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   103,    80,   161,    80,   174,    79,   356,
     458,    80,   329,   169,    85,   103,    83,    84,    84,    83,
      84,   188,   248,   219,   388,   519,   222,    83,    84,   225,
      89,   348,   254,    92,    93,   315,   103,   103,   234,   103,
     444,   263,    84,   210,   538,    80,   219,   103,   254,    18,
     256,    86,   248,    83,    84,    84,   103,   263,    85,    28,
      29,   103,   278,    32,    33,    34,    35,    84,    37,    38,
     266,   475,   424,   103,   103,   439,    82,   424,   234,   246,
     484,    84,    83,    84,   310,    89,   103,    80,    80,   315,
      99,   150,   100,    86,    86,   154,   500,   256,    90,   503,
     103,   418,   103,   270,   508,     0,   273,    79,   275,   456,
     266,   416,   308,   477,   310,    81,    26,   323,    84,   315,
      89,   485,   318,   339,    93,   321,    80,    96,    97,   325,
       7,     8,    86,   359,   103,   539,   332,   333,   334,   108,
     109,   110,   338,   112,   340,   509,    80,    81,   315,   454,
     119,    82,    86,   349,   360,   559,    90,   373,    24,    25,
     103,    80,   378,   359,   323,    64,   570,    86,   532,    90,
     337,   377,   519,    80,    95,    96,    84,    85,   483,    86,
      84,    83,   338,    87,   340,    81,   382,    81,    84,   385,
      84,   538,   388,    83,   420,    17,    18,    19,   257,   395,
      83,   360,    83,    80,    81,   401,   175,    84,   375,    86,
      84,    81,    79,    90,    84,   520,    81,   433,   377,   415,
       7,     8,    81,   419,   420,    84,    83,    84,   424,   455,
     198,   199,   200,   201,    81,    84,    85,    84,   444,   435,
     209,   437,   438,   439,    84,   441,    87,    87,    85,   554,
      81,   447,    81,    84,    80,    84,   315,    81,    80,   455,
     456,   566,   458,    79,    86,    84,    88,   572,    81,   475,
      81,   330,    80,   499,    83,    84,    83,    84,   484,   194,
     195,   477,   196,   197,   480,   444,   202,   203,    84,   485,
      83,   447,   351,   103,   500,    79,    87,   503,    81,   103,
     359,    78,   508,   499,    90,    81,    87,    81,    62,   505,
      82,    84,    82,   509,    82,   103,   475,    81,    84,    81,
      87,   103,    81,   519,   102,   484,    83,   523,   103,    85,
      83,    81,    84,   539,    81,    81,   532,    81,   534,    87,
     536,   500,   538,    83,   503,    87,    81,   543,    87,   508,
      81,    85,   103,   559,   550,    87,   332,   416,   270,   204,
     206,   205,   308,   325,   570,   207,   425,   523,     3,   208,
     333,   477,   341,   342,   505,   477,   534,   538,   543,     1,
     539,     3,   558,   499,   521,     7,     8,   455,   151,   420,
      86,   456,    -1,    -1,    -1,   454,   455,   458,    -1,   160,
     559,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,   570,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,   160,    -1,   483,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    -1,
     499,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    -1,    -1,     0,     1,    -1,     3,     4,     5,     6,
      -1,   520,     9,    10,    11,    -1,    13,    14,    15,    16,
      -1,    18,    19,    -1,    -1,   102,    -1,    -1,    -1,    -1,
     449,    -1,   104,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    40,    41,   554,    43,    44,    45,    46,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   566,    -1,    -1,
      -1,    -1,    -1,   572,    61,    -1,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,     3,    -1,    80,    -1,    82,    -1,    -1,    -1,    86,
      -1,    -1,    89,    90,    91,    92,    93,    94,     0,     1,
      -1,     3,     4,     5,     6,    -1,   103,     9,    10,    11,
      -1,    13,    14,    15,    16,    -1,    18,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    40,    41,
      -1,    43,    44,    45,    46,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    79,    -1,    61,
      -1,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    -1,     3,    80,    -1,
      82,     7,     8,    -1,    86,    -1,    -1,    89,    90,    91,
      92,    93,    94,     1,    -1,     3,    -1,    -1,    -1,     7,
       8,   103,    -1,    -1,    12,    -1,     1,    -1,     3,    -1,
      -1,    -1,     7,     8,    -1,    -1,    -1,    12,    -1,    -1,
      -1,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    -1,    -1,    -1,    -1,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,     1,    -1,     3,    -1,    -1,
      -1,     7,     8,    -1,    -1,    83,    -1,   103,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    -1,
       1,    -1,     3,     4,     5,     6,   104,    -1,     9,    10,
      11,    -1,    13,    14,    15,    16,    -1,    18,    19,   104,
      -1,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    -1,    -1,    -1,    -1,    40,
      41,    42,    43,    44,    45,    46,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    -1,    -1,
      61,    -1,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,     3,   104,    80,
      -1,    82,    83,    -1,    -1,    86,    -1,    -1,    89,    90,
      91,    92,    93,    94,    -1,     1,    -1,     3,     4,     5,
       6,    -1,   103,     9,    10,    11,    -1,    13,    14,    15,
      16,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    40,    41,    42,    43,    44,    45,
      46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    -1,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,     3,    -1,    80,    -1,    82,    83,    -1,    -1,
      86,    -1,    -1,    89,    90,    91,    92,    93,    94,    -1,
       1,    -1,     3,     4,     5,     6,    -1,   103,     9,    10,
      11,    -1,    13,    14,    15,    16,    -1,    18,    19,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    40,
      41,    -1,    43,    44,    45,    46,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      61,    -1,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    -1,    -1,    80,
      -1,    82,    83,    -1,    -1,    86,    -1,    -1,    89,    90,
      91,    92,    93,    94,    -1,     1,    -1,     3,     4,     5,
       6,    -1,   103,     9,    10,    11,    -1,    13,    14,    15,
      16,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    40,    41,    -1,    43,    44,    45,
      46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    -1,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    -1,    -1,    80,    -1,    82,    83,    -1,    -1,
      86,    -1,    -1,    89,    90,    91,    92,    93,    94,    -1,
       1,    -1,     3,     4,     5,     6,    -1,   103,     9,    10,
      11,    -1,    13,    14,    15,    16,    -1,    18,    19,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,
      41,    -1,    43,    44,    45,    46,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      61,    -1,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    -1,    -1,    80,
      -1,    82,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,
      91,    92,    93,    94,    -1,    -1,    -1,     3,     4,     5,
       6,    -1,   103,     9,    10,    11,    -1,    -1,    -1,    15,
      16,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,    -1,    -1,     9,    10,    11,    -1,
      -1,    -1,    15,    16,    -1,    18,    19,    43,    -1,    45,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    44,    45,    46,    -1,    -1,    72,    73,    74,    75,
      76,    77,    -1,    -1,    80,    -1,    82,    -1,    -1,    -1,
      86,    -1,    -1,    89,    90,    91,    92,    93,    94,    72,
      73,    74,    75,    76,    77,    -1,    -1,    80,   104,    82,
      -1,    -1,    -1,    86,    -1,    -1,    89,    90,    91,    92,
      93,    94,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     103,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      -1,    -1,    -1,    15,    16,    -1,    18,    19,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    -1,    -1,    -1,    15,    16,    -1,    18,
      19,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    -1,
      -1,    -1,    -1,    -1,    43,    44,    45,    46,    -1,    -1,
      72,    73,    74,    75,    76,    77,    -1,    79,    80,    -1,
      82,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    91,
      92,    93,    94,    72,    73,    74,    75,    76,    77,    -1,
      -1,    80,    -1,    82,    -1,    -1,    85,    86,    87,    -1,
      89,    90,    91,    92,    93,    94,     3,     4,     5,     6,
      -1,     3,     9,    10,    11,     7,     8,    -1,    15,    16,
      -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,    -1,    -1,     9,    10,    11,    -1,    -1,
      -1,    15,    16,    -1,    18,    19,    43,    44,    45,    46,
      -1,    -1,    -1,    -1,    -1,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    43,
      44,    45,    46,    -1,    -1,    72,    73,    74,    75,    76,
      77,    -1,    -1,    80,    81,    82,    -1,    79,    -1,    86,
      -1,    -1,    89,    90,    91,    92,    93,    94,    72,    73,
      74,    75,    76,    77,    -1,    -1,    80,    -1,    82,    -1,
      -1,    -1,    86,    87,    -1,    89,    90,    91,    92,    93,
      94,     3,     4,     5,     6,    -1,     3,     9,    10,    11,
       7,     8,    -1,    15,    16,    -1,    18,    19,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    -1,    -1,    -1,    15,    16,    -1,    18,
      19,    43,    44,    45,    46,    -1,    -1,    -1,    -1,    -1,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    43,    44,    45,    46,    -1,    -1,
      72,    73,    74,    75,    76,    77,    -1,    -1,    80,    -1,
      82,    -1,    -1,    -1,    86,    87,    -1,    89,    90,    91,
      92,    93,    94,    72,    73,    74,    75,    76,    77,    -1,
      -1,    80,    -1,    82,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    91,    92,    93,    94,     3,     4,     5,     6,
      -1,     3,     9,    10,    11,     7,     8,    -1,    15,    16,
      -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,    -1,    -1,     9,    10,    11,    -1,    -1,
      -1,    15,    16,    -1,    18,    19,    43,    -1,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    43,
      -1,    45,    -1,    -1,    -1,    72,    73,    74,    75,    76,
      77,    -1,    -1,    80,    -1,    82,    -1,    -1,    -1,    86,
      87,    -1,    89,    90,    91,    92,    93,    94,    72,    73,
      74,    75,    76,    77,    -1,    -1,    80,    -1,    82,    -1,
      -1,    -1,    86,    87,    -1,    89,    90,    91,    92,    93,
      94,     3,     4,     5,     6,    -1,    -1,     9,    10,    11,
      -1,    -1,    -1,    15,    16,    -1,    18,    19,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    -1,    -1,    -1,    15,    16,    -1,    18,
      19,    43,    -1,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    43,    -1,    45,    -1,    -1,    -1,
      72,    73,    74,    75,    76,    77,    -1,    -1,    80,    -1,
      82,    -1,    -1,    -1,    86,    87,    -1,    89,    90,    91,
      92,    93,    94,    72,    73,    74,    75,    76,    77,    -1,
      -1,    80,    -1,    82,    -1,    -1,    -1,    86,    87,    -1,
      89,    90,    91,    92,    93,    94,     3,     4,     5,     6,
      -1,    -1,     9,    10,    11,    -1,    -1,    -1,    15,    16,
      -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,    -1,    -1,     9,    10,    11,    -1,    -1,
      -1,    15,    16,    -1,    18,    19,    43,    -1,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,
      -1,    45,    -1,    -1,    -1,    72,    73,    74,    75,    76,
      77,    -1,    -1,    80,    -1,    82,    -1,    -1,    -1,    86,
      87,    -1,    89,    90,    91,    92,    93,    94,    72,    73,
      74,    75,    76,    77,    -1,    -1,    80,    -1,    82,    -1,
      -1,    -1,    86,    87,    -1,    89,    90,    91,    92,    93,
      94,     3,     4,     5,     6,    -1,    -1,     9,    10,    11,
      -1,    -1,    -1,    15,    16,    -1,    18,    19,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    -1,    -1,    -1,    15,    16,    -1,    18,
      19,    43,    -1,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    43,    -1,    45,    -1,    -1,    -1,
      72,    73,    74,    75,    76,    77,    -1,    -1,    80,    -1,
      82,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    91,
      92,    93,    94,    72,    73,    74,    75,    76,    77,    -1,
      -1,    80,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    91,    92,    93,    94,     3,     4,     5,     6,
      -1,    -1,     9,    10,    11,    -1,    -1,    -1,    15,    16,
      -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,    -1,    -1,     9,    10,    11,    -1,    -1,
      -1,    15,    16,    -1,    18,    19,    43,    -1,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,
      -1,    45,    -1,    -1,    -1,    72,    73,    74,    75,    76,
      77,     3,    -1,    80,    -1,     7,     8,    -1,    -1,    86,
      -1,    -1,    89,    90,    91,    92,    93,    94,    72,    73,
      74,    75,    76,    77,    -1,    -1,    80,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89,    90,    91,    92,    93,
      94,    -1,    -1,    -1,    -1,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,     3,    -1,
      -1,    -1,     7,     8,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    81,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    81
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     1,     3,     4,     5,     6,     9,    10,    11,    13,
      14,    15,    16,    18,    19,    40,    41,    43,    44,    45,
      46,    61,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    80,    82,    86,
      89,    90,    91,    92,    93,    94,   103,   106,   108,   109,
     110,   111,   112,   113,   116,   117,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   139,   140,   141,   142,   188,   189,   192,   194,   195,
     196,   198,   199,   200,   201,   202,   203,   204,   103,    80,
     106,   119,    80,    80,   119,   106,    80,    80,   119,   119,
     106,   106,   135,    82,   142,    82,   192,    80,    80,    80,
      80,   188,    80,   103,   103,   103,   142,   142,    85,    82,
     142,   188,   142,   188,   142,   188,   142,   188,   141,     7,
       8,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    79,   106,   142,   148,   149,   153,
     157,   158,   174,   175,   176,    42,    83,   142,   188,   190,
     191,   193,    85,    87,   114,   115,   118,   141,    79,    85,
      80,    17,    18,    19,    80,    86,    88,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,   102,   134,   119,
     121,    90,    95,    96,    91,    92,    20,    21,    22,    23,
      97,    98,    24,    25,    89,    99,   100,    26,    27,   101,
      84,   103,   106,     0,   204,   174,   106,   174,   174,    82,
     142,   103,    84,   103,   142,    82,   141,   142,   142,   142,
      64,   142,   197,   103,    85,   188,   142,   188,    83,    83,
      83,    83,    83,    83,    83,    83,    84,   106,    79,    81,
     148,   106,   158,   176,    81,    80,    86,    90,   170,   184,
     185,   158,   135,    83,    83,   190,   193,    83,   188,    87,
      84,    87,    60,    84,    87,    85,   106,   188,    81,   135,
     136,   106,    81,   118,   141,   142,   106,   141,   121,   121,
     121,   122,   122,   123,   123,   124,   124,   124,   124,   125,
     125,   126,   127,   128,   129,   130,   142,   141,    80,    81,
      79,    81,    81,    83,   135,    84,   106,     1,    12,    57,
      58,    59,    83,   104,   106,   137,   138,   144,   146,   149,
     150,   151,   152,   158,   160,   163,    81,    84,    81,    81,
      81,    80,   103,   188,    83,    83,   141,   106,   149,    79,
     121,    81,   184,    87,   132,   143,   158,   170,   185,    80,
      86,   103,   121,    83,    87,   114,   141,    87,    87,   141,
     141,   192,    60,    81,    81,    84,    87,    85,    81,   136,
     106,    83,    84,   174,   103,   160,   106,   107,   143,    83,
     138,   158,   106,   107,   150,    80,   103,   106,   147,   166,
     167,   170,   118,   188,   192,   188,   142,   197,    78,   106,
     158,   159,    81,    87,   170,    59,    81,   106,   149,   152,
     158,   177,   178,   179,   180,   181,   182,   183,    87,   143,
     192,   132,   192,    81,    83,    84,   147,    82,   160,    82,
     166,    84,   103,    80,    86,   167,    81,    62,    81,   103,
     106,   159,   106,   181,    81,    84,    80,   106,   170,   184,
     186,   187,   158,    87,   192,   106,   103,    83,    84,   106,
     164,   165,   147,     1,    83,   104,   106,   154,   155,   156,
     160,    81,   166,    81,   104,   145,   171,   172,   173,    87,
     143,   188,   103,   197,   159,    60,   179,   186,   187,    80,
      86,    81,    83,   102,    83,    84,   103,   103,   104,   143,
      85,   155,   156,    83,   161,   162,   166,   159,   143,   160,
      81,    84,    87,    81,    81,    81,   177,    87,   143,   143,
      83,   165,   143,   103,   160,    83,    84,   103,    80,    86,
     166,   168,   169,   170,   159,    60,   173,   188,    81,    87,
     160,   103,   161,   162,    81,   168,    87,   143,    80,    86,
     169,   162,   103,   159,    81,    87,    81,   171,    87,   143,
      85,   159,    81,    87,   143,   159,   103
};


/* Prevent warning if -Wmissing-prototypes.  */
int yyparse (U *ctx);

/* Error token number */
#define YYTERROR 1

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */


#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N) ((void) 0)
#endif


#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#define YYLEX yylex (&yylval, ctx)


#undef yynerrs
#define yynerrs (yystackp->yyerrcnt)
#undef yychar
#define yychar (yystackp->yyrawchar)
#undef yylval
#define yylval (yystackp->yyval)
#undef yylloc
#define yylloc (yystackp->yyloc)


static const int YYEOF = 0;
static const int YYEMPTY = -2;

typedef enum { yyok, yyaccept, yyabort, yyerr } YYRESULTTAG;

#define YYCHK(YYE)							     \
   do { YYRESULTTAG yyflag = YYE; if (yyflag != yyok) return yyflag; }	     \
   while (YYID (0))

#if YYDEBUG

# ifndef YYFPRINTF
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, U *ctx)
{
  if (!yyvaluep)
    return;
  YYUSE (ctx);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, U *ctx)
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, ctx);
  YYFPRINTF (yyoutput, ")");
}

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			    \
do {									    \
  if (yydebug)								    \
    {									    \
      YYFPRINTF (stderr, "%s ", Title);					    \
      yy_symbol_print (stderr, Type,					    \
		       Value, ctx);  \
      YYFPRINTF (stderr, "\n");						    \
    }									    \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;

#else /* !YYDEBUG */

# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)

#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYMAXDEPTH * sizeof (GLRStackItem)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

/* Minimum number of free items on the stack allowed after an
   allocation.  This is to allow allocation and initialization
   to be completed by functions that call yyexpandGLRStack before the
   stack is expanded, thus insuring that all necessary pointers get
   properly redirected to new data.  */
#define YYHEADROOM 2

#ifndef YYSTACKEXPANDABLE
# if (! defined __cplusplus \
      || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL))
#  define YYSTACKEXPANDABLE 1
# else
#  define YYSTACKEXPANDABLE 0
# endif
#endif

#if YYSTACKEXPANDABLE
# define YY_RESERVE_GLRSTACK(Yystack)			\
  do {							\
    if (Yystack->yyspaceLeft < YYHEADROOM)		\
      yyexpandGLRStack (Yystack);			\
  } while (YYID (0))
#else
# define YY_RESERVE_GLRSTACK(Yystack)			\
  do {							\
    if (Yystack->yyspaceLeft < YYHEADROOM)		\
      yyMemoryExhausted (Yystack);			\
  } while (YYID (0))
#endif


#if YYERROR_VERBOSE

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static size_t
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      size_t yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return strlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

#endif /* !YYERROR_VERBOSE */

/** State numbers, as in LALR(1) machine */
typedef int yyStateNum;

/** Rule numbers, as in LALR(1) machine */
typedef int yyRuleNum;

/** Grammar symbol */
typedef short int yySymbol;

/** Item references, as in LALR(1) machine */
typedef short int yyItemNum;

typedef struct yyGLRState yyGLRState;
typedef struct yyGLRStateSet yyGLRStateSet;
typedef struct yySemanticOption yySemanticOption;
typedef union yyGLRStackItem yyGLRStackItem;
typedef struct yyGLRStack yyGLRStack;

struct yyGLRState {
  /** Type tag: always true.  */
  yybool yyisState;
  /** Type tag for yysemantics.  If true, yysval applies, otherwise
   *  yyfirstVal applies.  */
  yybool yyresolved;
  /** Number of corresponding LALR(1) machine state.  */
  yyStateNum yylrState;
  /** Preceding state in this stack */
  yyGLRState* yypred;
  /** Source position of the first token produced by my symbol */
  size_t yyposn;
  union {
    /** First in a chain of alternative reductions producing the
     *  non-terminal corresponding to this state, threaded through
     *  yynext.  */
    yySemanticOption* yyfirstVal;
    /** Semantic value for this state.  */
    YYSTYPE yysval;
  } yysemantics;
  /** Source location for this state.  */
  YYLTYPE yyloc;
};

struct yyGLRStateSet {
  yyGLRState** yystates;
  /** During nondeterministic operation, yylookaheadNeeds tracks which
   *  stacks have actually needed the current lookahead.  During deterministic
   *  operation, yylookaheadNeeds[0] is not maintained since it would merely
   *  duplicate yychar != YYEMPTY.  */
  yybool* yylookaheadNeeds;
  size_t yysize, yycapacity;
};

struct yySemanticOption {
  /** Type tag: always false.  */
  yybool yyisState;
  /** Rule number for this reduction */
  yyRuleNum yyrule;
  /** The last RHS state in the list of states to be reduced.  */
  yyGLRState* yystate;
  /** The lookahead for this reduction.  */
  int yyrawchar;
  YYSTYPE yyval;
  YYLTYPE yyloc;
  /** Next sibling in chain of options.  To facilitate merging,
   *  options are chained in decreasing order by address.  */
  yySemanticOption* yynext;
};

/** Type of the items in the GLR stack.  The yyisState field
 *  indicates which item of the union is valid.  */
union yyGLRStackItem {
  yyGLRState yystate;
  yySemanticOption yyoption;
};

struct yyGLRStack {
  int yyerrState;


  int yyerrcnt;
  int yyrawchar;
  YYSTYPE yyval;
  YYLTYPE yyloc;

  YYJMP_BUF yyexception_buffer;
  yyGLRStackItem* yyitems;
  yyGLRStackItem* yynextFree;
  size_t yyspaceLeft;
  yyGLRState* yysplitPoint;
  yyGLRState* yylastDeleted;
  yyGLRStateSet yytops;
};

#if YYSTACKEXPANDABLE
static void yyexpandGLRStack (yyGLRStack* yystackp);
#endif

static void yyFail (yyGLRStack* yystackp, U *ctx, const char* yymsg)
  __attribute__ ((__noreturn__));
static void
yyFail (yyGLRStack* yystackp, U *ctx, const char* yymsg)
{
  if (yymsg != NULL)
    yyerror (ctx, yymsg);
  YYLONGJMP (yystackp->yyexception_buffer, 1);
}

static void yyMemoryExhausted (yyGLRStack* yystackp)
  __attribute__ ((__noreturn__));
static void
yyMemoryExhausted (yyGLRStack* yystackp)
{
  YYLONGJMP (yystackp->yyexception_buffer, 2);
}

#if YYDEBUG || YYERROR_VERBOSE
/** A printable representation of TOKEN.  */
static inline const char*
yytokenName (yySymbol yytoken)
{
  if (yytoken == YYEMPTY)
    return "";

  return yytname[yytoken];
}
#endif

/** Fill in YYVSP[YYLOW1 .. YYLOW0-1] from the chain of states starting
 *  at YYVSP[YYLOW0].yystate.yypred.  Leaves YYVSP[YYLOW1].yystate.yypred
 *  containing the pointer to the next state in the chain.  */
static void yyfillin (yyGLRStackItem *, int, int) __attribute__ ((__unused__));
static void
yyfillin (yyGLRStackItem *yyvsp, int yylow0, int yylow1)
{
  yyGLRState* s;
  int i;
  s = yyvsp[yylow0].yystate.yypred;
  for (i = yylow0-1; i >= yylow1; i -= 1)
    {
      YYASSERT (s->yyresolved);
      yyvsp[i].yystate.yyresolved = yytrue;
      yyvsp[i].yystate.yysemantics.yysval = s->yysemantics.yysval;
      yyvsp[i].yystate.yyloc = s->yyloc;
      s = yyvsp[i].yystate.yypred = s->yypred;
    }
}

/* Do nothing if YYNORMAL or if *YYLOW <= YYLOW1.  Otherwise, fill in
 * YYVSP[YYLOW1 .. *YYLOW-1] as in yyfillin and set *YYLOW = YYLOW1.
 * For convenience, always return YYLOW1.  */
static inline int yyfill (yyGLRStackItem *, int *, int, yybool)
     __attribute__ ((__unused__));
static inline int
yyfill (yyGLRStackItem *yyvsp, int *yylow, int yylow1, yybool yynormal)
{
  if (!yynormal && yylow1 < *yylow)
    {
      yyfillin (yyvsp, *yylow, yylow1);
      *yylow = yylow1;
    }
  return yylow1;
}

/** Perform user action for rule number YYN, with RHS length YYRHSLEN,
 *  and top stack item YYVSP.  YYLVALP points to place to put semantic
 *  value ($$), and yylocp points to place for location information
 *  (@$).  Returns yyok for normal return, yyaccept for YYACCEPT,
 *  yyerr for YYERROR, yyabort for YYABORT.  */
/*ARGSUSED*/ static YYRESULTTAG
yyuserAction (yyRuleNum yyn, int yyrhslen, yyGLRStackItem* yyvsp,
	      YYSTYPE* yyvalp,
	      YYLTYPE* YYOPTIONAL_LOC (yylocp),
	      yyGLRStack* yystackp
	      , U *ctx)
{
  yybool yynormal __attribute__ ((__unused__)) =
    (yystackp->yysplitPoint == NULL);
  int yylow;
  YYUSE (ctx);
# undef yyerrok
# define yyerrok (yystackp->yyerrState = 0)
# undef YYACCEPT
# define YYACCEPT return yyaccept
# undef YYABORT
# define YYABORT return yyabort
# undef YYERROR
# define YYERROR return yyerrok, yyerr
# undef YYRECOVERING
# define YYRECOVERING() (yystackp->yyerrState != 0)
# undef yyclearin
# define yyclearin (yychar = YYEMPTY)
# undef YYFILL
# define YYFILL(N) yyfill (yyvsp, &yylow, N, yynormal)
# undef YYBACKUP
# define YYBACKUP(Token, Value)						     \
  return yyerror (ctx, YY_("syntax error: cannot back up")),     \
	 yyerrok, yyerr

  yylow = 1;
  if (yyrhslen == 0)
    *yyvalp = yyval_default;
  else
    *yyvalp = yyvsp[YYFILL (1-yyrhslen)].yystate.yysemantics.yysval;
  YYLLOC_DEFAULT ((*yylocp), (yyvsp - yyrhslen), yyrhslen);

  switch (yyn)
    {
        case 2:

/* Line 936 of glr.c  */
#line 93 "c.y"
    { ((*yyvalp).expr) = doidnsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 4:

/* Line 936 of glr.c  */
#line 98 "c.y"
    { ((*yyvalp).expr) = 0 ;}
    break;

  case 5:

/* Line 936 of glr.c  */
#line 103 "c.y"
    { ((*yyvalp).expr) = dotickesrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 7:

/* Line 936 of glr.c  */
#line 112 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elambda, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 8:

/* Line 936 of glr.c  */
#line 114 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elambda, nullelist(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 9:

/* Line 936 of glr.c  */
#line 119 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefrec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 10:

/* Line 936 of glr.c  */
#line 121 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefrec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 11:

/* Line 936 of glr.c  */
#line 123 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefrec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 12:

/* Line 936 of glr.c  */
#line 128 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 13:

/* Line 936 of glr.c  */
#line 133 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Equote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 14:

/* Line 936 of glr.c  */
#line 135 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Equote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 15:

/* Line 936 of glr.c  */
#line 137 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Equote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 16:

/* Line 936 of glr.c  */
#line 139 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Equote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 17:

/* Line 936 of glr.c  */
#line 141 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Equote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 18:

/* Line 936 of glr.c  */
#line 143 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Equote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 19:

/* Line 936 of glr.c  */
#line 145 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Equote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 20:

/* Line 936 of glr.c  */
#line 147 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Equote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 21:

/* Line 936 of glr.c  */
#line 149 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Equote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 22:

/* Line 936 of glr.c  */
#line 151 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Equote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 23:

/* Line 936 of glr.c  */
#line 156 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, Eelist,
				     (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0),
			  0, 0);
	;}
    break;

  case 24:

/* Line 936 of glr.c  */
#line 166 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 25:

/* Line 936 of glr.c  */
#line 168 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 28:

/* Line 936 of glr.c  */
#line 175 "c.y"
    { ((*yyvalp).expr) = dosymsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 29:

/* Line 936 of glr.c  */
#line 177 "c.y"
    { ((*yyvalp).expr) = doconst(ctx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 30:

/* Line 936 of glr.c  */
#line 179 "c.y"
    { ((*yyvalp).expr) = doconstssrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 31:

/* Line 936 of glr.c  */
#line 181 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 32:

/* Line 936 of glr.c  */
#line 183 "c.y"
    { Expr *tl = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0);
          Expr *l = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), tl, 0, 0); 
          /* FIXME: should be a pair; what constructor to use? */
          ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, l, 0, 0, 0); ;}
    break;

  case 33:

/* Line 936 of glr.c  */
#line 188 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, nullelist(), 0, 0, 0); ;}
    break;

  case 34:

/* Line 936 of glr.c  */
#line 190 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 35:

/* Line 936 of glr.c  */
#line 192 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 36:

/* Line 936 of glr.c  */
#line 194 "c.y"
    { Expr *ell = newexprsrc(&ctx->inp->src, Eellipsis, 0, 0, 0, 0);
          ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, 
                          invert(newexprsrc(&ctx->inp->src, Eelist, 
                                            ell, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), 0, 0)), 0, 0, 0); ;}
    break;

  case 37:

/* Line 936 of glr.c  */
#line 199 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, nullelist(), 0, 0, 0); ;}
    break;

  case 38:

/* Line 936 of glr.c  */
#line 201 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 39:

/* Line 936 of glr.c  */
#line 203 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 45:

/* Line 936 of glr.c  */
#line 213 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 46:

/* Line 936 of glr.c  */
#line 215 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 47:

/* Line 936 of glr.c  */
#line 217 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 48:

/* Line 936 of glr.c  */
#line 219 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edot, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 49:

/* Line 936 of glr.c  */
#line 221 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earrow, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 50:

/* Line 936 of glr.c  */
#line 223 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epostinc, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 51:

/* Line 936 of glr.c  */
#line 225 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epostdec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 52:

/* Line 936 of glr.c  */
#line 227 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Econtainer, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 53:

/* Line 936 of glr.c  */
#line 232 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 54:

/* Line 936 of glr.c  */
#line 234 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 56:

/* Line 936 of glr.c  */
#line 240 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epreinc, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 57:

/* Line 936 of glr.c  */
#line 242 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epredec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 58:

/* Line 936 of glr.c  */
#line 244 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 59:

/* Line 936 of glr.c  */
#line 246 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Esizeofe, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 60:

/* Line 936 of glr.c  */
#line 248 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Esizeoft, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 61:

/* Line 936 of glr.c  */
#line 250 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypeofe, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 62:

/* Line 936 of glr.c  */
#line 252 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypeoft, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 63:

/* Line 936 of glr.c  */
#line 254 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Emkctype, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 64:

/* Line 936 of glr.c  */
#line 259 "c.y"
    { ((*yyvalp).kind) = Eref; ;}
    break;

  case 65:

/* Line 936 of glr.c  */
#line 261 "c.y"
    { ((*yyvalp).kind) = Ederef; ;}
    break;

  case 66:

/* Line 936 of glr.c  */
#line 263 "c.y"
    { ((*yyvalp).kind) = Euplus; ;}
    break;

  case 67:

/* Line 936 of glr.c  */
#line 265 "c.y"
    { ((*yyvalp).kind) = Euminus; ;}
    break;

  case 68:

/* Line 936 of glr.c  */
#line 267 "c.y"
    { ((*yyvalp).kind) = Eutwiddle; ;}
    break;

  case 69:

/* Line 936 of glr.c  */
#line 269 "c.y"
    { ((*yyvalp).kind) = Eunot; ;}
    break;

  case 71:

/* Line 936 of glr.c  */
#line 275 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecast, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 72:

/* Line 936 of glr.c  */
#line 277 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Excast, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 74:

/* Line 936 of glr.c  */
#line 283 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Emul, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 75:

/* Line 936 of glr.c  */
#line 285 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ediv, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 76:

/* Line 936 of glr.c  */
#line 287 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Emod, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 78:

/* Line 936 of glr.c  */
#line 293 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eadd, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 79:

/* Line 936 of glr.c  */
#line 295 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Esub, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 81:

/* Line 936 of glr.c  */
#line 301 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eshl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 82:

/* Line 936 of glr.c  */
#line 303 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eshr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 84:

/* Line 936 of glr.c  */
#line 309 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Elt, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 85:

/* Line 936 of glr.c  */
#line 311 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Egt, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 86:

/* Line 936 of glr.c  */
#line 313 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ele, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 87:

/* Line 936 of glr.c  */
#line 315 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ege, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 89:

/* Line 936 of glr.c  */
#line 321 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eeq, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 90:

/* Line 936 of glr.c  */
#line 323 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eneq, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 92:

/* Line 936 of glr.c  */
#line 329 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eband, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 94:

/* Line 936 of glr.c  */
#line 335 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ebxor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 96:

/* Line 936 of glr.c  */
#line 341 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ebor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 98:

/* Line 936 of glr.c  */
#line 347 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eland, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 100:

/* Line 936 of glr.c  */
#line 353 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 102:

/* Line 936 of glr.c  */
#line 359 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Econd, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 104:

/* Line 936 of glr.c  */
#line 365 "c.y"
    { if((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.kind) == Eg)
	  	((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0);
	  else
	  	((*yyvalp).expr) = newgopsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr));
	;}
    break;

  case 105:

/* Line 936 of glr.c  */
#line 374 "c.y"
    { ((*yyvalp).kind) = Eg; ;}
    break;

  case 106:

/* Line 936 of glr.c  */
#line 376 "c.y"
    { ((*yyvalp).kind) = Emul; ;}
    break;

  case 107:

/* Line 936 of glr.c  */
#line 378 "c.y"
    { ((*yyvalp).kind) = Ediv; ;}
    break;

  case 108:

/* Line 936 of glr.c  */
#line 380 "c.y"
    { ((*yyvalp).kind) = Emod; ;}
    break;

  case 109:

/* Line 936 of glr.c  */
#line 382 "c.y"
    { ((*yyvalp).kind) = Eadd; ;}
    break;

  case 110:

/* Line 936 of glr.c  */
#line 384 "c.y"
    { ((*yyvalp).kind) = Esub; ;}
    break;

  case 111:

/* Line 936 of glr.c  */
#line 386 "c.y"
    { ((*yyvalp).kind) = Eshl; ;}
    break;

  case 112:

/* Line 936 of glr.c  */
#line 388 "c.y"
    { ((*yyvalp).kind) = Eshr; ;}
    break;

  case 113:

/* Line 936 of glr.c  */
#line 390 "c.y"
    { ((*yyvalp).kind) = Eband; ;}
    break;

  case 114:

/* Line 936 of glr.c  */
#line 392 "c.y"
    { ((*yyvalp).kind) = Ebxor; ;}
    break;

  case 115:

/* Line 936 of glr.c  */
#line 394 "c.y"
    { ((*yyvalp).kind) = Ebor; ;}
    break;

  case 116:

/* Line 936 of glr.c  */
#line 399 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 117:

/* Line 936 of glr.c  */
#line 401 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 118:

/* Line 936 of glr.c  */
#line 406 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 119:

/* Line 936 of glr.c  */
#line 408 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
			  newexprsrc(&ctx->inp->src, Eellipsis, 0, 0, 0, 0),
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0);
	;}
    break;

  case 120:

/* Line 936 of glr.c  */
#line 416 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 121:

/* Line 936 of glr.c  */
#line 418 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 124:

/* Line 936 of glr.c  */
#line 425 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 126:

/* Line 936 of glr.c  */
#line 431 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Enames, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 127:

/* Line 936 of glr.c  */
#line 433 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Enames, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 128:

/* Line 936 of glr.c  */
#line 438 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elapply, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 129:

/* Line 936 of glr.c  */
#line 440 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elapply, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 133:

/* Line 936 of glr.c  */
#line 451 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecomma, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 135:

/* Line 936 of glr.c  */
#line 460 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 136:

/* Line 936 of glr.c  */
#line 465 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 137:

/* Line 936 of glr.c  */
#line 467 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 138:

/* Line 936 of glr.c  */
#line 472 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecls, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 139:

/* Line 936 of glr.c  */
#line 474 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecls, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 140:

/* Line 936 of glr.c  */
#line 476 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecls, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 141:

/* Line 936 of glr.c  */
#line 481 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 142:

/* Line 936 of glr.c  */
#line 483 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 143:

/* Line 936 of glr.c  */
#line 488 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Evoid, 0, 0, 0, 0); ;}
    break;

  case 144:

/* Line 936 of glr.c  */
#line 490 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebool, 0, 0, 0, 0); ;}
    break;

  case 145:

/* Line 936 of glr.c  */
#line 492 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Echar, 0, 0, 0, 0); ;}
    break;

  case 146:

/* Line 936 of glr.c  */
#line 494 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eshort, 0, 0, 0, 0); ;}
    break;

  case 147:

/* Line 936 of glr.c  */
#line 496 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eint, 0, 0, 0, 0); ;}
    break;

  case 148:

/* Line 936 of glr.c  */
#line 498 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elong, 0, 0, 0, 0); ;}
    break;

  case 149:

/* Line 936 of glr.c  */
#line 500 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efloat, 0, 0, 0, 0); ;}
    break;

  case 150:

/* Line 936 of glr.c  */
#line 502 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edouble, 0, 0, 0, 0); ;}
    break;

  case 151:

/* Line 936 of glr.c  */
#line 504 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Esigned, 0, 0, 0, 0); ;}
    break;

  case 152:

/* Line 936 of glr.c  */
#line 506 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eunsigned, 0, 0, 0, 0); ;}
    break;

  case 153:

/* Line 936 of glr.c  */
#line 510 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 154:

/* Line 936 of glr.c  */
#line 512 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 155:

/* Line 936 of glr.c  */
#line 517 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 156:

/* Line 936 of glr.c  */
#line 519 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 159:

/* Line 936 of glr.c  */
#line 526 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 160:

/* Line 936 of glr.c  */
#line 528 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), Znull(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 161:

/* Line 936 of glr.c  */
#line 530 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 162:

/* Line 936 of glr.c  */
#line 532 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 163:

/* Line 936 of glr.c  */
#line 537 "c.y"
    { ((*yyvalp).kind) = Estruct; ;}
    break;

  case 164:

/* Line 936 of glr.c  */
#line 539 "c.y"
    { ((*yyvalp).kind) = Eunion; ;}
    break;

  case 165:

/* Line 936 of glr.c  */
#line 544 "c.y"
    { ((*yyvalp).kind) = Estruct; ;}
    break;

  case 166:

/* Line 936 of glr.c  */
#line 546 "c.y"
    { ((*yyvalp).kind) = Eunion; ;}
    break;

  case 167:

/* Line 936 of glr.c  */
#line 548 "c.y"
    { ((*yyvalp).kind) = Eenum; ;}
    break;

  case 168:

/* Line 936 of glr.c  */
#line 553 "c.y"
    {
		/* labels yield null struct_declarations */
		if((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr))
			((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
					(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0);
		else
			((*yyvalp).expr) = Znull();
	;}
    break;

  case 169:

/* Line 936 of glr.c  */
#line 562 "c.y"
    { 
		/* labels yield null struct_declarations */
	 	if((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr))
			((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0);
		else
			((*yyvalp).expr) = Znull();
	;}
    break;

  case 170:

/* Line 936 of glr.c  */
#line 573 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efields, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 171:

/* Line 936 of glr.c  */
#line 575 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efields, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), Znull(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 172:

/* Line 936 of glr.c  */
#line 577 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efields, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 173:

/* Line 936 of glr.c  */
#line 579 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebitfield, (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 174:

/* Line 936 of glr.c  */
#line 582 "c.y"
    { freeexpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr)); ((*yyvalp).expr) = 0; ;}
    break;

  case 175:

/* Line 936 of glr.c  */
#line 584 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 176:

/* Line 936 of glr.c  */
#line 589 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 184:

/* Line 936 of glr.c  */
#line 611 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 185:

/* Line 936 of glr.c  */
#line 613 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 186:

/* Line 936 of glr.c  */
#line 618 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 187:

/* Line 936 of glr.c  */
#line 620 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 189:

/* Line 936 of glr.c  */
#line 629 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 190:

/* Line 936 of glr.c  */
#line 631 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 191:

/* Line 936 of glr.c  */
#line 633 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 192:

/* Line 936 of glr.c  */
#line 635 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 193:

/* Line 936 of glr.c  */
#line 637 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 194:

/* Line 936 of glr.c  */
#line 642 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 195:

/* Line 936 of glr.c  */
#line 644 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 196:

/* Line 936 of glr.c  */
#line 651 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenumel, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 197:

/* Line 936 of glr.c  */
#line 653 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenumel, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 198:

/* Line 936 of glr.c  */
#line 658 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 200:

/* Line 936 of glr.c  */
#line 664 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 201:

/* Line 936 of glr.c  */
#line 666 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 202:

/* Line 936 of glr.c  */
#line 668 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 203:

/* Line 936 of glr.c  */
#line 670 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 204:

/* Line 936 of glr.c  */
#line 672 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 205:

/* Line 936 of glr.c  */
#line 674 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 208:

/* Line 936 of glr.c  */
#line 681 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 209:

/* Line 936 of glr.c  */
#line 686 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 210:

/* Line 936 of glr.c  */
#line 688 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, 0, 0, 0); ;}
    break;

  case 211:

/* Line 936 of glr.c  */
#line 690 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 212:

/* Line 936 of glr.c  */
#line 692 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 213:

/* Line 936 of glr.c  */
#line 694 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 214:

/* Line 936 of glr.c  */
#line 696 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, 0, Znull(), 0, 0); ;}
    break;

  case 215:

/* Line 936 of glr.c  */
#line 698 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 216:

/* Line 936 of glr.c  */
#line 700 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 217:

/* Line 936 of glr.c  */
#line 705 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, 0, 0, 0, 0); ;}
    break;

  case 218:

/* Line 936 of glr.c  */
#line 707 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, 0, 0, 0, 0); ;}
    break;

  case 219:

/* Line 936 of glr.c  */
#line 709 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 220:

/* Line 936 of glr.c  */
#line 711 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 221:

/* Line 936 of glr.c  */
#line 716 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 222:

/* Line 936 of glr.c  */
#line 718 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 223:

/* Line 936 of glr.c  */
#line 723 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 224:

/* Line 936 of glr.c  */
#line 725 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 225:

/* Line 936 of glr.c  */
#line 730 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 226:

/* Line 936 of glr.c  */
#line 732 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 227:

/* Line 936 of glr.c  */
#line 734 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 228:

/* Line 936 of glr.c  */
#line 739 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypename, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 229:

/* Line 936 of glr.c  */
#line 741 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypename, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 231:

/* Line 936 of glr.c  */
#line 747 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 232:

/* Line 936 of glr.c  */
#line 749 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 233:

/* Line 936 of glr.c  */
#line 754 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 234:

/* Line 936 of glr.c  */
#line 756 "c.y"
    { ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0)); ;}
    break;

  case 235:

/* Line 936 of glr.c  */
#line 759 "c.y"
    { ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0)); ;}
    break;

  case 236:

/* Line 936 of glr.c  */
#line 762 "c.y"
    { ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, 0,
			  newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0)); ;}
    break;

  case 237:

/* Line 936 of glr.c  */
#line 765 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 238:

/* Line 936 of glr.c  */
#line 767 "c.y"
    { ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0)); ;}
    break;

  case 239:

/* Line 936 of glr.c  */
#line 772 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 240:

/* Line 936 of glr.c  */
#line 774 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 241:

/* Line 936 of glr.c  */
#line 779 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 242:

/* Line 936 of glr.c  */
#line 781 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 243:

/* Line 936 of glr.c  */
#line 786 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 244:

/* Line 936 of glr.c  */
#line 788 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 245:

/* Line 936 of glr.c  */
#line 790 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 247:

/* Line 936 of glr.c  */
#line 796 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 248:

/* Line 936 of glr.c  */
#line 798 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 249:

/* Line 936 of glr.c  */
#line 803 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 250:

/* Line 936 of glr.c  */
#line 805 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 253:

/* Line 936 of glr.c  */
#line 812 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 254:

/* Line 936 of glr.c  */
#line 817 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 257:

/* Line 936 of glr.c  */
#line 824 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 258:

/* Line 936 of glr.c  */
#line 829 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 259:

/* Line 936 of glr.c  */
#line 831 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, 0, 0, 0); ;}
    break;

  case 260:

/* Line 936 of glr.c  */
#line 833 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 261:

/* Line 936 of glr.c  */
#line 835 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 262:

/* Line 936 of glr.c  */
#line 837 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 263:

/* Line 936 of glr.c  */
#line 839 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, 0, nullelist(), 0, 0); ;}
    break;

  case 264:

/* Line 936 of glr.c  */
#line 841 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 265:

/* Line 936 of glr.c  */
#line 843 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 266:

/* Line 936 of glr.c  */
#line 848 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 268:

/* Line 936 of glr.c  */
#line 854 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 269:

/* Line 936 of glr.c  */
#line 856 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 270:

/* Line 936 of glr.c  */
#line 858 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 271:

/* Line 936 of glr.c  */
#line 860 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 272:

/* Line 936 of glr.c  */
#line 862 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 273:

/* Line 936 of glr.c  */
#line 864 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 282:

/* Line 936 of glr.c  */
#line 880 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eglobal, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 283:

/* Line 936 of glr.c  */
#line 885 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 284:

/* Line 936 of glr.c  */
#line 890 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 285:

/* Line 936 of glr.c  */
#line 892 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 286:

/* Line 936 of glr.c  */
#line 897 "c.y"
    {
		((*yyvalp).expr) = newexprsrc(&ctx->inp->src,
				Escope,
				newexprsrc(&ctx->inp->src,
					   Eblock,
					   nullelist(),
					   nullelist(), 0, 0),
				0, 0, 0);
	;}
    break;

  case 287:

/* Line 936 of glr.c  */
#line 907 "c.y"
    {
		((*yyvalp).expr) = newexprsrc(&ctx->inp->src,
				Escope,
				newexprsrc(&ctx->inp->src,
					   Eblock,
					   flatten((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)),
					   nullelist(), 0, 0),
				0, 0, 0);
	;}
    break;

  case 288:

/* Line 936 of glr.c  */
#line 917 "c.y"
    {
		/* use src of first statement */
		Expr *sl;
		sl = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr));
		((*yyvalp).expr) = newexprsrc(&sl->src,
				Escope,
				newexprsrc(&sl->src,
					   Eblock,
					   nullelist(),
					   sl, 0, 0),
				0, 0, 0);
	;}
    break;

  case 289:

/* Line 936 of glr.c  */
#line 930 "c.y"
    {
		/* use src of first statement */
		Expr *sl;
		sl = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr));
		((*yyvalp).expr) = newexprsrc(&sl->src,
				Escope,
				newexprsrc(&sl->src,
					   Eblock,
					   flatten((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)),
					   sl, 0, 0),
				0, 0, 0);
	;}
    break;

  case 290:

/* Line 936 of glr.c  */
#line 946 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 291:

/* Line 936 of glr.c  */
#line 948 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 292:

/* Line 936 of glr.c  */
#line 953 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Enil, 0, 0, 0, 0); ;}
    break;

  case 293:

/* Line 936 of glr.c  */
#line 955 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 294:

/* Line 936 of glr.c  */
#line 957 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 295:

/* Line 936 of glr.c  */
#line 962 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eif, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 296:

/* Line 936 of glr.c  */
#line 964 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eif, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (7))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (7))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (7))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 297:

/* Line 936 of glr.c  */
#line 967 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eswitch, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 298:

/* Line 936 of glr.c  */
#line 972 "c.y"
    { ((*yyvalp).expr) = Zlabelsrc(&(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr)->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 299:

/* Line 936 of glr.c  */
#line 974 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), NULL, NULL); ;}
    break;

  case 300:

/* Line 936 of glr.c  */
#line 976 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefault, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), NULL, NULL, NULL); ;}
    break;

  case 302:

/* Line 936 of glr.c  */
#line 982 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 303:

/* Line 936 of glr.c  */
#line 987 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ewhile, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 304:

/* Line 936 of glr.c  */
#line 989 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edo, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (7))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 305:

/* Line 936 of glr.c  */
#line 991 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (9))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (9))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (9))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((9) - (9))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 306:

/* Line 936 of glr.c  */
#line 996 "c.y"
    { ((*yyvalp).expr) = Zgotosrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 307:

/* Line 936 of glr.c  */
#line 998 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Econtinue, 0, 0, 0, 0); ;}
    break;

  case 308:

/* Line 936 of glr.c  */
#line 1000 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebreak, 0, 0, 0, 0); ;}
    break;

  case 309:

/* Line 936 of glr.c  */
#line 1002 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eret, 0, 0, 0, 0); ;}
    break;

  case 310:

/* Line 936 of glr.c  */
#line 1004 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eret, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 311:

/* Line 936 of glr.c  */
#line 1009 "c.y"
    { ((*yyvalp).kind) = Edefine; ;}
    break;

  case 312:

/* Line 936 of glr.c  */
#line 1011 "c.y"
    { ((*yyvalp).kind) = Edefloc; ;}
    break;

  case 313:

/* Line 936 of glr.c  */
#line 1016 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr)->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (6))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 314:

/* Line 936 of glr.c  */
#line 1018 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr)->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), nullelist(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 319:

/* Line 936 of glr.c  */
#line 1032 "c.y"
    { dotop(ctx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr)); ;}
    break;



/* Line 936 of glr.c  */
#line 3992 "c.tab.c"
      default: break;
    }

  return yyok;
# undef yyerrok
# undef YYABORT
# undef YYACCEPT
# undef YYERROR
# undef YYBACKUP
# undef yyclearin
# undef YYRECOVERING
}


/*ARGSUSED*/ static void
yyuserMerge (int yyn, YYSTYPE* yy0, YYSTYPE* yy1)
{
  YYUSE (yy0);
  YYUSE (yy1);

  switch (yyn)
    {
        case 1: yy0->expr = ofmerge (*yy0, *yy1); break;
  case 2: yy0->expr = castmerge (*yy0, *yy1); break;
  case 3: yy0->expr = mulmerge (*yy0, *yy1); break;
  case 4: yy0->expr = andmerge (*yy0, *yy1); break;

      default: break;
    }
}

			      /* Bison grammar-table manipulation.  */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, U *ctx)
{
  YYUSE (yyvaluep);
  YYUSE (ctx);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/** Number of symbols composing the right hand side of rule #RULE.  */
static inline int
yyrhsLength (yyRuleNum yyrule)
{
  return yyr2[yyrule];
}

static void
yydestroyGLRState (char const *yymsg, yyGLRState *yys, U *ctx)
{
  if (yys->yyresolved)
    yydestruct (yymsg, yystos[yys->yylrState],
		&yys->yysemantics.yysval, ctx);
  else
    {
#if YYDEBUG
      if (yydebug)
	{
	  if (yys->yysemantics.yyfirstVal)
	    YYFPRINTF (stderr, "%s unresolved ", yymsg);
	  else
	    YYFPRINTF (stderr, "%s incomplete ", yymsg);
	  yy_symbol_print (stderr, yystos[yys->yylrState],
			   NULL, ctx);
	  YYFPRINTF (stderr, "\n");
	}
#endif

      if (yys->yysemantics.yyfirstVal)
	{
	  yySemanticOption *yyoption = yys->yysemantics.yyfirstVal;
	  yyGLRState *yyrh;
	  int yyn;
	  for (yyrh = yyoption->yystate, yyn = yyrhsLength (yyoption->yyrule);
	       yyn > 0;
	       yyrh = yyrh->yypred, yyn -= 1)
	    yydestroyGLRState (yymsg, yyrh, ctx);
	}
    }
}

/** Left-hand-side symbol for rule #RULE.  */
static inline yySymbol
yylhsNonterm (yyRuleNum yyrule)
{
  return yyr1[yyrule];
}

#define yyis_pact_ninf(yystate) \
  ((yystate) == YYPACT_NINF)

/** True iff LR state STATE has only a default reduction (regardless
 *  of token).  */
static inline yybool
yyisDefaultedState (yyStateNum yystate)
{
  return yyis_pact_ninf (yypact[yystate]);
}

/** The default reduction for STATE, assuming it has one.  */
static inline yyRuleNum
yydefaultAction (yyStateNum yystate)
{
  return yydefact[yystate];
}

#define yyis_table_ninf(yytable_value) \
  YYID (0)

/** Set *YYACTION to the action to take in YYSTATE on seeing YYTOKEN.
 *  Result R means
 *    R < 0:  Reduce on rule -R.
 *    R = 0:  Error.
 *    R > 0:  Shift to state R.
 *  Set *CONFLICTS to a pointer into yyconfl to 0-terminated list of
 *  conflicting reductions.
 */
static inline void
yygetLRActions (yyStateNum yystate, int yytoken,
		int* yyaction, const short int** yyconflicts)
{
  int yyindex = yypact[yystate] + yytoken;
  if (yyindex < 0 || YYLAST < yyindex || yycheck[yyindex] != yytoken)
    {
      *yyaction = -yydefact[yystate];
      *yyconflicts = yyconfl;
    }
  else if (! yyis_table_ninf (yytable[yyindex]))
    {
      *yyaction = yytable[yyindex];
      *yyconflicts = yyconfl + yyconflp[yyindex];
    }
  else
    {
      *yyaction = 0;
      *yyconflicts = yyconfl + yyconflp[yyindex];
    }
}

static inline yyStateNum
yyLRgotoState (yyStateNum yystate, yySymbol yylhs)
{
  int yyr;
  yyr = yypgoto[yylhs - YYNTOKENS] + yystate;
  if (0 <= yyr && yyr <= YYLAST && yycheck[yyr] == yystate)
    return yytable[yyr];
  else
    return yydefgoto[yylhs - YYNTOKENS];
}

static inline yybool
yyisShiftAction (int yyaction)
{
  return 0 < yyaction;
}

static inline yybool
yyisErrorAction (int yyaction)
{
  return yyaction == 0;
}

				/* GLRStates */

/** Return a fresh GLRStackItem.  Callers should call
 * YY_RESERVE_GLRSTACK afterwards to make sure there is sufficient
 * headroom.  */

static inline yyGLRStackItem*
yynewGLRStackItem (yyGLRStack* yystackp, yybool yyisState)
{
  yyGLRStackItem* yynewItem = yystackp->yynextFree;
  yystackp->yyspaceLeft -= 1;
  yystackp->yynextFree += 1;
  yynewItem->yystate.yyisState = yyisState;
  return yynewItem;
}

/** Add a new semantic action that will execute the action for rule
 *  RULENUM on the semantic values in RHS to the list of
 *  alternative actions for STATE.  Assumes that RHS comes from
 *  stack #K of *STACKP. */
static void
yyaddDeferredAction (yyGLRStack* yystackp, size_t yyk, yyGLRState* yystate,
		     yyGLRState* rhs, yyRuleNum yyrule)
{
  yySemanticOption* yynewOption =
    &yynewGLRStackItem (yystackp, yyfalse)->yyoption;
  yynewOption->yystate = rhs;
  yynewOption->yyrule = yyrule;
  if (yystackp->yytops.yylookaheadNeeds[yyk])
    {
      yynewOption->yyrawchar = yychar;
      yynewOption->yyval = yylval;
      yynewOption->yyloc = yylloc;
    }
  else
    yynewOption->yyrawchar = YYEMPTY;
  yynewOption->yynext = yystate->yysemantics.yyfirstVal;
  yystate->yysemantics.yyfirstVal = yynewOption;

  YY_RESERVE_GLRSTACK (yystackp);
}

				/* GLRStacks */

/** Initialize SET to a singleton set containing an empty stack.  */
static yybool
yyinitStateSet (yyGLRStateSet* yyset)
{
  yyset->yysize = 1;
  yyset->yycapacity = 16;
  yyset->yystates = (yyGLRState**) YYMALLOC (16 * sizeof yyset->yystates[0]);
  if (! yyset->yystates)
    return yyfalse;
  yyset->yystates[0] = NULL;
  yyset->yylookaheadNeeds =
    (yybool*) YYMALLOC (16 * sizeof yyset->yylookaheadNeeds[0]);
  if (! yyset->yylookaheadNeeds)
    {
      YYFREE (yyset->yystates);
      return yyfalse;
    }
  return yytrue;
}

static void yyfreeStateSet (yyGLRStateSet* yyset)
{
  YYFREE (yyset->yystates);
  YYFREE (yyset->yylookaheadNeeds);
}

/** Initialize STACK to a single empty stack, with total maximum
 *  capacity for all stacks of SIZE.  */
static yybool
yyinitGLRStack (yyGLRStack* yystackp, size_t yysize)
{
  yystackp->yyerrState = 0;
  yynerrs = 0;
  yystackp->yyspaceLeft = yysize;
  yystackp->yyitems =
    (yyGLRStackItem*) YYMALLOC (yysize * sizeof yystackp->yynextFree[0]);
  if (!yystackp->yyitems)
    return yyfalse;
  yystackp->yynextFree = yystackp->yyitems;
  yystackp->yysplitPoint = NULL;
  yystackp->yylastDeleted = NULL;
  return yyinitStateSet (&yystackp->yytops);
}


#if YYSTACKEXPANDABLE
# define YYRELOC(YYFROMITEMS,YYTOITEMS,YYX,YYTYPE) \
  &((YYTOITEMS) - ((YYFROMITEMS) - (yyGLRStackItem*) (YYX)))->YYTYPE

/** If STACK is expandable, extend it.  WARNING: Pointers into the
    stack from outside should be considered invalid after this call.
    We always expand when there are 1 or fewer items left AFTER an
    allocation, so that we can avoid having external pointers exist
    across an allocation.  */
static void
yyexpandGLRStack (yyGLRStack* yystackp)
{
  yyGLRStackItem* yynewItems;
  yyGLRStackItem* yyp0, *yyp1;
  size_t yysize, yynewSize;
  size_t yyn;
  yysize = yystackp->yynextFree - yystackp->yyitems;
  if (YYMAXDEPTH - YYHEADROOM < yysize)
    yyMemoryExhausted (yystackp);
  yynewSize = 2*yysize;
  if (YYMAXDEPTH < yynewSize)
    yynewSize = YYMAXDEPTH;
  yynewItems = (yyGLRStackItem*) YYMALLOC (yynewSize * sizeof yynewItems[0]);
  if (! yynewItems)
    yyMemoryExhausted (yystackp);
  for (yyp0 = yystackp->yyitems, yyp1 = yynewItems, yyn = yysize;
       0 < yyn;
       yyn -= 1, yyp0 += 1, yyp1 += 1)
    {
      *yyp1 = *yyp0;
      if (*(yybool *) yyp0)
	{
	  yyGLRState* yys0 = &yyp0->yystate;
	  yyGLRState* yys1 = &yyp1->yystate;
	  if (yys0->yypred != NULL)
	    yys1->yypred =
	      YYRELOC (yyp0, yyp1, yys0->yypred, yystate);
	  if (! yys0->yyresolved && yys0->yysemantics.yyfirstVal != NULL)
	    yys1->yysemantics.yyfirstVal =
	      YYRELOC(yyp0, yyp1, yys0->yysemantics.yyfirstVal, yyoption);
	}
      else
	{
	  yySemanticOption* yyv0 = &yyp0->yyoption;
	  yySemanticOption* yyv1 = &yyp1->yyoption;
	  if (yyv0->yystate != NULL)
	    yyv1->yystate = YYRELOC (yyp0, yyp1, yyv0->yystate, yystate);
	  if (yyv0->yynext != NULL)
	    yyv1->yynext = YYRELOC (yyp0, yyp1, yyv0->yynext, yyoption);
	}
    }
  if (yystackp->yysplitPoint != NULL)
    yystackp->yysplitPoint = YYRELOC (yystackp->yyitems, yynewItems,
				 yystackp->yysplitPoint, yystate);

  for (yyn = 0; yyn < yystackp->yytops.yysize; yyn += 1)
    if (yystackp->yytops.yystates[yyn] != NULL)
      yystackp->yytops.yystates[yyn] =
	YYRELOC (yystackp->yyitems, yynewItems,
		 yystackp->yytops.yystates[yyn], yystate);
  YYFREE (yystackp->yyitems);
  yystackp->yyitems = yynewItems;
  yystackp->yynextFree = yynewItems + yysize;
  yystackp->yyspaceLeft = yynewSize - yysize;
}
#endif

static void
yyfreeGLRStack (yyGLRStack* yystackp)
{
  YYFREE (yystackp->yyitems);
  yyfreeStateSet (&yystackp->yytops);
}

/** Assuming that S is a GLRState somewhere on STACK, update the
 *  splitpoint of STACK, if needed, so that it is at least as deep as
 *  S.  */
static inline void
yyupdateSplit (yyGLRStack* yystackp, yyGLRState* yys)
{
  if (yystackp->yysplitPoint != NULL && yystackp->yysplitPoint > yys)
    yystackp->yysplitPoint = yys;
}

/** Invalidate stack #K in STACK.  */
static inline void
yymarkStackDeleted (yyGLRStack* yystackp, size_t yyk)
{
  if (yystackp->yytops.yystates[yyk] != NULL)
    yystackp->yylastDeleted = yystackp->yytops.yystates[yyk];
  yystackp->yytops.yystates[yyk] = NULL;
}

/** Undelete the last stack that was marked as deleted.  Can only be
    done once after a deletion, and only when all other stacks have
    been deleted.  */
static void
yyundeleteLastStack (yyGLRStack* yystackp)
{
  if (yystackp->yylastDeleted == NULL || yystackp->yytops.yysize != 0)
    return;
  yystackp->yytops.yystates[0] = yystackp->yylastDeleted;
  yystackp->yytops.yysize = 1;
  YYDPRINTF ((stderr, "Restoring last deleted stack as stack #0.\n"));
  yystackp->yylastDeleted = NULL;
}

static inline void
yyremoveDeletes (yyGLRStack* yystackp)
{
  size_t yyi, yyj;
  yyi = yyj = 0;
  while (yyj < yystackp->yytops.yysize)
    {
      if (yystackp->yytops.yystates[yyi] == NULL)
	{
	  if (yyi == yyj)
	    {
	      YYDPRINTF ((stderr, "Removing dead stacks.\n"));
	    }
	  yystackp->yytops.yysize -= 1;
	}
      else
	{
	  yystackp->yytops.yystates[yyj] = yystackp->yytops.yystates[yyi];
	  /* In the current implementation, it's unnecessary to copy
	     yystackp->yytops.yylookaheadNeeds[yyi] since, after
	     yyremoveDeletes returns, the parser immediately either enters
	     deterministic operation or shifts a token.  However, it doesn't
	     hurt, and the code might evolve to need it.  */
	  yystackp->yytops.yylookaheadNeeds[yyj] =
	    yystackp->yytops.yylookaheadNeeds[yyi];
	  if (yyj != yyi)
	    {
	      YYDPRINTF ((stderr, "Rename stack %lu -> %lu.\n",
			  (unsigned long int) yyi, (unsigned long int) yyj));
	    }
	  yyj += 1;
	}
      yyi += 1;
    }
}

/** Shift to a new state on stack #K of STACK, corresponding to LR state
 * LRSTATE, at input position POSN, with (resolved) semantic value SVAL.  */
static inline void
yyglrShift (yyGLRStack* yystackp, size_t yyk, yyStateNum yylrState,
	    size_t yyposn,
	    YYSTYPE* yyvalp, YYLTYPE* yylocp)
{
  yyGLRState* yynewState = &yynewGLRStackItem (yystackp, yytrue)->yystate;

  yynewState->yylrState = yylrState;
  yynewState->yyposn = yyposn;
  yynewState->yyresolved = yytrue;
  yynewState->yypred = yystackp->yytops.yystates[yyk];
  yynewState->yysemantics.yysval = *yyvalp;
  yynewState->yyloc = *yylocp;
  yystackp->yytops.yystates[yyk] = yynewState;

  YY_RESERVE_GLRSTACK (yystackp);
}

/** Shift stack #K of YYSTACK, to a new state corresponding to LR
 *  state YYLRSTATE, at input position YYPOSN, with the (unresolved)
 *  semantic value of YYRHS under the action for YYRULE.  */
static inline void
yyglrShiftDefer (yyGLRStack* yystackp, size_t yyk, yyStateNum yylrState,
		 size_t yyposn, yyGLRState* rhs, yyRuleNum yyrule)
{
  yyGLRState* yynewState = &yynewGLRStackItem (yystackp, yytrue)->yystate;

  yynewState->yylrState = yylrState;
  yynewState->yyposn = yyposn;
  yynewState->yyresolved = yyfalse;
  yynewState->yypred = yystackp->yytops.yystates[yyk];
  yynewState->yysemantics.yyfirstVal = NULL;
  yystackp->yytops.yystates[yyk] = yynewState;

  /* Invokes YY_RESERVE_GLRSTACK.  */
  yyaddDeferredAction (yystackp, yyk, yynewState, rhs, yyrule);
}

/** Pop the symbols consumed by reduction #RULE from the top of stack
 *  #K of STACK, and perform the appropriate semantic action on their
 *  semantic values.  Assumes that all ambiguities in semantic values
 *  have been previously resolved.  Set *VALP to the resulting value,
 *  and *LOCP to the computed location (if any).  Return value is as
 *  for userAction.  */
static inline YYRESULTTAG
yydoAction (yyGLRStack* yystackp, size_t yyk, yyRuleNum yyrule,
	    YYSTYPE* yyvalp, YYLTYPE* yylocp, U *ctx)
{
  int yynrhs = yyrhsLength (yyrule);

  if (yystackp->yysplitPoint == NULL)
    {
      /* Standard special case: single stack.  */
      yyGLRStackItem* rhs = (yyGLRStackItem*) yystackp->yytops.yystates[yyk];
      YYASSERT (yyk == 0);
      yystackp->yynextFree -= yynrhs;
      yystackp->yyspaceLeft += yynrhs;
      yystackp->yytops.yystates[0] = & yystackp->yynextFree[-1].yystate;
      return yyuserAction (yyrule, yynrhs, rhs,
			   yyvalp, yylocp, yystackp, ctx);
    }
  else
    {
      /* At present, doAction is never called in nondeterministic
       * mode, so this branch is never taken.  It is here in
       * anticipation of a future feature that will allow immediate
       * evaluation of selected actions in nondeterministic mode.  */
      int yyi;
      yyGLRState* yys;
      yyGLRStackItem yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
      yys = yyrhsVals[YYMAXRHS + YYMAXLEFT].yystate.yypred
	= yystackp->yytops.yystates[yyk];
      for (yyi = 0; yyi < yynrhs; yyi += 1)
	{
	  yys = yys->yypred;
	  YYASSERT (yys);
	}
      yyupdateSplit (yystackp, yys);
      yystackp->yytops.yystates[yyk] = yys;
      return yyuserAction (yyrule, yynrhs, yyrhsVals + YYMAXRHS + YYMAXLEFT - 1,
			   yyvalp, yylocp, yystackp, ctx);
    }
}

#if !YYDEBUG
# define YY_REDUCE_PRINT(Args)
#else
# define YY_REDUCE_PRINT(Args)		\
do {					\
  if (yydebug)				\
    yy_reduce_print Args;		\
} while (YYID (0))

/*----------------------------------------------------------.
| Report that the RULE is going to be reduced on stack #K.  |
`----------------------------------------------------------*/

/*ARGSUSED*/ static inline void
yy_reduce_print (yyGLRStack* yystackp, size_t yyk, yyRuleNum yyrule,
		 YYSTYPE* yyvalp, YYLTYPE* yylocp, U *ctx)
{
  int yynrhs = yyrhsLength (yyrule);
  yybool yynormal __attribute__ ((__unused__)) =
    (yystackp->yysplitPoint == NULL);
  yyGLRStackItem* yyvsp = (yyGLRStackItem*) yystackp->yytops.yystates[yyk];
  int yylow = 1;
  int yyi;
  YYUSE (yyvalp);
  YYUSE (yylocp);
  YYUSE (ctx);
  YYFPRINTF (stderr, "Reducing stack %lu by rule %d (line %lu):\n",
	     (unsigned long int) yyk, yyrule - 1,
	     (unsigned long int) yyrline[yyrule]);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(((yyGLRStackItem const *)yyvsp)[YYFILL ((yyi + 1) - (yynrhs))].yystate.yysemantics.yysval)
		       		       , ctx);
      YYFPRINTF (stderr, "\n");
    }
}
#endif

/** Pop items off stack #K of STACK according to grammar rule RULE,
 *  and push back on the resulting nonterminal symbol.  Perform the
 *  semantic action associated with RULE and store its value with the
 *  newly pushed state, if FORCEEVAL or if STACK is currently
 *  unambiguous.  Otherwise, store the deferred semantic action with
 *  the new state.  If the new state would have an identical input
 *  position, LR state, and predecessor to an existing state on the stack,
 *  it is identified with that existing state, eliminating stack #K from
 *  the STACK.  In this case, the (necessarily deferred) semantic value is
 *  added to the options for the existing state's semantic value.
 */
static inline YYRESULTTAG
yyglrReduce (yyGLRStack* yystackp, size_t yyk, yyRuleNum yyrule,
	     yybool yyforceEval, U *ctx)
{
  size_t yyposn = yystackp->yytops.yystates[yyk]->yyposn;

  if (yyforceEval || yystackp->yysplitPoint == NULL)
    {
      YYSTYPE yysval;
      YYLTYPE yyloc;

      YY_REDUCE_PRINT ((yystackp, yyk, yyrule, &yysval, &yyloc, ctx));
      YYCHK (yydoAction (yystackp, yyk, yyrule, &yysval,
			 &yyloc, ctx));
      YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyrule], &yysval, &yyloc);
      yyglrShift (yystackp, yyk,
		  yyLRgotoState (yystackp->yytops.yystates[yyk]->yylrState,
				 yylhsNonterm (yyrule)),
		  yyposn, &yysval, &yyloc);
    }
  else
    {
      size_t yyi;
      int yyn;
      yyGLRState* yys, *yys0 = yystackp->yytops.yystates[yyk];
      yyStateNum yynewLRState;

      for (yys = yystackp->yytops.yystates[yyk], yyn = yyrhsLength (yyrule);
	   0 < yyn; yyn -= 1)
	{
	  yys = yys->yypred;
	  YYASSERT (yys);
	}
      yyupdateSplit (yystackp, yys);
      yynewLRState = yyLRgotoState (yys->yylrState, yylhsNonterm (yyrule));
      YYDPRINTF ((stderr,
		  "Reduced stack %lu by rule #%d; action deferred.  Now in state %d.\n",
		  (unsigned long int) yyk, yyrule - 1, yynewLRState));
      for (yyi = 0; yyi < yystackp->yytops.yysize; yyi += 1)
	if (yyi != yyk && yystackp->yytops.yystates[yyi] != NULL)
	  {
	    yyGLRState* yyp, *yysplit = yystackp->yysplitPoint;
	    yyp = yystackp->yytops.yystates[yyi];
	    while (yyp != yys && yyp != yysplit && yyp->yyposn >= yyposn)
	      {
		if (yyp->yylrState == yynewLRState && yyp->yypred == yys)
		  {
		    yyaddDeferredAction (yystackp, yyk, yyp, yys0, yyrule);
		    yymarkStackDeleted (yystackp, yyk);
		    YYDPRINTF ((stderr, "Merging stack %lu into stack %lu.\n",
				(unsigned long int) yyk,
				(unsigned long int) yyi));
		    return yyok;
		  }
		yyp = yyp->yypred;
	      }
	  }
      yystackp->yytops.yystates[yyk] = yys;
      yyglrShiftDefer (yystackp, yyk, yynewLRState, yyposn, yys0, yyrule);
    }
  return yyok;
}

static size_t
yysplitStack (yyGLRStack* yystackp, size_t yyk)
{
  if (yystackp->yysplitPoint == NULL)
    {
      YYASSERT (yyk == 0);
      yystackp->yysplitPoint = yystackp->yytops.yystates[yyk];
    }
  if (yystackp->yytops.yysize >= yystackp->yytops.yycapacity)
    {
      yyGLRState** yynewStates;
      yybool* yynewLookaheadNeeds;

      yynewStates = NULL;

      if (yystackp->yytops.yycapacity
	  > (YYSIZEMAX / (2 * sizeof yynewStates[0])))
	yyMemoryExhausted (yystackp);
      yystackp->yytops.yycapacity *= 2;

      yynewStates =
	(yyGLRState**) YYREALLOC (yystackp->yytops.yystates,
				  (yystackp->yytops.yycapacity
				   * sizeof yynewStates[0]));
      if (yynewStates == NULL)
	yyMemoryExhausted (yystackp);
      yystackp->yytops.yystates = yynewStates;

      yynewLookaheadNeeds =
	(yybool*) YYREALLOC (yystackp->yytops.yylookaheadNeeds,
			     (yystackp->yytops.yycapacity
			      * sizeof yynewLookaheadNeeds[0]));
      if (yynewLookaheadNeeds == NULL)
	yyMemoryExhausted (yystackp);
      yystackp->yytops.yylookaheadNeeds = yynewLookaheadNeeds;
    }
  yystackp->yytops.yystates[yystackp->yytops.yysize]
    = yystackp->yytops.yystates[yyk];
  yystackp->yytops.yylookaheadNeeds[yystackp->yytops.yysize]
    = yystackp->yytops.yylookaheadNeeds[yyk];
  yystackp->yytops.yysize += 1;
  return yystackp->yytops.yysize-1;
}

/** True iff Y0 and Y1 represent identical options at the top level.
 *  That is, they represent the same rule applied to RHS symbols
 *  that produce the same terminal symbols.  */
static yybool
yyidenticalOptions (yySemanticOption* yyy0, yySemanticOption* yyy1)
{
  if (yyy0->yyrule == yyy1->yyrule)
    {
      yyGLRState *yys0, *yys1;
      int yyn;
      for (yys0 = yyy0->yystate, yys1 = yyy1->yystate,
	   yyn = yyrhsLength (yyy0->yyrule);
	   yyn > 0;
	   yys0 = yys0->yypred, yys1 = yys1->yypred, yyn -= 1)
	if (yys0->yyposn != yys1->yyposn)
	  return yyfalse;
      return yytrue;
    }
  else
    return yyfalse;
}

/** Assuming identicalOptions (Y0,Y1), destructively merge the
 *  alternative semantic values for the RHS-symbols of Y1 and Y0.  */
static void
yymergeOptionSets (yySemanticOption* yyy0, yySemanticOption* yyy1)
{
  yyGLRState *yys0, *yys1;
  int yyn;
  for (yys0 = yyy0->yystate, yys1 = yyy1->yystate,
       yyn = yyrhsLength (yyy0->yyrule);
       yyn > 0;
       yys0 = yys0->yypred, yys1 = yys1->yypred, yyn -= 1)
    {
      if (yys0 == yys1)
	break;
      else if (yys0->yyresolved)
	{
	  yys1->yyresolved = yytrue;
	  yys1->yysemantics.yysval = yys0->yysemantics.yysval;
	}
      else if (yys1->yyresolved)
	{
	  yys0->yyresolved = yytrue;
	  yys0->yysemantics.yysval = yys1->yysemantics.yysval;
	}
      else
	{
	  yySemanticOption** yyz0p;
	  yySemanticOption* yyz1;
	  yyz0p = &yys0->yysemantics.yyfirstVal;
	  yyz1 = yys1->yysemantics.yyfirstVal;
	  while (YYID (yytrue))
	    {
	      if (yyz1 == *yyz0p || yyz1 == NULL)
		break;
	      else if (*yyz0p == NULL)
		{
		  *yyz0p = yyz1;
		  break;
		}
	      else if (*yyz0p < yyz1)
		{
		  yySemanticOption* yyz = *yyz0p;
		  *yyz0p = yyz1;
		  yyz1 = yyz1->yynext;
		  (*yyz0p)->yynext = yyz;
		}
	      yyz0p = &(*yyz0p)->yynext;
	    }
	  yys1->yysemantics.yyfirstVal = yys0->yysemantics.yyfirstVal;
	}
    }
}

/** Y0 and Y1 represent two possible actions to take in a given
 *  parsing state; return 0 if no combination is possible,
 *  1 if user-mergeable, 2 if Y0 is preferred, 3 if Y1 is preferred.  */
static int
yypreference (yySemanticOption* y0, yySemanticOption* y1)
{
  yyRuleNum r0 = y0->yyrule, r1 = y1->yyrule;
  int p0 = yydprec[r0], p1 = yydprec[r1];

  if (p0 == p1)
    {
      if (yymerger[r0] == 0 || yymerger[r0] != yymerger[r1])
	return 0;
      else
	return 1;
    }
  if (p0 == 0 || p1 == 0)
    return 0;
  if (p0 < p1)
    return 3;
  if (p1 < p0)
    return 2;
  return 0;
}

static YYRESULTTAG yyresolveValue (yyGLRState* yys,
				   yyGLRStack* yystackp, U *ctx);


/** Resolve the previous N states starting at and including state S.  If result
 *  != yyok, some states may have been left unresolved possibly with empty
 *  semantic option chains.  Regardless of whether result = yyok, each state
 *  has been left with consistent data so that yydestroyGLRState can be invoked
 *  if necessary.  */
static YYRESULTTAG
yyresolveStates (yyGLRState* yys, int yyn,
		 yyGLRStack* yystackp, U *ctx)
{
  if (0 < yyn)
    {
      YYASSERT (yys->yypred);
      YYCHK (yyresolveStates (yys->yypred, yyn-1, yystackp, ctx));
      if (! yys->yyresolved)
	YYCHK (yyresolveValue (yys, yystackp, ctx));
    }
  return yyok;
}

/** Resolve the states for the RHS of OPT, perform its user action, and return
 *  the semantic value and location.  Regardless of whether result = yyok, all
 *  RHS states have been destroyed (assuming the user action destroys all RHS
 *  semantic values if invoked).  */
static YYRESULTTAG
yyresolveAction (yySemanticOption* yyopt, yyGLRStack* yystackp,
		 YYSTYPE* yyvalp, YYLTYPE* yylocp, U *ctx)
{
  yyGLRStackItem yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
  int yynrhs;
  int yychar_current;
  YYSTYPE yylval_current;
  YYLTYPE yylloc_current;
  YYRESULTTAG yyflag;

  yynrhs = yyrhsLength (yyopt->yyrule);
  yyflag = yyresolveStates (yyopt->yystate, yynrhs, yystackp, ctx);
  if (yyflag != yyok)
    {
      yyGLRState *yys;
      for (yys = yyopt->yystate; yynrhs > 0; yys = yys->yypred, yynrhs -= 1)
	yydestroyGLRState ("Cleanup: popping", yys, ctx);
      return yyflag;
    }

  yyrhsVals[YYMAXRHS + YYMAXLEFT].yystate.yypred = yyopt->yystate;
  yychar_current = yychar;
  yylval_current = yylval;
  yylloc_current = yylloc;
  yychar = yyopt->yyrawchar;
  yylval = yyopt->yyval;
  yylloc = yyopt->yyloc;
  yyflag = yyuserAction (yyopt->yyrule, yynrhs,
			   yyrhsVals + YYMAXRHS + YYMAXLEFT - 1,
			   yyvalp, yylocp, yystackp, ctx);
  yychar = yychar_current;
  yylval = yylval_current;
  yylloc = yylloc_current;
  return yyflag;
}

#if YYDEBUG
static void
yyreportTree (yySemanticOption* yyx, int yyindent)
{
  int yynrhs = yyrhsLength (yyx->yyrule);
  int yyi;
  yyGLRState* yys;
  yyGLRState* yystates[1 + YYMAXRHS];
  yyGLRState yyleftmost_state;

  for (yyi = yynrhs, yys = yyx->yystate; 0 < yyi; yyi -= 1, yys = yys->yypred)
    yystates[yyi] = yys;
  if (yys == NULL)
    {
      yyleftmost_state.yyposn = 0;
      yystates[0] = &yyleftmost_state;
    }
  else
    yystates[0] = yys;

  if (yyx->yystate->yyposn < yys->yyposn + 1)
    YYFPRINTF (stderr, "%*s%s -> <Rule %d, empty>\n",
	       yyindent, "", yytokenName (yylhsNonterm (yyx->yyrule)),
	       yyx->yyrule - 1);
  else
    YYFPRINTF (stderr, "%*s%s -> <Rule %d, tokens %lu .. %lu>\n",
	       yyindent, "", yytokenName (yylhsNonterm (yyx->yyrule)),
	       yyx->yyrule - 1, (unsigned long int) (yys->yyposn + 1),
	       (unsigned long int) yyx->yystate->yyposn);
  for (yyi = 1; yyi <= yynrhs; yyi += 1)
    {
      if (yystates[yyi]->yyresolved)
	{
	  if (yystates[yyi-1]->yyposn+1 > yystates[yyi]->yyposn)
	    YYFPRINTF (stderr, "%*s%s <empty>\n", yyindent+2, "",
		       yytokenName (yyrhs[yyprhs[yyx->yyrule]+yyi-1]));
	  else
	    YYFPRINTF (stderr, "%*s%s <tokens %lu .. %lu>\n", yyindent+2, "",
		       yytokenName (yyrhs[yyprhs[yyx->yyrule]+yyi-1]),
		       (unsigned long int) (yystates[yyi - 1]->yyposn + 1),
		       (unsigned long int) yystates[yyi]->yyposn);
	}
      else
	yyreportTree (yystates[yyi]->yysemantics.yyfirstVal, yyindent+2);
    }
}
#endif

/*ARGSUSED*/ static YYRESULTTAG
yyreportAmbiguity (yySemanticOption* yyx0,
		   yySemanticOption* yyx1, U *ctx)
{
  YYUSE (yyx0);
  YYUSE (yyx1);

#if YYDEBUG
  YYFPRINTF (stderr, "Ambiguity detected.\n");
  YYFPRINTF (stderr, "Option 1,\n");
  yyreportTree (yyx0, 2);
  YYFPRINTF (stderr, "\nOption 2,\n");
  yyreportTree (yyx1, 2);
  YYFPRINTF (stderr, "\n");
#endif

  yyerror (ctx, YY_("syntax is ambiguous"));
  return yyabort;
}

/** Starting at and including state S1, resolve the location for each of the
 *  previous N1 states that is unresolved.  The first semantic option of a state
 *  is always chosen.  */
static void
yyresolveLocations (yyGLRState* yys1, int yyn1,
		    yyGLRStack *yystackp, U *ctx)
{
  if (0 < yyn1)
    {
      yyresolveLocations (yys1->yypred, yyn1 - 1, yystackp, ctx);
      if (!yys1->yyresolved)
	{
	  yySemanticOption *yyoption;
	  yyGLRStackItem yyrhsloc[1 + YYMAXRHS];
	  int yynrhs;
	  int yychar_current;
	  YYSTYPE yylval_current;
	  YYLTYPE yylloc_current;
	  yyoption = yys1->yysemantics.yyfirstVal;
	  YYASSERT (yyoption != NULL);
	  yynrhs = yyrhsLength (yyoption->yyrule);
	  if (yynrhs > 0)
	    {
	      yyGLRState *yys;
	      int yyn;
	      yyresolveLocations (yyoption->yystate, yynrhs,
				  yystackp, ctx);
	      for (yys = yyoption->yystate, yyn = yynrhs;
		   yyn > 0;
		   yys = yys->yypred, yyn -= 1)
		yyrhsloc[yyn].yystate.yyloc = yys->yyloc;
	    }
	  else
	    {
	      /* Both yyresolveAction and yyresolveLocations traverse the GSS
		 in reverse rightmost order.  It is only necessary to invoke
		 yyresolveLocations on a subforest for which yyresolveAction
		 would have been invoked next had an ambiguity not been
		 detected.  Thus the location of the previous state (but not
		 necessarily the previous state itself) is guaranteed to be
		 resolved already.  */
	      yyGLRState *yyprevious = yyoption->yystate;
	      yyrhsloc[0].yystate.yyloc = yyprevious->yyloc;
	    }
	  yychar_current = yychar;
	  yylval_current = yylval;
	  yylloc_current = yylloc;
	  yychar = yyoption->yyrawchar;
	  yylval = yyoption->yyval;
	  yylloc = yyoption->yyloc;
	  YYLLOC_DEFAULT ((yys1->yyloc), yyrhsloc, yynrhs);
	  yychar = yychar_current;
	  yylval = yylval_current;
	  yylloc = yylloc_current;
	}
    }
}

/** Resolve the ambiguity represented in state S, perform the indicated
 *  actions, and set the semantic value of S.  If result != yyok, the chain of
 *  semantic options in S has been cleared instead or it has been left
 *  unmodified except that redundant options may have been removed.  Regardless
 *  of whether result = yyok, S has been left with consistent data so that
 *  yydestroyGLRState can be invoked if necessary.  */
static YYRESULTTAG
yyresolveValue (yyGLRState* yys, yyGLRStack* yystackp, U *ctx)
{
  yySemanticOption* yyoptionList = yys->yysemantics.yyfirstVal;
  yySemanticOption* yybest;
  yySemanticOption** yypp;
  yybool yymerge;
  YYSTYPE yysval;
  YYRESULTTAG yyflag;
  YYLTYPE *yylocp = &yys->yyloc;

  yybest = yyoptionList;
  yymerge = yyfalse;
  for (yypp = &yyoptionList->yynext; *yypp != NULL; )
    {
      yySemanticOption* yyp = *yypp;

      if (yyidenticalOptions (yybest, yyp))
	{
	  yymergeOptionSets (yybest, yyp);
	  *yypp = yyp->yynext;
	}
      else
	{
	  switch (yypreference (yybest, yyp))
	    {
	    case 0:
	      yyresolveLocations (yys, 1, yystackp, ctx);
	      return yyreportAmbiguity (yybest, yyp, ctx);
	      break;
	    case 1:
	      yymerge = yytrue;
	      break;
	    case 2:
	      break;
	    case 3:
	      yybest = yyp;
	      yymerge = yyfalse;
	      break;
	    default:
	      /* This cannot happen so it is not worth a YYASSERT (yyfalse),
		 but some compilers complain if the default case is
		 omitted.  */
	      break;
	    }
	  yypp = &yyp->yynext;
	}
    }

  if (yymerge)
    {
      yySemanticOption* yyp;
      int yyprec = yydprec[yybest->yyrule];
      yyflag = yyresolveAction (yybest, yystackp, &yysval,
				yylocp, ctx);
      if (yyflag == yyok)
	for (yyp = yybest->yynext; yyp != NULL; yyp = yyp->yynext)
	  {
	    if (yyprec == yydprec[yyp->yyrule])
	      {
		YYSTYPE yysval_other;
		YYLTYPE yydummy;
		yyflag = yyresolveAction (yyp, yystackp, &yysval_other,
					  &yydummy, ctx);
		if (yyflag != yyok)
		  {
		    yydestruct ("Cleanup: discarding incompletely merged value for",
				yystos[yys->yylrState],
				&yysval, ctx);
		    break;
		  }
		yyuserMerge (yymerger[yyp->yyrule], &yysval, &yysval_other);
	      }
	  }
    }
  else
    yyflag = yyresolveAction (yybest, yystackp, &yysval, yylocp, ctx);

  if (yyflag == yyok)
    {
      yys->yyresolved = yytrue;
      yys->yysemantics.yysval = yysval;
    }
  else
    yys->yysemantics.yyfirstVal = NULL;
  return yyflag;
}

static YYRESULTTAG
yyresolveStack (yyGLRStack* yystackp, U *ctx)
{
  if (yystackp->yysplitPoint != NULL)
    {
      yyGLRState* yys;
      int yyn;

      for (yyn = 0, yys = yystackp->yytops.yystates[0];
	   yys != yystackp->yysplitPoint;
	   yys = yys->yypred, yyn += 1)
	continue;
      YYCHK (yyresolveStates (yystackp->yytops.yystates[0], yyn, yystackp
			     , ctx));
    }
  return yyok;
}

static void
yycompressStack (yyGLRStack* yystackp)
{
  yyGLRState* yyp, *yyq, *yyr;

  if (yystackp->yytops.yysize != 1 || yystackp->yysplitPoint == NULL)
    return;

  for (yyp = yystackp->yytops.yystates[0], yyq = yyp->yypred, yyr = NULL;
       yyp != yystackp->yysplitPoint;
       yyr = yyp, yyp = yyq, yyq = yyp->yypred)
    yyp->yypred = yyr;

  yystackp->yyspaceLeft += yystackp->yynextFree - yystackp->yyitems;
  yystackp->yynextFree = ((yyGLRStackItem*) yystackp->yysplitPoint) + 1;
  yystackp->yyspaceLeft -= yystackp->yynextFree - yystackp->yyitems;
  yystackp->yysplitPoint = NULL;
  yystackp->yylastDeleted = NULL;

  while (yyr != NULL)
    {
      yystackp->yynextFree->yystate = *yyr;
      yyr = yyr->yypred;
      yystackp->yynextFree->yystate.yypred = &yystackp->yynextFree[-1].yystate;
      yystackp->yytops.yystates[0] = &yystackp->yynextFree->yystate;
      yystackp->yynextFree += 1;
      yystackp->yyspaceLeft -= 1;
    }
}

static YYRESULTTAG
yyprocessOneStack (yyGLRStack* yystackp, size_t yyk,
		   size_t yyposn, U *ctx)
{
  int yyaction;
  const short int* yyconflicts;
  yyRuleNum yyrule;

  while (yystackp->yytops.yystates[yyk] != NULL)
    {
      yyStateNum yystate = yystackp->yytops.yystates[yyk]->yylrState;
      YYDPRINTF ((stderr, "Stack %lu Entering state %d\n",
		  (unsigned long int) yyk, yystate));

      YYASSERT (yystate != YYFINAL);

      if (yyisDefaultedState (yystate))
	{
	  yyrule = yydefaultAction (yystate);
	  if (yyrule == 0)
	    {
	      YYDPRINTF ((stderr, "Stack %lu dies.\n",
			  (unsigned long int) yyk));
	      yymarkStackDeleted (yystackp, yyk);
	      return yyok;
	    }
	  YYCHK (yyglrReduce (yystackp, yyk, yyrule, yyfalse, ctx));
	}
      else
	{
	  yySymbol yytoken;
	  yystackp->yytops.yylookaheadNeeds[yyk] = yytrue;
	  if (yychar == YYEMPTY)
	    {
	      YYDPRINTF ((stderr, "Reading a token: "));
	      yychar = YYLEX;
	    }

	  if (yychar <= YYEOF)
	    {
	      yychar = yytoken = YYEOF;
	      YYDPRINTF ((stderr, "Now at end of input.\n"));
	    }
	  else
	    {
	      yytoken = YYTRANSLATE (yychar);
	      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
	    }

	  yygetLRActions (yystate, yytoken, &yyaction, &yyconflicts);

	  while (*yyconflicts != 0)
	    {
	      size_t yynewStack = yysplitStack (yystackp, yyk);
	      YYDPRINTF ((stderr, "Splitting off stack %lu from %lu.\n",
			  (unsigned long int) yynewStack,
			  (unsigned long int) yyk));
	      YYCHK (yyglrReduce (yystackp, yynewStack,
				  *yyconflicts, yyfalse, ctx));
	      YYCHK (yyprocessOneStack (yystackp, yynewStack,
					yyposn, ctx));
	      yyconflicts += 1;
	    }

	  if (yyisShiftAction (yyaction))
	    break;
	  else if (yyisErrorAction (yyaction))
	    {
	      YYDPRINTF ((stderr, "Stack %lu dies.\n",
			  (unsigned long int) yyk));
	      yymarkStackDeleted (yystackp, yyk);
	      break;
	    }
	  else
	    YYCHK (yyglrReduce (yystackp, yyk, -yyaction,
				yyfalse, ctx));
	}
    }
  return yyok;
}

/*ARGSUSED*/ static void
yyreportSyntaxError (yyGLRStack* yystackp, U *ctx)
{
  if (yystackp->yyerrState == 0)
    {
#if YYERROR_VERBOSE
      int yyn;
      yyn = yypact[yystackp->yytops.yystates[0]->yylrState];
      if (YYPACT_NINF < yyn && yyn <= YYLAST)
	{
	  yySymbol yytoken = YYTRANSLATE (yychar);
	  size_t yysize0 = yytnamerr (NULL, yytokenName (yytoken));
	  size_t yysize = yysize0;
	  size_t yysize1;
	  yybool yysize_overflow = yyfalse;
	  char* yymsg = NULL;
	  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
	  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
	  int yyx;
	  char *yyfmt;
	  char const *yyf;
	  static char const yyunexpected[] = "syntax error, unexpected %s";
	  static char const yyexpecting[] = ", expecting %s";
	  static char const yyor[] = " or %s";
	  char yyformat[sizeof yyunexpected
			+ sizeof yyexpecting - 1
			+ ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
			   * (sizeof yyor - 1))];
	  char const *yyprefix = yyexpecting;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn + 1;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 1;

	  yyarg[0] = yytokenName (yytoken);
	  yyfmt = yystpcpy (yyformat, yyunexpected);

	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
		  {
		    yycount = 1;
		    yysize = yysize0;
		    yyformat[sizeof yyunexpected - 1] = '\0';
		    break;
		  }
		yyarg[yycount++] = yytokenName (yyx);
		yysize1 = yysize + yytnamerr (NULL, yytokenName (yyx));
		yysize_overflow |= yysize1 < yysize;
		yysize = yysize1;
		yyfmt = yystpcpy (yyfmt, yyprefix);
		yyprefix = yyor;
	      }

	  yyf = YY_(yyformat);
	  yysize1 = yysize + strlen (yyf);
	  yysize_overflow |= yysize1 < yysize;
	  yysize = yysize1;

	  if (!yysize_overflow)
	    yymsg = (char *) YYMALLOC (yysize);

	  if (yymsg)
	    {
	      char *yyp = yymsg;
	      int yyi = 0;
	      while ((*yyp = *yyf))
		{
		  if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		    {
		      yyp += yytnamerr (yyp, yyarg[yyi++]);
		      yyf += 2;
		    }
		  else
		    {
		      yyp++;
		      yyf++;
		    }
		}
	      yyerror (ctx, yymsg);
	      YYFREE (yymsg);
	    }
	  else
	    {
	      yyerror (ctx, YY_("syntax error"));
	      yyMemoryExhausted (yystackp);
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (ctx, YY_("syntax error"));
      yynerrs += 1;
    }
}

/* Recover from a syntax error on *YYSTACKP, assuming that *YYSTACKP->YYTOKENP,
   yylval, and yylloc are the syntactic category, semantic value, and location
   of the lookahead.  */
/*ARGSUSED*/ static void
yyrecoverSyntaxError (yyGLRStack* yystackp, U *ctx)
{
  size_t yyk;
  int yyj;

  if (yystackp->yyerrState == 3)
    /* We just shifted the error token and (perhaps) took some
       reductions.  Skip tokens until we can proceed.  */
    while (YYID (yytrue))
      {
	yySymbol yytoken;
	if (yychar == YYEOF)
	  yyFail (yystackp, ctx, NULL);
	if (yychar != YYEMPTY)
	  {
	    yytoken = YYTRANSLATE (yychar);
	    yydestruct ("Error: discarding",
			yytoken, &yylval, ctx);
	  }
	YYDPRINTF ((stderr, "Reading a token: "));
	yychar = YYLEX;
	if (yychar <= YYEOF)
	  {
	    yychar = yytoken = YYEOF;
	    YYDPRINTF ((stderr, "Now at end of input.\n"));
	  }
	else
	  {
	    yytoken = YYTRANSLATE (yychar);
	    YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
	  }
	yyj = yypact[yystackp->yytops.yystates[0]->yylrState];
	if (yyis_pact_ninf (yyj))
	  return;
	yyj += yytoken;
	if (yyj < 0 || YYLAST < yyj || yycheck[yyj] != yytoken)
	  {
	    if (yydefact[yystackp->yytops.yystates[0]->yylrState] != 0)
	      return;
	  }
	else if (yytable[yyj] != 0 && ! yyis_table_ninf (yytable[yyj]))
	  return;
      }

  /* Reduce to one stack.  */
  for (yyk = 0; yyk < yystackp->yytops.yysize; yyk += 1)
    if (yystackp->yytops.yystates[yyk] != NULL)
      break;
  if (yyk >= yystackp->yytops.yysize)
    yyFail (yystackp, ctx, NULL);
  for (yyk += 1; yyk < yystackp->yytops.yysize; yyk += 1)
    yymarkStackDeleted (yystackp, yyk);
  yyremoveDeletes (yystackp);
  yycompressStack (yystackp);

  /* Now pop stack until we find a state that shifts the error token.  */
  yystackp->yyerrState = 3;
  while (yystackp->yytops.yystates[0] != NULL)
    {
      yyGLRState *yys = yystackp->yytops.yystates[0];
      yyj = yypact[yys->yylrState];
      if (! yyis_pact_ninf (yyj))
	{
	  yyj += YYTERROR;
	  if (0 <= yyj && yyj <= YYLAST && yycheck[yyj] == YYTERROR
	      && yyisShiftAction (yytable[yyj]))
	    {
	      /* Shift the error token having adjusted its location.  */
	      YYLTYPE yyerrloc;
	      YY_SYMBOL_PRINT ("Shifting", yystos[yytable[yyj]],
			       &yylval, &yyerrloc);
	      yyglrShift (yystackp, 0, yytable[yyj],
			  yys->yyposn, &yylval, &yyerrloc);
	      yys = yystackp->yytops.yystates[0];
	      break;
	    }
	}

      if (yys->yypred != NULL)
	yydestroyGLRState ("Error: popping", yys, ctx);
      yystackp->yytops.yystates[0] = yys->yypred;
      yystackp->yynextFree -= 1;
      yystackp->yyspaceLeft += 1;
    }
  if (yystackp->yytops.yystates[0] == NULL)
    yyFail (yystackp, ctx, NULL);
}

#define YYCHK1(YYE)							     \
  do {									     \
    switch (YYE) {							     \
    case yyok:								     \
      break;								     \
    case yyabort:							     \
      goto yyabortlab;							     \
    case yyaccept:							     \
      goto yyacceptlab;							     \
    case yyerr:								     \
      goto yyuser_error;						     \
    default:								     \
      goto yybuglab;							     \
    }									     \
  } while (YYID (0))


/*----------.
| yyparse.  |
`----------*/

int
yyparse (U *ctx)
{
  int yyresult;
  yyGLRStack yystack;
  yyGLRStack* const yystackp = &yystack;
  size_t yyposn;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY;
  yylval = yyval_default;


  if (! yyinitGLRStack (yystackp, YYINITDEPTH))
    goto yyexhaustedlab;
  switch (YYSETJMP (yystack.yyexception_buffer))
    {
    case 0: break;
    case 1: goto yyabortlab;
    case 2: goto yyexhaustedlab;
    default: goto yybuglab;
    }
  yyglrShift (&yystack, 0, 0, 0, &yylval, &yylloc);
  yyposn = 0;

  while (YYID (yytrue))
    {
      /* For efficiency, we have two loops, the first of which is
	 specialized to deterministic operation (single stack, no
	 potential ambiguity).  */
      /* Standard mode */
      while (YYID (yytrue))
	{
	  yyRuleNum yyrule;
	  int yyaction;
	  const short int* yyconflicts;

	  yyStateNum yystate = yystack.yytops.yystates[0]->yylrState;
	  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
	  if (yystate == YYFINAL)
	    goto yyacceptlab;
	  if (yyisDefaultedState (yystate))
	    {
	      yyrule = yydefaultAction (yystate);
	      if (yyrule == 0)
		{

		  yyreportSyntaxError (&yystack, ctx);
		  goto yyuser_error;
		}
	      YYCHK1 (yyglrReduce (&yystack, 0, yyrule, yytrue, ctx));
	    }
	  else
	    {
	      yySymbol yytoken;
	      if (yychar == YYEMPTY)
		{
		  YYDPRINTF ((stderr, "Reading a token: "));
		  yychar = YYLEX;
		}

	      if (yychar <= YYEOF)
		{
		  yychar = yytoken = YYEOF;
		  YYDPRINTF ((stderr, "Now at end of input.\n"));
		}
	      else
		{
		  yytoken = YYTRANSLATE (yychar);
		  YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
		}

	      yygetLRActions (yystate, yytoken, &yyaction, &yyconflicts);
	      if (*yyconflicts != 0)
		break;
	      if (yyisShiftAction (yyaction))
		{
		  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
		  yychar = YYEMPTY;
		  yyposn += 1;
		  yyglrShift (&yystack, 0, yyaction, yyposn, &yylval, &yylloc);
		  if (0 < yystack.yyerrState)
		    yystack.yyerrState -= 1;
		}
	      else if (yyisErrorAction (yyaction))
		{

		  yyreportSyntaxError (&yystack, ctx);
		  goto yyuser_error;
		}
	      else
		YYCHK1 (yyglrReduce (&yystack, 0, -yyaction, yytrue, ctx));
	    }
	}

      while (YYID (yytrue))
	{
	  yySymbol yytoken_to_shift;
	  size_t yys;

	  for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
	    yystackp->yytops.yylookaheadNeeds[yys] = yychar != YYEMPTY;

	  /* yyprocessOneStack returns one of three things:

	      - An error flag.  If the caller is yyprocessOneStack, it
		immediately returns as well.  When the caller is finally
		yyparse, it jumps to an error label via YYCHK1.

	      - yyok, but yyprocessOneStack has invoked yymarkStackDeleted
		(&yystack, yys), which sets the top state of yys to NULL.  Thus,
		yyparse's following invocation of yyremoveDeletes will remove
		the stack.

	      - yyok, when ready to shift a token.

	     Except in the first case, yyparse will invoke yyremoveDeletes and
	     then shift the next token onto all remaining stacks.  This
	     synchronization of the shift (that is, after all preceding
	     reductions on all stacks) helps prevent double destructor calls
	     on yylval in the event of memory exhaustion.  */

	  for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
	    YYCHK1 (yyprocessOneStack (&yystack, yys, yyposn, ctx));
	  yyremoveDeletes (&yystack);
	  if (yystack.yytops.yysize == 0)
	    {
	      yyundeleteLastStack (&yystack);
	      if (yystack.yytops.yysize == 0)
		yyFail (&yystack, ctx, YY_("syntax error"));
	      YYCHK1 (yyresolveStack (&yystack, ctx));
	      YYDPRINTF ((stderr, "Returning to deterministic operation.\n"));

	      yyreportSyntaxError (&yystack, ctx);
	      goto yyuser_error;
	    }

	  /* If any yyglrShift call fails, it will fail after shifting.  Thus,
	     a copy of yylval will already be on stack 0 in the event of a
	     failure in the following loop.  Thus, yychar is set to YYEMPTY
	     before the loop to make sure the user destructor for yylval isn't
	     called twice.  */
	  yytoken_to_shift = YYTRANSLATE (yychar);
	  yychar = YYEMPTY;
	  yyposn += 1;
	  for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
	    {
	      int yyaction;
	      const short int* yyconflicts;
	      yyStateNum yystate = yystack.yytops.yystates[yys]->yylrState;
	      yygetLRActions (yystate, yytoken_to_shift, &yyaction,
			      &yyconflicts);
	      /* Note that yyconflicts were handled by yyprocessOneStack.  */
	      YYDPRINTF ((stderr, "On stack %lu, ", (unsigned long int) yys));
	      YY_SYMBOL_PRINT ("shifting", yytoken_to_shift, &yylval, &yylloc);
	      yyglrShift (&yystack, yys, yyaction, yyposn,
			  &yylval, &yylloc);
	      YYDPRINTF ((stderr, "Stack %lu now in state #%d\n",
			  (unsigned long int) yys,
			  yystack.yytops.yystates[yys]->yylrState));
	    }

	  if (yystack.yytops.yysize == 1)
	    {
	      YYCHK1 (yyresolveStack (&yystack, ctx));
	      YYDPRINTF ((stderr, "Returning to deterministic operation.\n"));
	      yycompressStack (&yystack);
	      break;
	    }
	}
      continue;
    yyuser_error:
      yyrecoverSyntaxError (&yystack, ctx);
      yyposn = yystack.yytops.yystates[0]->yyposn;
    }

 yyacceptlab:
  yyresult = 0;
  goto yyreturn;

 yybuglab:
  YYASSERT (yyfalse);
  goto yyabortlab;

 yyabortlab:
  yyresult = 1;
  goto yyreturn;

 yyexhaustedlab:
  yyerror (ctx, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;

 yyreturn:
  if (yychar != YYEMPTY)
    yydestruct ("Cleanup: discarding lookahead",
		YYTRANSLATE (yychar),
		&yylval, ctx);

  /* If the stack is well-formed, pop the stack until it is empty,
     destroying its entries as we go.  But free the stack regardless
     of whether it is well-formed.  */
  if (yystack.yyitems)
    {
      yyGLRState** yystates = yystack.yytops.yystates;
      if (yystates)
	{
	  size_t yysize = yystack.yytops.yysize;
	  size_t yyk;
	  for (yyk = 0; yyk < yysize; yyk += 1)
	    if (yystates[yyk])
	      {
		while (yystates[yyk])
		  {
		    yyGLRState *yys = yystates[yyk];
		    if (yys->yypred != NULL)
		      yydestroyGLRState ("Cleanup: popping", yys, ctx);
		    yystates[yyk] = yys->yypred;
		    yystack.yynextFree -= 1;
		    yystack.yyspaceLeft += 1;
		  }
		break;
	      }
	}
      yyfreeGLRStack (&yystack);
    }

  /* Make sure YYID is used.  */
  return YYID (yyresult);
}

/* DEBUGGING ONLY */
#if YYDEBUG
static void yypstack (yyGLRStack* yystackp, size_t yyk)
  __attribute__ ((__unused__));
static void yypdumpstack (yyGLRStack* yystackp) __attribute__ ((__unused__));

static void
yy_yypstack (yyGLRState* yys)
{
  if (yys->yypred)
    {
      yy_yypstack (yys->yypred);
      YYFPRINTF (stderr, " -> ");
    }
  YYFPRINTF (stderr, "%d@%lu", yys->yylrState,
             (unsigned long int) yys->yyposn);
}

static void
yypstates (yyGLRState* yyst)
{
  if (yyst == NULL)
    YYFPRINTF (stderr, "<null>");
  else
    yy_yypstack (yyst);
  YYFPRINTF (stderr, "\n");
}

static void
yypstack (yyGLRStack* yystackp, size_t yyk)
{
  yypstates (yystackp->yytops.yystates[yyk]);
}

#define YYINDEX(YYX)							     \
    ((YYX) == NULL ? -1 : (yyGLRStackItem*) (YYX) - yystackp->yyitems)


static void
yypdumpstack (yyGLRStack* yystackp)
{
  yyGLRStackItem* yyp;
  size_t yyi;
  for (yyp = yystackp->yyitems; yyp < yystackp->yynextFree; yyp += 1)
    {
      YYFPRINTF (stderr, "%3lu. ",
                 (unsigned long int) (yyp - yystackp->yyitems));
      if (*(yybool *) yyp)
	{
	  YYFPRINTF (stderr, "Res: %d, LR State: %d, posn: %lu, pred: %ld",
		     yyp->yystate.yyresolved, yyp->yystate.yylrState,
		     (unsigned long int) yyp->yystate.yyposn,
		     (long int) YYINDEX (yyp->yystate.yypred));
	  if (! yyp->yystate.yyresolved)
	    YYFPRINTF (stderr, ", firstVal: %ld",
		       (long int) YYINDEX (yyp->yystate
                                             .yysemantics.yyfirstVal));
	}
      else
	{
	  YYFPRINTF (stderr, "Option. rule: %d, state: %ld, next: %ld",
		     yyp->yyoption.yyrule - 1,
		     (long int) YYINDEX (yyp->yyoption.yystate),
		     (long int) YYINDEX (yyp->yyoption.yynext));
	}
      YYFPRINTF (stderr, "\n");
    }
  YYFPRINTF (stderr, "Tops:");
  for (yyi = 0; yyi < yystackp->yytops.yysize; yyi += 1)
    YYFPRINTF (stderr, "%lu: %ld; ", (unsigned long int) yyi,
	       (long int) YYINDEX (yystackp->yytops.yystates[yyi]));
  YYFPRINTF (stderr, "\n");
}
#endif



/* Line 2634 of glr.c  */
#line 1035 "c.y"

static void
yyerror(U *ctx, const char *s)
{
	parseerror(ctx, (char*)s);
}

/* expression trees for ambiguous forms share identifier nodes;
   separate them so that we can handle them independently. */
static void
duptickid(Expr *e)
{
	if(e == 0)
		return;
	if(e->kind == Eticke || e->kind == Etickt){
		e->e1 = copyexpr(e->e1);
		e->e2 = copyexpr(e->e2);
		return;
	}
	duptickid(e->e1);
	duptickid(e->e2);
	duptickid(e->e3);
	duptickid(e->e4);
}

static Expr*
castmerge(YYSTYPE ye1, YYSTYPE ye2)
{
	Expr *cast, *other;

	// (T)(<expr>)  cast expression or function call

	if(ye1.expr->kind == Ecast){
		cast = ye1.expr;
		other = ye2.expr;
	}else if(ye2.expr->kind == Ecast){
		cast = ye2.expr;
		other = ye1.expr;
	}else
		yyerror(0, "unresolved ambiguity");

	/* what else could it be? */
	if(other->kind != Ecall)
		yyerror(0, "unresolved ambiguity");
	if(other->e1->kind != Eticke && other->e1->kind != Etickt)
		yyerror(0, "unresolved ambiguity");

	/* it's not possible to call through a domain reference,
	   so call it a cast. */
	duptickid(other->e1);
	freeexpr(other);
	return cast;
}

static Expr*
andmerge(YYSTYPE ye1, YYSTYPE ye2)
{
	Expr *cast, *other;

	// (dom`T)&p  cast expression or and

	if(ye1.expr->kind == Ecast){
		cast = ye1.expr;
		other = ye2.expr;
	}else if(ye2.expr->kind == Ecast){
		cast = ye2.expr;
		other = ye1.expr;
	}else
		yyerror(0, "unresolved ambiguity");

	duptickid(other->e1);

	return newexprsrc(&cast->src, Eambig, cast, other, NULL, NULL);
}

static Expr*
mulmerge(YYSTYPE ye1, YYSTYPE ye2)
{
	Expr *cast, *other;

	// (dom`T)*p  cast expression or multiply

	if(ye1.expr->kind == Ecast){
		cast = ye1.expr;
		other = ye2.expr;
	}else if(ye2.expr->kind == Ecast){
		cast = ye2.expr;
		other = ye1.expr;
	}else
		yyerror(0, "unresolved ambiguity");

	duptickid(other->e1);

	return newexprsrc(&cast->src, Eambig, cast, other, NULL, NULL);
}

static int
ofkind(int kind)
{
	return (kind == Esizeoft) || (kind == Esizeofe)
		|| (kind == Etypeoft) || (kind == Etypeofe);
}

static Expr*
ofmerge(YYSTYPE ye1, YYSTYPE ye2)
{
	Expr *e1, *e2;

	// Xof(dom`T)   X is "size" or "type"

	e1 = ye1.expr;
	e2 = ye2.expr;
	if(!ofkind(e1->kind) || !ofkind(e2->kind))
		yyerror(0, "unresolved ambiguity");

	if(e1->kind == Esizeofe || e1->kind == Etypeofe)
		duptickid(e1->e1);
	else
		duptickid(e2->e1);
	return newexprsrc(&e1->src, Eambig, e1, e2, NULL, NULL);
}

