
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
#line 85 "c.y"

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
#define YYFINAL  225
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3039

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  108
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  109
/* YYNRULES -- Number of rules.  */
#define YYNRULES  345
/* YYNRULES -- Number of states.  */
#define YYNSTATES  633
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule.  */
#define YYMAXRHS 9
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule.  */
#define YYMAXLEFT 0

/* YYTRANSLATE(X) -- Bison symbol number corresponding to X.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   336

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    98,     2,     2,     2,   100,    93,     2,
      82,    83,    94,    95,    87,    96,    92,    99,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    88,   106,
     101,    91,   102,   105,   107,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    89,     2,    90,   103,     2,    84,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    85,   104,    86,    97,     2,     2,     2,
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
      75,    76,    77,    78,    79,    80,    81
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     5,     7,    10,    15,    17,    18,    22,
      24,    30,    35,    41,    48,    53,    56,    60,    64,    68,
      72,    77,    83,    87,    89,    93,    95,    97,    99,   101,
     103,   107,   113,   116,   120,   125,   130,   134,   138,   143,
     145,   147,   149,   151,   153,   155,   157,   159,   163,   169,
     172,   176,   181,   186,   190,   194,   199,   204,   208,   210,
     212,   214,   218,   222,   228,   230,   234,   238,   240,   245,
     249,   254,   258,   262,   265,   268,   277,   279,   283,   285,
     288,   291,   294,   297,   302,   305,   310,   315,   317,   319,
     321,   323,   325,   327,   329,   334,   339,   341,   345,   349,
     353,   355,   359,   363,   365,   369,   373,   375,   379,   383,
     387,   391,   393,   397,   401,   403,   407,   409,   413,   415,
     419,   421,   425,   427,   431,   433,   439,   441,   445,   447,
     449,   451,   453,   455,   457,   459,   461,   463,   465,   467,
     469,   473,   475,   478,   480,   483,   485,   487,   490,   492,
     498,   503,   510,   515,   517,   519,   521,   525,   527,   532,
     535,   536,   539,   545,   549,   551,   555,   557,   559,   561,
     563,   565,   567,   569,   571,   573,   575,   577,   580,   582,
     584,   586,   588,   595,   601,   606,   609,   611,   613,   615,
     617,   619,   621,   624,   630,   635,   639,   648,   651,   654,
     658,   660,   662,   664,   667,   669,   670,   672,   675,   678,
     680,   684,   686,   692,   699,   704,   710,   713,   715,   719,
     721,   725,   728,   730,   732,   736,   741,   745,   751,   756,
     758,   760,   763,   767,   770,   774,   778,   783,   787,   792,
     798,   800,   803,   806,   810,   812,   816,   818,   822,   826,
     830,   833,   835,   838,   840,   843,   846,   848,   852,   856,
     859,   862,   867,   869,   873,   875,   879,   882,   885,   887,
     889,   892,   895,   897,   899,   901,   903,   906,   909,   911,
     913,   916,   920,   923,   927,   931,   936,   940,   945,   951,
     954,   956,   958,   962,   967,   971,   976,   980,   982,   984,
     986,   988,   990,   992,   994,   996,   998,  1003,  1007,  1011,
    1013,  1016,  1019,  1023,  1027,  1032,  1034,  1037,  1039,  1042,
    1045,  1051,  1059,  1065,  1069,  1074,  1079,  1083,  1085,  1086,
    1092,  1100,  1110,  1114,  1117,  1120,  1123,  1127,  1129,  1131,
    1138,  1144,  1146,  1147,  1149,  1152
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const short int yyrhs[] =
{
     214,     0,    -1,     3,    -1,   109,    -1,    76,   109,    -1,
      76,    82,   153,    83,    -1,   110,    -1,    -1,   110,    84,
     110,    -1,    43,    -1,   113,    82,   147,    83,   204,    -1,
     113,    82,    83,   204,    -1,    15,   110,    85,   146,    86,
      -1,    15,   110,    85,   146,    87,    86,    -1,    15,   110,
      85,    86,    -1,    45,   204,    -1,    74,   199,    86,    -1,
      74,   153,    86,    -1,    75,   199,    86,    -1,    75,   153,
      86,    -1,    73,    82,   110,    83,    -1,    72,   110,    82,
     129,    83,    -1,   152,    88,   152,    -1,   119,    -1,   120,
      87,   119,    -1,   110,    -1,   112,    -1,     4,    -1,     5,
      -1,     6,    -1,    82,   153,    83,    -1,    78,   152,    87,
     152,    79,    -1,    89,    90,    -1,    89,   129,    90,    -1,
      89,   129,    87,    90,    -1,    89,   129,    60,    90,    -1,
      89,    88,    90,    -1,    89,   120,    90,    -1,    89,   120,
      87,    90,    -1,   114,    -1,   115,    -1,   116,    -1,   117,
      -1,   118,    -1,   110,    -1,     5,    -1,     6,    -1,    82,
     122,    83,    -1,    78,   122,    87,   122,    79,    -1,    89,
      90,    -1,    89,   124,    90,    -1,    89,   124,    87,    90,
      -1,    89,   124,    60,    90,    -1,    89,    88,    90,    -1,
      89,   126,    90,    -1,    89,   126,    87,    90,    -1,   110,
      82,   123,    83,    -1,   110,    82,    83,    -1,   124,    -1,
     125,    -1,   122,    -1,   124,    87,   122,    -1,   110,    91,
     122,    -1,   125,    87,   110,    91,   122,    -1,   127,    -1,
     126,    87,   127,    -1,   122,    88,   122,    -1,   121,    -1,
     128,    89,   153,    90,    -1,   128,    82,    83,    -1,   128,
      82,   129,    83,    -1,   128,    92,   110,    -1,   128,    17,
     110,    -1,   128,    18,    -1,   128,    19,    -1,    16,    82,
     153,    87,   185,    87,   110,    83,    -1,   152,    -1,   129,
      87,   152,    -1,   128,    -1,    18,   130,    -1,    19,   130,
      -1,   131,   132,    -1,     9,   130,    -1,     9,    82,   185,
      83,    -1,    11,   130,    -1,    11,    82,   185,    83,    -1,
      10,    82,   185,    83,    -1,    93,    -1,    94,    -1,    95,
      -1,    96,    -1,    97,    -1,    98,    -1,   130,    -1,    82,
     185,    83,   132,    -1,    85,   153,    86,   132,    -1,   132,
      -1,   133,    94,   132,    -1,   133,    99,   132,    -1,   133,
     100,   132,    -1,   133,    -1,   134,    95,   133,    -1,   134,
      96,   133,    -1,   134,    -1,   135,    20,   134,    -1,   135,
      21,   134,    -1,   135,    -1,   136,   101,   135,    -1,   136,
     102,   135,    -1,   136,    22,   135,    -1,   136,    23,   135,
      -1,   136,    -1,   137,    24,   136,    -1,   137,    25,   136,
      -1,   137,    -1,   138,    93,   137,    -1,   138,    -1,   139,
     103,   138,    -1,   139,    -1,   140,   104,   139,    -1,   140,
      -1,   141,    26,   140,    -1,   141,    -1,   142,    27,   141,
      -1,   142,    -1,   142,   105,   153,    88,   143,    -1,   143,
      -1,   130,   145,   152,    -1,    91,    -1,    31,    -1,    35,
      -1,    37,    -1,    36,    -1,    29,    -1,    30,    -1,    28,
      -1,    33,    -1,    34,    -1,    32,    -1,   110,    -1,   146,
      87,   110,    -1,   146,    -1,   146,    60,    -1,   149,    -1,
     148,   149,    -1,   157,    -1,   155,    -1,     1,   106,    -1,
     144,    -1,    44,   153,    85,   148,    86,    -1,    44,   153,
      85,    86,    -1,    46,    82,   152,    87,   129,    83,    -1,
      46,    82,   152,    83,    -1,   150,    -1,   151,    -1,   152,
      -1,   153,    87,   152,    -1,   143,    -1,    12,   171,   158,
     106,    -1,   107,   154,    -1,    -1,   171,   106,    -1,   107,
     154,   171,   158,   106,    -1,   171,   158,   106,    -1,   177,
      -1,   158,    87,   177,    -1,    56,    -1,    47,    -1,    48,
      -1,    49,    -1,    50,    -1,    51,    -1,    54,    -1,    55,
      -1,    52,    -1,    53,    -1,   159,    -1,   160,   159,    -1,
     160,    -1,   110,    -1,   162,    -1,   174,    -1,   163,   111,
      85,   165,   167,    86,    -1,   163,   111,    85,   167,    86,
      -1,   163,   111,    85,    86,    -1,   163,   110,    -1,    57,
      -1,    58,    -1,    57,    -1,    58,    -1,    59,    -1,   166,
      -1,   165,   166,    -1,   107,   154,   171,   172,   106,    -1,
     107,   154,   171,   106,    -1,   171,   172,   106,    -1,   107,
     107,   154,   171,   173,    88,   154,   106,    -1,   110,    88,
      -1,     1,   106,    -1,   107,   154,   106,    -1,     7,    -1,
       8,    -1,   168,    -1,   168,   169,    -1,   169,    -1,    -1,
     161,    -1,   169,   161,    -1,   161,   169,    -1,   173,    -1,
     172,    87,   173,    -1,   177,    -1,    59,   111,    85,   175,
      86,    -1,    59,   111,    85,   175,    87,    86,    -1,    59,
     111,    85,    86,    -1,    59,   111,    85,    87,    86,    -1,
      59,   110,    -1,   176,    -1,   175,    87,   176,    -1,   110,
      -1,   110,    91,   154,    -1,   181,   178,    -1,   178,    -1,
     110,    -1,    82,   177,    83,    -1,   178,    89,   154,    90,
      -1,   178,    89,    90,    -1,   178,    82,   182,    83,   170,
      -1,   178,    82,    83,   170,    -1,   181,    -1,   180,    -1,
     181,   180,    -1,    82,   179,    83,    -1,    89,    90,    -1,
      89,   154,    90,    -1,   180,    89,    90,    -1,   180,    89,
     154,    90,    -1,    82,    83,   170,    -1,   180,    82,    83,
     170,    -1,   180,    82,   182,    83,   170,    -1,    94,    -1,
      94,   169,    -1,    94,   181,    -1,    94,   169,   181,    -1,
     183,    -1,   183,    87,    60,    -1,   184,    -1,   183,    87,
     184,    -1,   156,   171,   177,    -1,   156,   171,   179,    -1,
     156,   171,    -1,   186,    -1,   186,   195,    -1,   187,    -1,
     169,   187,    -1,   187,   169,    -1,   160,    -1,   110,    84,
     160,    -1,   110,    84,   110,    -1,    84,   110,    -1,   164,
     110,    -1,   164,   110,    84,   110,    -1,   189,    -1,   189,
      87,    60,    -1,   190,    -1,   189,    87,   190,    -1,   191,
     197,    -1,   191,   195,    -1,   191,    -1,   192,    -1,   192,
     169,    -1,   169,   192,    -1,   160,    -1,   110,    -1,   193,
      -1,   194,    -1,   163,   110,    -1,    59,   110,    -1,   181,
      -1,   196,    -1,   181,   196,    -1,    82,   195,    83,    -1,
      89,    90,    -1,    89,   154,    90,    -1,   196,    89,    90,
      -1,   196,    89,   154,    90,    -1,    82,    83,   170,    -1,
     196,    82,    83,   170,    -1,   196,    82,   188,    83,   170,
      -1,   181,   198,    -1,   198,    -1,   110,    -1,    82,   197,
      83,    -1,   198,    89,   154,    90,    -1,   198,    89,    90,
      -1,   198,    82,   188,    83,    -1,   198,    82,    83,    -1,
     204,    -1,   206,    -1,   207,    -1,   210,    -1,   211,    -1,
     213,    -1,   208,    -1,   201,    -1,   200,    -1,    76,    85,
     152,    86,    -1,    41,   146,   106,    -1,    42,   146,   106,
      -1,   202,    -1,   203,   202,    -1,    85,    86,    -1,    85,
     203,    86,    -1,    85,   205,    86,    -1,    85,   203,   205,
      86,    -1,   199,    -1,   205,   199,    -1,   106,    -1,   153,
     106,    -1,     1,   106,    -1,    61,    82,   153,    83,   199,
      -1,    61,    82,   153,    83,   199,    62,   199,    -1,    63,
      82,   153,    83,   204,    -1,   110,    88,   199,    -1,    70,
     153,    88,   199,    -1,    81,   122,    88,   199,    -1,    71,
      88,   199,    -1,   153,    -1,    -1,    64,    82,   153,    83,
     199,    -1,    65,   199,    64,    82,   153,    83,   106,    -1,
      66,    82,   209,   106,   209,   106,   209,    83,   199,    -1,
      40,   110,   106,    -1,    67,   106,    -1,    68,   106,    -1,
      69,   106,    -1,    69,   153,   106,    -1,    13,    -1,    14,
      -1,   212,   110,    82,   147,    83,   204,    -1,   212,   110,
      82,    83,   204,    -1,   215,    -1,    -1,   216,    -1,   215,
     216,    -1,   199,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,    95,    95,   100,   101,   103,   108,   110,   114,   119,
     123,   125,   130,   132,   134,   139,   144,   146,   148,   150,
     155,   157,   162,   172,   174,   179,   180,   181,   183,   185,
     187,   189,   191,   193,   195,   197,   202,   204,   206,   208,
     209,   210,   211,   212,   216,   217,   219,   221,   223,   225,
     227,   229,   231,   236,   238,   240,   242,   244,   249,   250,
     254,   256,   261,   266,   274,   276,   281,   293,   294,   296,
     298,   300,   302,   304,   306,   308,   313,   315,   320,   321,
     323,   325,   327,   329,   331,   333,   335,   340,   342,   344,
     346,   348,   350,   355,   356,   358,   363,   364,   366,   368,
     373,   374,   376,   381,   382,   384,   389,   390,   392,   394,
     396,   401,   402,   404,   409,   410,   415,   416,   421,   422,
     427,   428,   433,   434,   439,   440,   445,   446,   455,   457,
     459,   461,   463,   465,   467,   469,   471,   473,   475,   480,
     482,   487,   489,   497,   499,   504,   505,   506,   511,   512,
     514,   519,   521,   526,   527,   531,   532,   537,   541,   546,
     549,   553,   555,   557,   562,   564,   569,   571,   573,   575,
     577,   579,   581,   583,   585,   587,   591,   593,   598,   600,
     602,   603,   607,   609,   611,   613,   618,   620,   625,   627,
     629,   634,   643,   654,   656,   658,   660,   663,   665,   670,
     676,   677,   681,   682,   686,   687,   691,   692,   694,   699,
     701,   706,   710,   712,   714,   716,   718,   723,   725,   732,
     734,   739,   741,   745,   747,   749,   751,   753,   755,   760,
     761,   762,   767,   769,   771,   773,   775,   777,   779,   781,
     786,   788,   790,   792,   797,   799,   804,   806,   811,   813,
     815,   820,   822,   827,   828,   830,   835,   837,   840,   843,
     846,   848,   853,   855,   860,   862,   867,   869,   871,   876,
     877,   879,   884,   886,   888,   889,   893,   898,   903,   904,
     905,   910,   912,   914,   916,   918,   920,   922,   924,   929,
     931,   935,   937,   939,   941,   943,   945,   950,   951,   952,
     953,   954,   955,   956,   957,   958,   962,   967,   972,   977,
     979,   984,   994,  1004,  1017,  1033,  1035,  1040,  1042,  1044,
    1049,  1051,  1053,  1059,  1061,  1063,  1065,  1070,  1072,  1076,
    1078,  1080,  1085,  1087,  1089,  1091,  1093,  1098,  1100,  1105,
    1107,  1112,  1113,  1117,  1118,  1122
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
  "DO", "FOR", "CONTINUE", "BREAK", "RETURN", "CASE", "DEFAULT", "SYNTAX",
  "SYNTAXID", "SYNTAXQUOTE", "SYNTAXQUASI", "SYNTAXUNQUOTE",
  "SYNTAXSPLICE", "LPAIR", "RPAIR", "NOBIND_PRE", "MATCH", "'('", "')'",
  "'`'", "'{'", "'}'", "','", "':'", "'['", "']'", "'='", "'.'", "'&'",
  "'*'", "'+'", "'-'", "'~'", "'!'", "'/'", "'%'", "'<'", "'>'", "'^'",
  "'|'", "'?'", "';'", "'@'", "$accept", "_id", "id", "maybeid", "tickid",
  "lambda", "lambda_expression", "defrec_expression", "let_expression",
  "quote_expression", "syntax_expression", "table_init", "table_init_list",
  "primary_expression", "pattern", "rec_pat_list", "pattern_list",
  "var_pat_list", "table_init_pattern_list", "table_init_pattern",
  "postfix_expression", "argument_expression_list", "unary_expression",
  "unary_operator", "cast_expression", "multiplicative_expression",
  "additive_expression", "shift_expression", "relational_expression",
  "equality_expression", "and_expression", "exclusive_or_expression",
  "inclusive_or_expression", "logical_and_expression",
  "logical_or_expression", "conditional_expression",
  "assignment_expression", "assignment_operator", "identifier_list",
  "arg_id_list", "names_declaration_list", "names_declaration",
  "names_expression", "lapply_expression", "root_expression", "expression",
  "constant_expression", "typedef", "maybe_attr", "declaration",
  "declarator_list", "base", "base_list", "type_specifier",
  "struct_or_union_specifier", "struct_or_union",
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
  "statement", "unquote_statement", "global_statement", "local",
  "local_list", "compound_statement", "statement_list",
  "expression_statement", "selection_statement", "labeled_statement",
  "maybe_expression", "iteration_statement", "jump_statement", "define",
  "define_statement", "translation_unit_seq", "translation_unit",
  "external_declaration", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   108,   109,   110,   110,   110,   111,   111,   112,   113,
     114,   114,   115,   115,   115,   116,   117,   117,   117,   117,
     118,   118,   119,   120,   120,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   122,   122,   122,   122,   122,   122,
     122,   122,   122,   122,   122,   122,   122,   122,   123,   123,
     124,   124,   125,   125,   126,   126,   127,   128,   128,   128,
     128,   128,   128,   128,   128,   128,   129,   129,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   131,   131,   131,
     131,   131,   131,   132,   132,   132,   133,   133,   133,   133,
     134,   134,   134,   135,   135,   135,   136,   136,   136,   136,
     136,   137,   137,   137,   138,   138,   139,   139,   140,   140,
     141,   141,   142,   142,   143,   143,   144,   144,   145,   145,
     145,   145,   145,   145,   145,   145,   145,   145,   145,   146,
     146,   147,   147,   148,   148,   149,   149,   149,   150,   150,
     150,   151,   151,   152,   152,   153,   153,   154,   155,   156,
     156,   157,   157,   157,   158,   158,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   160,   160,   161,   161,
     161,   161,   162,   162,   162,   162,   163,   163,   164,   164,
     164,   165,   165,   166,   166,   166,   166,   166,   166,   167,
     168,   168,   169,   169,   170,   170,   171,   171,   171,   172,
     172,   173,   174,   174,   174,   174,   174,   175,   175,   176,
     176,   177,   177,   178,   178,   178,   178,   178,   178,   179,
     179,   179,   180,   180,   180,   180,   180,   180,   180,   180,
     181,   181,   181,   181,   182,   182,   183,   183,   184,   184,
     184,   185,   185,   186,   186,   186,   187,   187,   187,   187,
     187,   187,   188,   188,   189,   189,   190,   190,   190,   191,
     191,   191,   192,   192,   192,   192,   193,   194,   195,   195,
     195,   196,   196,   196,   196,   196,   196,   196,   196,   197,
     197,   198,   198,   198,   198,   198,   198,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   200,   201,   202,   203,
     203,   204,   204,   204,   204,   205,   205,   206,   206,   206,
     207,   207,   207,   208,   208,   208,   208,   209,   209,   210,
     210,   210,   211,   211,   211,   211,   211,   212,   212,   213,
     213,   214,   214,   215,   215,   216
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     1,     2,     4,     1,     0,     3,     1,
       5,     4,     5,     6,     4,     2,     3,     3,     3,     3,
       4,     5,     3,     1,     3,     1,     1,     1,     1,     1,
       3,     5,     2,     3,     4,     4,     3,     3,     4,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     5,     2,
       3,     4,     4,     3,     3,     4,     4,     3,     1,     1,
       1,     3,     3,     5,     1,     3,     3,     1,     4,     3,
       4,     3,     3,     2,     2,     8,     1,     3,     1,     2,
       2,     2,     2,     4,     2,     4,     4,     1,     1,     1,
       1,     1,     1,     1,     4,     4,     1,     3,     3,     3,
       1,     3,     3,     1,     3,     3,     1,     3,     3,     3,
       3,     1,     3,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     3,     1,     5,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     1,     2,     1,     2,     1,     1,     2,     1,     5,
       4,     6,     4,     1,     1,     1,     3,     1,     4,     2,
       0,     2,     5,     3,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     1,
       1,     1,     6,     5,     4,     2,     1,     1,     1,     1,
       1,     1,     2,     5,     4,     3,     8,     2,     2,     3,
       1,     1,     1,     2,     1,     0,     1,     2,     2,     1,
       3,     1,     5,     6,     4,     5,     2,     1,     3,     1,
       3,     2,     1,     1,     3,     4,     3,     5,     4,     1,
       1,     2,     3,     2,     3,     3,     4,     3,     4,     5,
       1,     2,     2,     3,     1,     3,     1,     3,     3,     3,
       2,     1,     2,     1,     2,     2,     1,     3,     3,     2,
       2,     4,     1,     3,     1,     3,     2,     2,     1,     1,
       2,     2,     1,     1,     1,     1,     2,     2,     1,     1,
       2,     3,     2,     3,     3,     4,     3,     4,     5,     2,
       1,     1,     3,     4,     3,     4,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     4,     3,     3,     1,
       2,     2,     3,     3,     4,     1,     2,     1,     2,     2,
       5,     7,     5,     3,     4,     4,     3,     1,     0,     5,
       7,     9,     3,     2,     2,     2,     3,     1,     1,     6,
       5,     1,     0,     1,     2,     1
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       2,     1,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0
};

/* YYMERGER[RULE-NUM] -- Index of merging function for rule #RULE-NUM.  */
static const unsigned char yymerger[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     1,     1,     1,     1,     0,     0,     0,     0,
       0,     0,     0,     2,     2,     0,     3,     3,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     4,     4,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error.  */
static const unsigned short int yydefact[] =
{
       0,     0,     2,    27,    28,    29,     0,     0,     0,   337,
     338,     0,     0,     0,     0,     0,     0,     9,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    87,    88,    89,    90,    91,    92,   317,     3,    25,
      26,     0,    39,    40,    41,    42,    43,    67,    78,    93,
       0,    96,   100,   103,   106,   111,   114,   116,   118,   120,
     122,   124,   126,   148,   153,   154,   155,     0,   345,   305,
     304,   297,   298,   299,   303,   300,   301,     0,   302,     0,
       0,   343,   319,     0,     0,    25,    82,     0,     0,    84,
       0,     0,     0,    79,    80,     0,   139,     0,     0,     0,
       0,    15,     0,     0,     0,     0,     0,   328,   333,   334,
     335,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     4,     0,    45,    46,     0,     0,     0,    44,
       0,   200,   201,   167,   168,   169,   170,   171,   174,   175,
     172,   173,   166,   188,   189,   190,     0,    25,     0,   176,
     256,     0,   202,     0,     0,   251,   253,     0,   311,     0,
     315,   309,     0,     0,     0,    32,    23,     0,     0,    76,
       0,     0,     0,     0,    73,    74,     0,     0,     0,   135,
     133,   134,   129,   138,   136,   137,   130,   132,   131,   128,
       0,    93,    81,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   318,     0,     1,   344,     0,     0,     0,
       0,     0,     0,   332,     0,   307,     0,     0,     0,     0,
       0,     0,     0,   327,     0,   336,     0,   326,     0,     0,
      17,    16,    19,    18,     0,     0,     0,     0,     0,     0,
      49,    60,     0,     0,    64,     0,     0,   259,     0,    30,
     177,   260,   203,   254,     0,     0,     0,   240,   278,   252,
     279,   255,     0,     0,   312,   310,     0,   313,   316,    36,
       0,    37,     0,     0,    33,     0,     8,   323,     0,   141,
       0,    72,    69,     0,    76,     0,    71,   127,    97,    98,
      99,   101,   102,   104,   105,   109,   110,   107,   108,   112,
     113,   115,   117,   119,   121,   123,     0,   156,     0,    83,
       0,    86,    85,    14,     0,     0,   140,     0,     0,   186,
     187,     7,   150,     0,   179,     0,   143,   146,   145,   178,
     206,   180,     7,     0,     0,   181,   152,     0,     0,     0,
       0,     0,   328,   324,     0,    20,     5,   306,     0,     0,
      47,    53,     0,     0,     0,    50,     0,    54,    57,    44,
      60,     0,    58,    59,   325,     8,   257,     0,    94,   205,
       0,   282,   157,     0,   241,   242,   280,     0,     0,   308,
      95,   314,    38,    24,     0,    35,    34,    77,    22,    11,
     142,     0,    70,     0,    68,     0,     0,     0,   258,    12,
       0,     0,   147,     0,   216,     0,     0,   149,   144,   208,
     185,     0,   207,     0,   161,   223,     0,   164,   222,     0,
       0,   320,   322,   329,     0,     0,    21,    31,     0,    66,
      52,    51,    61,    55,     0,    65,     0,    56,     0,     0,
     261,   204,   286,   281,   283,   243,     0,   205,   273,   272,
       0,     0,     0,   262,   264,   268,   269,   274,   275,   284,
       0,    10,   125,   340,     0,    13,     0,     0,     0,     0,
       0,     0,     0,   163,   160,     0,   221,   151,     0,     0,
     328,    48,    62,     0,   277,   287,   276,   271,   205,     0,
       0,   291,   278,   267,   266,   290,   270,   285,   339,     0,
     158,   214,     0,   219,     0,   217,     0,     0,   184,     0,
     179,     0,   191,     0,     0,   224,   165,   205,     0,     0,
       0,   244,   246,   226,     0,   321,   330,     0,     0,   288,
     263,   265,     0,   289,     0,     0,    75,   215,     0,   212,
       0,   162,   198,     0,     0,   197,   192,     0,   183,     0,
     209,   211,   228,   159,   250,   205,   160,   225,     0,    63,
     292,   296,     0,   294,     0,   220,   213,   218,     0,   199,
       0,   182,     0,   195,     0,     0,   248,   249,   230,   229,
     227,   245,   247,   331,   295,   293,     0,   194,     0,   210,
     205,     0,   233,     0,   160,     0,   231,     0,   193,   237,
     232,   234,   205,     0,   235,     0,     0,   238,   205,   236,
       0,   239,   196
};

/* YYPDEFGOTO[NTERM-NUM].  */
static const short int yydefgoto[] =
{
      -1,    48,    95,   425,    50,    51,    52,    53,    54,    55,
      56,   176,   177,    57,   452,   381,   262,   383,   263,   264,
      58,   178,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,   200,   299,   300,
     345,   346,    74,    75,    76,    77,   393,   347,   539,   348,
     436,   159,   349,   350,   351,   352,   161,   531,   532,   533,
     162,   353,   462,   354,   569,   570,   355,   524,   525,   571,
     438,   597,   598,   439,   540,   541,   542,   164,   165,   166,
     472,   473,   474,   475,   476,   477,   478,   390,   280,   514,
     515,   170,    79,    80,   171,   172,    81,   173,    82,    83,
      84,   244,    85,    86,    87,    88,    89,    90,    91
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -533
static const short int yypact[] =
{
     798,   -74,  -533,  -533,  -533,  -533,  2802,   -29,  2845,  -533,
    -533,    61,     0,  2941,  2941,    61,    61,  -533,  2244,   -12,
      75,    86,   100,   108,  1463,   154,    30,   143,  1660,  2244,
     185,    61,   180,  1463,  1463,    24,  2244,   596,  1764,  1071,
    1860,  -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,   163,
    -533,   183,  -533,  -533,  -533,  -533,  -533,  -533,   559,   992,
    2706,  -533,   127,   244,   202,    33,   403,   220,   196,   223,
     342,    34,  -533,  -533,  -533,  -533,  -533,    43,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,  -533,    61,  -533,   324,
     911,  -533,  -533,    45,  1764,   304,  -533,  1441,  1764,  -533,
     311,  2244,  2244,  -533,  -533,   292,  -533,    87,  2244,   265,
    1071,  -533,  2244,  2244,  2244,  2244,   344,  2244,  -533,  -533,
    -533,   144,   367,  1463,   340,    61,    -9,   339,   172,   352,
    2244,  2244,  -533,   347,  -533,  -533,   596,   596,   323,   358,
     356,  -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,    61,   364,    58,  -533,
    1576,    61,   461,   885,   369,   225,   461,    61,  -533,   211,
    -533,  -533,  1169,  1267,   360,  -533,  -533,    -3,   -19,   378,
      61,  1463,    49,    61,  -533,  -533,  1956,  2244,    61,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,
    2244,  -533,  -533,  2706,  2706,  2706,  2706,  2706,  2706,  2706,
    2706,  2706,  2706,  2706,  2706,  2706,  2706,  2706,  2706,  2706,
    2706,  2244,  2244,  -533,   375,  -533,  -533,   388,   389,   397,
     405,    31,   406,  -533,    61,  -533,   392,   688,   206,   282,
     298,   300,   393,   410,   384,  -533,  1463,  -533,  2244,   420,
    -533,  -533,  -533,  -533,   303,   419,  2244,   423,   431,   430,
    -533,   429,   -18,    12,  -533,   541,  1463,  -533,  1674,  -533,
    -533,   438,  -533,  -533,  2706,   290,  2289,    16,   -24,  -533,
     102,  -533,   174,  2706,  -533,  -533,  1365,  -533,  -533,  -533,
    2052,  -533,   434,  2148,  -533,  2244,  -533,  -533,   -12,   -11,
     445,  -533,  -533,   308,  -533,   105,  -533,  -533,  -533,  -533,
    -533,   127,   127,   244,   244,   202,   202,   202,   202,    33,
      33,   403,   220,   196,   223,   342,   399,  -533,    77,  -533,
    1674,  -533,  -533,  -533,   413,  1764,  -533,   432,   702,  -533,
    -533,    61,  -533,  2706,  -533,   774,  -533,  -533,  -533,  1576,
     461,  -533,    61,  1145,    44,  -533,  -533,  2244,  1463,   -12,
    1463,  2244,  2244,  -533,   327,  -533,  -533,  -533,   457,   596,
    -533,  -533,   596,   451,   371,  -533,   453,  -533,  -533,     7,
    -533,   465,   463,   464,  -533,   348,  1576,    61,  -533,   461,
     470,  -533,  -533,   466,   467,  -533,   102,  1539,  2385,  -533,
    -533,  -533,  -533,  -533,   378,  -533,  -533,  -533,  -533,  -533,
    -533,   -12,  -533,  2244,  -533,  2706,   -12,   476,  -533,  -533,
      32,   482,  -533,   236,   487,   488,   702,  -533,  -533,  -533,
     487,   494,  -533,   236,  -533,  -533,   177,  -533,   197,   214,
     333,   520,  -533,  -533,   336,   477,  -533,  -533,   513,  -533,
    -533,  -533,  -533,  -533,   429,  -533,   596,  -533,   596,    61,
    -533,  -533,  -533,  -533,  -533,  -533,    61,   461,  -533,  1576,
      61,  1341,   510,   508,  -533,    83,   461,  -533,  -533,  -533,
     507,  -533,  -533,  -533,   -12,  -533,    61,   204,    25,   236,
     987,   521,   236,  -533,   -37,  2428,   197,  -533,  1463,   500,
    2244,  -533,  -533,   516,  -533,  -533,  -533,  -533,   461,  1865,
     193,  -533,   178,  -533,  -533,   243,  -533,  -533,  -533,   527,
    -533,  -533,   525,   523,   422,  -533,   248,   509,  -533,  1561,
     528,  1048,  -533,   532,   236,  -533,  -533,   461,  2706,   702,
     538,   535,  -533,  -533,   536,  -533,  -533,   542,   596,  -533,
    -533,  -533,   545,   243,  1637,  2524,  -533,  -533,  2706,  -533,
      47,  -533,  -533,  2706,  1245,  -533,  -533,   546,  -533,   255,
    -533,  -533,  -533,  -533,   212,   461,   -22,  -533,  1463,  -533,
    -533,  -533,   548,  -533,   543,  -533,  -533,  -533,   702,  -533,
      64,  -533,   236,  -533,   226,  2567,  -533,  -533,   281,   318,
    -533,  -533,  -533,  -533,  -533,  -533,   236,  -533,   261,  -533,
     461,   551,  -533,   547,   -32,  2663,   281,   550,  -533,  -533,
    -533,  -533,   461,   553,  -533,   549,  2706,  -533,   461,  -533,
     534,  -533,  -533
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -533,     1,     6,   291,  -533,  -533,  -533,  -533,  -533,  -533,
    -533,   354,  -533,  -533,   -23,  -533,   377,  -533,  -533,   269,
    -533,  -182,    -6,  -533,   -49,   320,   325,   272,   350,   436,
     433,   428,   435,   437,  -533,   170,  -533,  -533,     4,   319,
    -533,   310,  -533,  -533,   427,   473,  -310,  -533,  -533,  -533,
    -410,  -131,   -35,   305,  -533,  -388,  -533,  -533,   122,   125,
    -533,    54,  -204,  -307,    69,  -532,  -533,  -533,   101,  -328,
    -423,    68,    67,  -153,    53,  -533,    92,   -79,  -533,   506,
     109,  -533,   161,  -533,   200,  -533,  -533,  -164,  -268,   167,
     168,    57,  -533,  -533,   501,  -533,   -14,   512,  -533,  -533,
    -533,  -337,  -533,  -533,  -533,  -533,  -533,  -533,   589
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -343
static const short int yytable[] =
{
      96,   279,    99,   160,   303,   111,    49,   103,   104,   470,
     396,   202,   278,   487,   140,   227,   496,   100,   229,   230,
     107,   105,   106,   141,   142,   445,   437,     2,     2,   270,
      49,   423,    92,   426,     2,     2,   132,   124,   601,    49,
      49,   292,   373,   139,   157,    49,   537,     2,     2,   410,
       2,   622,     2,    97,   201,   210,   211,    78,   275,   160,
     609,   220,   160,   160,     2,   276,   364,     2,   293,   374,
     538,   294,   375,   110,   617,   538,   234,   250,   222,   526,
       2,   116,   101,   470,   290,   538,     2,   291,   480,   265,
     127,   129,   163,   224,   132,   437,    49,   223,   456,   376,
     157,    93,   377,   228,   157,   491,   130,    93,    93,   131,
     277,   521,   522,   257,   258,   261,    49,   333,   485,   489,
      93,   470,   278,    93,   395,    93,   433,   130,   160,    49,
     222,   249,   298,   586,   212,   213,   118,    93,   277,   221,
      93,   269,   139,   139,   139,   222,   433,    78,   163,   223,
     434,   163,   163,    93,   308,   309,   310,   112,   277,    93,
     416,   437,   267,   547,   536,   510,   470,   271,   113,   228,
     607,   282,   276,   106,   234,   440,   496,   277,    49,    49,
     247,     2,   114,   534,   397,   544,   296,    49,   106,   301,
     115,   398,   222,   235,   306,   414,     2,   201,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   201,   201,   201,   201,     2,   272,     2,   270,   564,
     281,   203,   208,   209,   534,   388,   204,   205,   573,     2,
     288,   222,   574,   386,   400,   334,   117,   106,   297,     2,
     336,   465,   380,   344,   396,   584,   596,   180,   585,   119,
     245,   181,    49,   588,    93,   270,   421,   590,   252,   222,
     510,   234,   125,   505,   492,   182,   491,   276,   201,    93,
     201,   379,    49,   123,   385,   510,   389,   201,   223,   494,
     399,   606,   276,   493,   409,   613,   495,   277,    93,   356,
      93,   492,    49,   357,   594,   386,   433,   283,   222,   217,
     160,   595,    93,   363,   549,   625,   277,   275,   594,   610,
     520,   513,    93,   216,   276,   595,   630,   223,   433,   277,
     277,     2,   512,   384,   225,   554,     2,   218,   134,   135,
     277,   394,   555,   572,   106,   492,   418,   201,   270,   206,
     207,   157,   592,   288,   344,   442,   448,   424,   592,   449,
     237,   344,   222,   454,   561,  -258,  -258,   512,   430,   344,
     435,   593,   469,   614,    49,   358,    49,   618,   219,   222,
     615,   600,   275,   389,     2,   139,   134,   135,   139,   276,
     139,   359,   139,   360,   277,   222,   366,   222,   180,   163,
     222,   412,   201,   460,    93,   413,   231,   481,   233,    93,
     594,   136,   483,   468,   429,   137,   619,   595,   242,   201,
     446,   259,   138,   260,   413,   441,   497,   443,   627,   499,
     413,   599,   248,   222,   631,   251,   336,   214,   215,   435,
      -8,    -8,   344,   502,   256,    -8,   469,    -8,   253,   435,
     265,   599,    -8,   461,   266,   435,   392,    93,   268,   136,
     289,   471,   274,   137,   222,   246,     2,   328,   134,   135,
     138,   451,   139,   133,   139,   503,   295,   179,   141,   142,
     518,   329,   504,   330,   469,   361,   506,   468,   283,   222,
     331,   511,   315,   316,   317,   318,   222,   415,   332,   201,
     362,   109,   519,   335,   523,   435,   530,   222,   435,   419,
     420,   121,   122,   365,    49,   367,   126,   128,   559,   560,
     369,   158,   169,   392,   370,   468,   511,   372,   511,   469,
     371,   461,   387,   201,   405,   579,   311,   312,   411,    93,
     516,   136,   201,   313,   314,   137,   447,   530,   422,   238,
     435,   450,   138,   453,     2,   344,   134,   135,   457,   201,
     458,   459,   201,   463,   139,   545,   464,   201,   255,   484,
     468,   277,   461,   471,   319,   320,   523,   158,   392,   486,
     344,   158,    -6,   488,   232,   158,   183,   184,   185,   490,
     435,   236,   498,   500,    49,   482,   239,   240,   241,   201,
     243,   461,   501,   508,   344,   509,   435,   517,   435,     2,
     435,   134,   135,   254,   535,   435,   546,   548,   471,   201,
     556,   557,   435,   304,   558,   562,   565,    93,   568,   136,
     201,   575,   576,   137,   378,   578,   577,   307,   580,   461,
     138,   604,   591,   605,   620,   603,   628,   621,   626,   629,
     632,   186,   382,   431,   403,   455,   323,   417,   187,   327,
     322,   188,   321,   566,   324,   428,   567,   325,   432,   608,
     305,   587,   611,   582,   461,   392,   616,   623,   602,   273,
     551,   507,    93,   285,   136,   304,   461,   552,   137,   226,
     553,     0,   461,   368,   286,   138,     0,     0,     0,   337,
       0,     2,     0,     0,   326,   141,   142,     0,     0,   392,
     338,     0,     0,     0,     0,     2,     0,     0,   392,   141,
     142,     0,     0,     0,     0,     0,     0,   404,     0,     0,
     407,     0,   408,     0,     0,   392,     0,     0,   392,     0,
       0,     0,     0,   392,     0,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   339,   340,   341,     0,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   339,
     340,   341,   327,     0,    93,   392,     0,     0,     0,     0,
       0,     0,     0,     0,   342,   337,     0,     2,    93,     0,
       0,   141,   142,     0,   304,   392,   338,     0,     0,     0,
       0,     0,     0,     0,     0,   343,   392,     0,  -342,     1,
       0,     2,     3,     4,     5,     0,     0,     6,     7,     8,
       0,     9,    10,    11,    12,     0,    13,    14,     0,     0,
       0,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   339,   340,   341,   444,   243,     0,     0,    15,    16,
     407,    17,    18,    19,    20,     0,     0,     0,     0,     0,
      93,     0,     0,     0,     0,     0,     0,     0,     0,    21,
     427,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,     0,    36,     0,     0,    37,
      38,   343,     0,    39,     0,     0,     0,    40,     2,     0,
       0,    41,    42,    43,    44,    45,    46,     0,     0,     0,
       0,     0,     0,     0,    47,     0,     0,     0,     0,     0,
       0,  -341,     1,     0,     2,     3,     4,     5,     0,     0,
       6,     7,     8,     0,     9,    10,    11,    12,     0,    13,
      14,     0,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,     0,     0,     0,     0,     0,
       0,    15,    16,     0,    17,    18,    19,    20,     0,     0,
       0,    93,     0,     0,     0,     0,     0,     0,     0,   156,
       0,     0,    21,   243,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,   527,    36,
       2,     0,    37,    38,   141,   142,    39,     0,     0,     0,
      40,     0,     0,     0,    41,    42,    43,    44,    45,    46,
       0,     0,     0,     0,     0,     0,     0,    47,     0,     0,
     189,   190,   191,   192,   193,   194,   195,   196,   197,   198,
       0,     0,     0,     0,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   339,   340,   341,     0,     0,   527,
       0,     2,     0,     0,     0,   141,   142,     0,     0,     0,
       0,     0,     0,    93,     0,     0,     0,     0,     0,     0,
       0,     0,     1,   528,     2,     3,     4,     5,     0,     0,
       6,     7,     8,   199,     9,    10,    11,    12,     0,    13,
      14,     0,     0,     0,   529,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   339,   340,   341,     0,     0,
       0,    15,    16,   167,    17,    18,    19,    20,     0,     0,
       0,     0,     0,     0,    93,     0,     0,     0,     0,     0,
       0,     0,    21,     0,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,     2,    36,
       0,     0,    37,    38,     0,   529,    39,   168,     0,     0,
      40,     0,     0,     0,    41,    42,    43,    44,    45,    46,
       1,     0,     2,     3,     4,     5,     0,    47,     6,     7,
       8,     0,     9,    10,    11,    12,     0,    13,    14,     0,
       0,     0,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   339,   340,   341,     0,     0,     0,     0,    15,
      16,   167,    17,    18,    19,    20,     0,     0,     0,     0,
       0,    93,     0,     0,     0,     0,     0,     0,     0,     0,
      21,     0,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,     0,    36,     2,     0,
      37,    38,   141,   142,    39,   284,     0,     0,    40,     0,
       0,     0,    41,    42,    43,    44,    45,    46,     1,     0,
       2,     3,     4,     5,     0,    47,     6,     7,     8,     0,
       9,    10,    11,    12,     0,    13,    14,     0,     0,     0,
       0,     0,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   339,   340,   341,     0,     0,    15,    16,     0,
      17,    18,    19,    20,     0,     0,     0,     0,     0,     0,
       0,    93,     0,     0,     0,     0,     0,     0,    21,     0,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,     2,    36,     0,     0,    37,    38,
       0,   589,    39,   287,     0,     0,    40,     0,     0,     0,
      41,    42,    43,    44,    45,    46,     1,     0,     2,     3,
       4,     5,     0,    47,     6,     7,     8,     0,     9,    10,
      11,    12,     0,    13,    14,     0,     0,     0,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   339,   340,
     466,     0,     0,     0,     0,    15,    16,     0,    17,    18,
      19,    20,     0,     0,     0,     0,     0,    93,     0,     0,
       0,     0,     0,     0,     0,     0,    21,     0,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,     0,    36,     2,     0,    37,    38,   141,   142,
      39,   401,     0,     0,    40,     0,     0,     0,    41,    42,
      43,    44,    45,    46,     1,     0,     2,     3,     4,     5,
       0,    47,     6,     7,     8,     0,     9,    10,    11,    12,
       0,    13,    14,     0,     0,     0,     0,     0,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,     0,     0,    15,    16,     0,    17,    18,    19,    20,
       0,     0,     0,     0,     0,     0,     0,    93,     0,     0,
       0,     0,     0,     0,    21,   156,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
       0,    36,     2,     0,    37,    38,   141,   142,    39,     0,
       0,     0,    40,     0,     0,     0,    41,    42,    43,    44,
      45,    46,     0,     0,     2,     3,     4,     5,     0,    47,
       6,     7,     8,     0,     0,     0,    11,    12,     0,    13,
      14,     0,     0,     0,     0,     0,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   339,   340,   466,     0,
       0,     0,     0,     0,    17,     0,    19,     0,     0,     0,
       0,     0,     0,     0,     0,    93,     0,     0,     0,     0,
       0,     0,   467,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   152,    31,    32,    33,    34,    93,     0,    36,
       2,     0,     0,    38,   141,   142,   108,     0,     0,     0,
      40,     0,     0,     0,    41,    42,    43,    44,    45,    46,
       0,     0,     0,     2,     3,     4,     5,     0,   563,     6,
       7,     8,     0,     0,     0,    11,    12,     2,    13,    14,
       0,     0,     0,     0,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   339,   340,   466,     0,     0,     0,
       0,     0,     0,    17,    18,    19,    20,     0,     0,     0,
       0,     0,     0,    93,     0,     0,     0,     0,     0,     0,
     581,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,     0,    31,    32,    33,    34,    93,     0,    36,     0,
       0,     0,    38,     0,     0,   108,     0,     0,     0,    40,
      93,     0,     0,    41,    42,    43,    44,    45,    46,     0,
       0,     0,     0,     0,     0,     0,   120,     2,     3,     4,
       5,   141,   142,     6,     7,     8,     0,     0,     0,    11,
      12,     0,    13,    14,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    17,    18,    19,
      20,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   155,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    31,    32,    33,    34,
      93,     0,    36,     0,     0,     0,    38,     0,   156,   108,
       0,     0,     0,    40,     0,     0,     0,    41,    42,    43,
      44,    45,    46,     2,     3,     4,     5,     0,     2,     6,
       7,     8,   141,   142,     0,    11,    12,     0,    13,    14,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    17,    18,    19,    20,     0,     0,     0,
       0,     0,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   339,   340,   466,   550,     0,     0,     0,     0,
       0,     0,    31,    32,    33,    34,    93,     0,    36,     0,
       0,    93,    38,     0,     0,   108,     0,     0,   174,    40,
     175,     0,     0,    41,    42,    43,    44,    45,    46,     2,
       3,     4,     5,     0,     0,     6,     7,     8,     0,     0,
       0,    11,    12,     0,    13,    14,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    17,
      18,    19,    20,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    31,    32,
      33,    34,    93,     0,    36,     0,     0,     0,    38,   302,
       0,   108,     0,     0,     0,    40,     0,     0,     0,    41,
      42,    43,    44,    45,    46,     2,     3,     4,     5,     0,
       0,     6,     7,     8,     0,     0,     0,    11,    12,     0,
      13,    14,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    17,    18,    19,    20,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    31,    32,    33,    34,    93,     0,
      36,     0,     0,     0,    38,     0,     0,   108,     0,     0,
       0,    40,   402,     0,     0,    41,    42,    43,    44,    45,
      46,     2,     3,     4,     5,     0,     0,     6,     7,     8,
       0,     0,     0,    11,    12,     0,    13,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    17,    18,    19,    20,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      31,    32,    33,    34,    93,     0,    36,     0,     0,     0,
      38,     0,     0,   108,     0,     0,     0,    40,   406,     0,
       0,    41,    42,    43,    44,    45,    46,     2,     3,     4,
       5,     0,     0,     6,     7,     8,     0,     0,     0,    11,
      12,     0,    13,    14,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    17,    18,    19,
      20,     0,     2,     3,     4,     5,     0,     0,     6,     7,
       8,     0,     0,     0,    11,    12,     0,    13,    14,     0,
       0,     0,     0,     0,     0,     0,    31,    32,    33,    34,
      93,     0,    36,     0,     0,     0,    38,     0,     0,   108,
       0,     0,    17,    40,    19,     0,     0,    41,    42,    43,
      44,    45,    46,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    31,    32,    33,    34,    93,     0,    36,     0,     0,
       0,    38,     0,     0,   108,     0,     0,     0,    40,   391,
       0,     0,    41,    42,    43,    44,    45,    46,     2,     3,
       4,     5,     0,     0,     6,     7,     8,     0,     0,     0,
      11,    12,     0,    13,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    17,     0,
      19,     2,     3,     4,     5,     0,     0,     6,     7,     8,
       0,     0,     0,    11,    12,     0,    13,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    31,    32,    33,
      34,    93,     0,    36,     0,     0,     0,    38,     0,     0,
     108,    17,     0,    19,    40,   479,     0,     0,    41,    42,
      43,    44,    45,    46,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      31,    32,    33,    34,    93,     0,    36,     0,     0,     0,
      38,     0,     0,   108,     0,     0,     0,    40,   543,     0,
       0,    41,    42,    43,    44,    45,    46,     2,     3,     4,
       5,     0,     0,     6,     7,     8,     0,     0,     0,    11,
      12,     0,    13,    14,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    17,     0,    19,
       2,     3,     4,     5,     0,     0,     6,     7,     8,     0,
       0,     0,    11,    12,     0,    13,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    31,    32,    33,    34,
      93,     0,    36,     0,     0,     0,    38,     0,     0,   108,
      17,     0,    19,    40,   583,     0,     0,    41,    42,    43,
      44,    45,    46,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    31,
      32,    33,    34,    93,     0,    36,     0,     0,     0,    38,
       0,     0,   108,     0,     0,     0,    40,   612,     0,     0,
      41,    42,    43,    44,    45,    46,     2,     3,     4,     5,
       0,     0,     6,     7,     8,     0,     0,     0,    11,    12,
       0,    13,    14,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    17,     0,    19,     2,
       3,     4,     5,     0,     0,     6,     7,     8,     0,     0,
       0,    11,    12,     0,    13,    14,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    31,    32,    33,    34,    93,
       0,    36,     0,     0,     0,    38,     0,     0,   108,    17,
       0,    19,    40,   624,     0,     0,    41,    42,    43,    44,
      45,    46,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    31,    32,
      33,    34,    93,     0,    36,     0,     0,     0,    38,     0,
       0,   108,     0,     0,     0,    40,     0,     0,     0,    41,
      42,    43,    44,    45,    46,     2,     3,     4,     5,     0,
       0,     6,     7,     8,     0,     0,     0,    11,    12,     0,
      13,    14,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    17,     0,    19,     2,     3,
       4,     5,     0,     0,     6,     7,     8,     0,     0,     0,
      11,    12,     0,    13,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    31,    32,    33,    34,    93,     0,
      36,     0,     0,     0,    94,     0,     0,     0,    17,     0,
      19,    40,     0,     0,     0,    41,    42,    43,    44,    45,
      46,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    31,    32,    33,
      34,    93,     0,    36,     0,     0,     0,    98,     0,     0,
       0,     0,     0,     0,    40,     0,     0,     0,    41,    42,
      43,    44,    45,    46,     2,     3,     4,     5,     0,     0,
       6,     7,     8,     0,     0,     0,    11,    12,     0,    13,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    17,     0,    19,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    32,    33,    34,    93,     0,    36,
       0,     0,     0,   102,     0,     0,     0,     0,     0,     0,
      40,     0,     0,     0,    41,    42,    43,    44,    45,    46
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       1,     3,     0,     0,     0,     5,     0,     7,     0,     0,
       0,     0,     9,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    11,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
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

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short int yyconfl[] =
{
       0,   258,     0,   258,     0,   258,     0,   258,     0,   258,
       0,   320,     0
};

static const short int yycheck[] =
{
       6,   165,     8,    38,   186,    19,     0,    13,    14,   397,
     278,    60,   165,   423,    37,    94,   439,    11,    97,    98,
      16,    15,    16,     7,     8,   362,   354,     3,     3,   160,
      24,   338,   106,   343,     3,     3,    35,    31,    60,    33,
      34,    60,    60,    37,    38,    39,    83,     3,     3,    60,
       3,    83,     3,    82,    60,    22,    23,     0,    82,    94,
     592,    27,    97,    98,     3,    89,   248,     3,    87,    87,
     107,    90,    90,    85,   606,   107,    87,    86,    87,   489,
       3,    24,    82,   471,    87,   107,     3,    90,   398,    82,
      33,    34,    38,    87,    93,   423,    90,   106,    91,    87,
      94,    76,    90,    97,    98,   433,    82,    76,    76,    85,
      94,    86,    87,   136,   137,   138,   110,    86,    86,   426,
      76,   509,   275,    76,   277,    76,    82,    82,   163,   123,
      87,   125,    83,    86,   101,   102,   106,    76,    94,   105,
      76,    83,   136,   137,   138,    87,    82,    90,    94,   106,
     106,    97,    98,    76,   203,   204,   205,    82,    94,    76,
      83,   489,   156,   500,   492,    82,   554,   161,    82,   163,
     106,   167,    89,   167,    87,   357,   599,    94,   172,   173,
     123,     3,    82,   490,    82,   495,   180,   181,   182,   183,
      82,    89,    87,   106,   188,    90,     3,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,     3,   162,     3,   349,   529,
     166,    94,    20,    21,   531,   274,    99,   100,   538,     3,
     173,    87,   539,   268,   283,   231,    82,   231,   181,     3,
     234,   394,   265,   237,   512,   555,   574,    84,   558,   106,
     106,    88,   246,   563,    76,   386,   335,   564,    86,    87,
      82,    87,    82,   467,    87,    82,   594,    89,   274,    76,
     276,   265,   266,    88,   268,    82,    83,   283,   106,    82,
     106,   588,    89,   106,   298,   595,    89,    94,    76,    83,
      76,    87,   286,    87,    82,   330,    82,    86,    87,   103,
     335,    89,    76,   246,   508,   615,    94,    82,    82,    83,
     106,   475,    76,    93,    89,    89,   626,   106,    82,    94,
      94,     3,   475,   266,     0,    82,     3,   104,     5,     6,
      94,   277,    89,   537,   328,    87,   330,   343,   469,    95,
      96,   335,    87,   286,   338,   359,   369,   341,    87,   372,
      85,   345,    87,   376,   106,     7,     8,   510,   352,   353,
     354,   106,   397,    82,   358,    83,   360,   106,    26,    87,
      89,   575,    82,    83,     3,   369,     5,     6,   372,    89,
     374,    83,   376,    83,    94,    87,    83,    87,    84,   335,
      87,    83,   398,   387,    76,    87,    85,   411,   106,    76,
      82,    78,   416,   397,   350,    82,   610,    89,    64,   415,
      83,    88,    89,    90,    87,   358,    83,   360,   622,    83,
      87,   574,    82,    87,   628,    86,   420,    24,    25,   423,
      82,    83,   426,   456,    87,    87,   471,    89,    86,   433,
      82,   594,    94,   389,    88,   439,   276,    76,    84,    78,
      90,   397,    83,    82,    87,    88,     3,    82,     5,     6,
      89,    90,   456,    36,   458,   459,    88,    40,     7,     8,
     484,    83,   466,    84,   509,    82,   470,   471,    86,    87,
      83,   475,   210,   211,   212,   213,    87,    88,    83,   495,
     106,    18,   486,    87,   488,   489,   490,    87,   492,    86,
      87,    28,    29,    83,   498,    86,    33,    34,    86,    87,
      87,    38,    39,   343,    83,   509,   510,    88,   512,   554,
      90,   467,    84,   529,    90,   548,   206,   207,    83,    76,
     476,    78,   538,   208,   209,    82,    79,   531,   106,   112,
     534,    90,    89,    90,     3,   539,     5,     6,    83,   555,
      87,    87,   558,    83,   548,   498,    90,   563,   131,    83,
     554,    94,   508,   509,   214,   215,   560,    94,   398,    87,
     564,    98,    85,    85,   101,   102,    17,    18,    19,    85,
     574,   108,    62,   106,   578,   415,   113,   114,   115,   595,
     117,   537,    79,    83,   588,    87,   590,    90,   592,     3,
     594,     5,     6,   130,    83,   599,   106,    91,   554,   615,
      83,    86,   606,   186,    91,   106,    88,    76,    86,    78,
     626,    83,    87,    82,    83,    83,    90,   200,    83,   575,
      89,    83,    86,    90,    83,   578,    83,    90,    88,    90,
     106,    82,   265,   352,   290,   376,   218,   328,    89,   222,
     217,    92,   216,   531,   219,   345,   531,   220,   353,   590,
     187,   560,   594,   554,   610,   495,   599,   614,   576,   163,
     509,   471,    76,   172,    78,   248,   622,   510,    82,    90,
     512,    -1,   628,   256,   172,    89,    -1,    -1,    -1,     1,
      -1,     3,    -1,    -1,   221,     7,     8,    -1,    -1,   529,
      12,    -1,    -1,    -1,    -1,     3,    -1,    -1,   538,     7,
       8,    -1,    -1,    -1,    -1,    -1,    -1,   290,    -1,    -1,
     293,    -1,   295,    -1,    -1,   555,    -1,    -1,   558,    -1,
      -1,    -1,    -1,   563,    -1,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,   335,    -1,    76,   595,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,     1,    -1,     3,    76,    -1,
      -1,     7,     8,    -1,   357,   615,    12,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   107,   626,    -1,     0,     1,
      -1,     3,     4,     5,     6,    -1,    -1,     9,    10,    11,
      -1,    13,    14,    15,    16,    -1,    18,    19,    -1,    -1,
      -1,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,   361,   362,    -1,    -1,    40,    41,
     413,    43,    44,    45,    46,    -1,    -1,    -1,    -1,    -1,
      76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      86,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    -1,    78,    -1,    -1,    81,
      82,   107,    -1,    85,    -1,    -1,    -1,    89,     3,    -1,
      -1,    93,    94,    95,    96,    97,    98,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   106,    -1,    -1,    -1,    -1,    -1,
      -1,     0,     1,    -1,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    -1,    13,    14,    15,    16,    -1,    18,
      19,    -1,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    -1,    -1,    -1,
      -1,    40,    41,    -1,    43,    44,    45,    46,    -1,    -1,
      -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      -1,    -1,    61,   500,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,     1,    78,
       3,    -1,    81,    82,     7,     8,    85,    -1,    -1,    -1,
      89,    -1,    -1,    -1,    93,    94,    95,    96,    97,    98,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   106,    -1,    -1,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      -1,    -1,    -1,    -1,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,     1,
      -1,     3,    -1,    -1,    -1,     7,     8,    -1,    -1,    -1,
      -1,    -1,    -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     1,    86,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    91,    13,    14,    15,    16,    -1,    18,
      19,    -1,    -1,    -1,   107,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    -1,
      -1,    40,    41,    42,    43,    44,    45,    46,    -1,    -1,
      -1,    -1,    -1,    -1,    76,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    61,    -1,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,     3,    78,
      -1,    -1,    81,    82,    -1,   107,    85,    86,    -1,    -1,
      89,    -1,    -1,    -1,    93,    94,    95,    96,    97,    98,
       1,    -1,     3,     4,     5,     6,    -1,   106,     9,    10,
      11,    -1,    13,    14,    15,    16,    -1,    18,    19,    -1,
      -1,    -1,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    -1,    -1,    40,
      41,    42,    43,    44,    45,    46,    -1,    -1,    -1,    -1,
      -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      61,    -1,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    -1,    78,     3,    -1,
      81,    82,     7,     8,    85,    86,    -1,    -1,    89,    -1,
      -1,    -1,    93,    94,    95,    96,    97,    98,     1,    -1,
       3,     4,     5,     6,    -1,   106,     9,    10,    11,    -1,
      13,    14,    15,    16,    -1,    18,    19,    -1,    -1,    -1,
      -1,    -1,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    40,    41,    -1,
      43,    44,    45,    46,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,     3,    78,    -1,    -1,    81,    82,
      -1,   106,    85,    86,    -1,    -1,    89,    -1,    -1,    -1,
      93,    94,    95,    96,    97,    98,     1,    -1,     3,     4,
       5,     6,    -1,   106,     9,    10,    11,    -1,    13,    14,
      15,    16,    -1,    18,    19,    -1,    -1,    -1,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    -1,    -1,    -1,    -1,    40,    41,    -1,    43,    44,
      45,    46,    -1,    -1,    -1,    -1,    -1,    76,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    -1,    78,     3,    -1,    81,    82,     7,     8,
      85,    86,    -1,    -1,    89,    -1,    -1,    -1,    93,    94,
      95,    96,    97,    98,     1,    -1,     3,     4,     5,     6,
      -1,   106,     9,    10,    11,    -1,    13,    14,    15,    16,
      -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    -1,    -1,    40,    41,    -1,    43,    44,    45,    46,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,    -1,    -1,
      -1,    -1,    -1,    -1,    61,    84,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      -1,    78,     3,    -1,    81,    82,     7,     8,    85,    -1,
      -1,    -1,    89,    -1,    -1,    -1,    93,    94,    95,    96,
      97,    98,    -1,    -1,     3,     4,     5,     6,    -1,   106,
       9,    10,    11,    -1,    -1,    -1,    15,    16,    -1,    18,
      19,    -1,    -1,    -1,    -1,    -1,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    -1,
      -1,    -1,    -1,    -1,    43,    -1,    45,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    76,    -1,    -1,    -1,    -1,
      -1,    -1,    83,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    72,    73,    74,    75,    76,    -1,    78,
       3,    -1,    -1,    82,     7,     8,    85,    -1,    -1,    -1,
      89,    -1,    -1,    -1,    93,    94,    95,    96,    97,    98,
      -1,    -1,    -1,     3,     4,     5,     6,    -1,   107,     9,
      10,    11,    -1,    -1,    -1,    15,    16,     3,    18,    19,
      -1,    -1,    -1,    -1,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    44,    45,    46,    -1,    -1,    -1,
      -1,    -1,    -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,
      83,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    -1,    72,    73,    74,    75,    76,    -1,    78,    -1,
      -1,    -1,    82,    -1,    -1,    85,    -1,    -1,    -1,    89,
      76,    -1,    -1,    93,    94,    95,    96,    97,    98,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   106,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    -1,    -1,    -1,    15,
      16,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    72,    73,    74,    75,
      76,    -1,    78,    -1,    -1,    -1,    82,    -1,    84,    85,
      -1,    -1,    -1,    89,    -1,    -1,    -1,    93,    94,    95,
      96,    97,    98,     3,     4,     5,     6,    -1,     3,     9,
      10,    11,     7,     8,    -1,    15,    16,    -1,    18,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    44,    45,    46,    -1,    -1,    -1,
      -1,    -1,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    -1,    -1,    -1,    -1,
      -1,    -1,    72,    73,    74,    75,    76,    -1,    78,    -1,
      -1,    76,    82,    -1,    -1,    85,    -1,    -1,    88,    89,
      90,    -1,    -1,    93,    94,    95,    96,    97,    98,     3,
       4,     5,     6,    -1,    -1,     9,    10,    11,    -1,    -1,
      -1,    15,    16,    -1,    18,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,
      44,    45,    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    72,    73,
      74,    75,    76,    -1,    78,    -1,    -1,    -1,    82,    83,
      -1,    85,    -1,    -1,    -1,    89,    -1,    -1,    -1,    93,
      94,    95,    96,    97,    98,     3,     4,     5,     6,    -1,
      -1,     9,    10,    11,    -1,    -1,    -1,    15,    16,    -1,
      18,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    43,    44,    45,    46,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    72,    73,    74,    75,    76,    -1,
      78,    -1,    -1,    -1,    82,    -1,    -1,    85,    -1,    -1,
      -1,    89,    90,    -1,    -1,    93,    94,    95,    96,    97,
      98,     3,     4,     5,     6,    -1,    -1,     9,    10,    11,
      -1,    -1,    -1,    15,    16,    -1,    18,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    44,    45,    46,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      72,    73,    74,    75,    76,    -1,    78,    -1,    -1,    -1,
      82,    -1,    -1,    85,    -1,    -1,    -1,    89,    90,    -1,
      -1,    93,    94,    95,    96,    97,    98,     3,     4,     5,
       6,    -1,    -1,     9,    10,    11,    -1,    -1,    -1,    15,
      16,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,    45,
      46,    -1,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    -1,    -1,    -1,    15,    16,    -1,    18,    19,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    72,    73,    74,    75,
      76,    -1,    78,    -1,    -1,    -1,    82,    -1,    -1,    85,
      -1,    -1,    43,    89,    45,    -1,    -1,    93,    94,    95,
      96,    97,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    72,    73,    74,    75,    76,    -1,    78,    -1,    -1,
      -1,    82,    -1,    -1,    85,    -1,    -1,    -1,    89,    90,
      -1,    -1,    93,    94,    95,    96,    97,    98,     3,     4,
       5,     6,    -1,    -1,     9,    10,    11,    -1,    -1,    -1,
      15,    16,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    -1,
      45,     3,     4,     5,     6,    -1,    -1,     9,    10,    11,
      -1,    -1,    -1,    15,    16,    -1,    18,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    72,    73,    74,
      75,    76,    -1,    78,    -1,    -1,    -1,    82,    -1,    -1,
      85,    43,    -1,    45,    89,    90,    -1,    -1,    93,    94,
      95,    96,    97,    98,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      72,    73,    74,    75,    76,    -1,    78,    -1,    -1,    -1,
      82,    -1,    -1,    85,    -1,    -1,    -1,    89,    90,    -1,
      -1,    93,    94,    95,    96,    97,    98,     3,     4,     5,
       6,    -1,    -1,     9,    10,    11,    -1,    -1,    -1,    15,
      16,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    -1,    45,
       3,     4,     5,     6,    -1,    -1,     9,    10,    11,    -1,
      -1,    -1,    15,    16,    -1,    18,    19,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    72,    73,    74,    75,
      76,    -1,    78,    -1,    -1,    -1,    82,    -1,    -1,    85,
      43,    -1,    45,    89,    90,    -1,    -1,    93,    94,    95,
      96,    97,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    72,
      73,    74,    75,    76,    -1,    78,    -1,    -1,    -1,    82,
      -1,    -1,    85,    -1,    -1,    -1,    89,    90,    -1,    -1,
      93,    94,    95,    96,    97,    98,     3,     4,     5,     6,
      -1,    -1,     9,    10,    11,    -1,    -1,    -1,    15,    16,
      -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    43,    -1,    45,     3,
       4,     5,     6,    -1,    -1,     9,    10,    11,    -1,    -1,
      -1,    15,    16,    -1,    18,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    72,    73,    74,    75,    76,
      -1,    78,    -1,    -1,    -1,    82,    -1,    -1,    85,    43,
      -1,    45,    89,    90,    -1,    -1,    93,    94,    95,    96,
      97,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    72,    73,
      74,    75,    76,    -1,    78,    -1,    -1,    -1,    82,    -1,
      -1,    85,    -1,    -1,    -1,    89,    -1,    -1,    -1,    93,
      94,    95,    96,    97,    98,     3,     4,     5,     6,    -1,
      -1,     9,    10,    11,    -1,    -1,    -1,    15,    16,    -1,
      18,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    43,    -1,    45,     3,     4,
       5,     6,    -1,    -1,     9,    10,    11,    -1,    -1,    -1,
      15,    16,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    72,    73,    74,    75,    76,    -1,
      78,    -1,    -1,    -1,    82,    -1,    -1,    -1,    43,    -1,
      45,    89,    -1,    -1,    -1,    93,    94,    95,    96,    97,
      98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    72,    73,    74,
      75,    76,    -1,    78,    -1,    -1,    -1,    82,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    -1,    -1,    -1,    93,    94,
      95,    96,    97,    98,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    -1,    -1,    -1,    15,    16,    -1,    18,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    43,    -1,    45,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    72,    73,    74,    75,    76,    -1,    78,
      -1,    -1,    -1,    82,    -1,    -1,    -1,    -1,    -1,    -1,
      89,    -1,    -1,    -1,    93,    94,    95,    96,    97,    98
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     1,     3,     4,     5,     6,     9,    10,    11,    13,
      14,    15,    16,    18,    19,    40,    41,    43,    44,    45,
      46,    61,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    78,    81,    82,    85,
      89,    93,    94,    95,    96,    97,    98,   106,   109,   110,
     112,   113,   114,   115,   116,   117,   118,   121,   128,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   150,   151,   152,   153,   199,   200,
     201,   204,   206,   207,   208,   210,   211,   212,   213,   214,
     215,   216,   106,    76,    82,   110,   130,    82,    82,   130,
     110,    82,    82,   130,   130,   110,   110,   146,    85,   153,
      85,   204,    82,    82,    82,    82,   199,    82,   106,   106,
     106,   153,   153,    88,   110,    82,   153,   199,   153,   199,
      82,    85,   109,   152,     5,     6,    78,    82,    89,   110,
     122,     7,     8,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    84,   110,   153,   159,
     160,   164,   168,   169,   185,   186,   187,    42,    86,   153,
     199,   202,   203,   205,    88,    90,   119,   120,   129,   152,
      84,    88,    82,    17,    18,    19,    82,    89,    92,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    91,
     145,   130,   132,    94,    99,   100,    95,    96,    20,    21,
      22,    23,   101,   102,    24,    25,    93,   103,   104,    26,
      27,   105,    87,   106,   110,     0,   216,   185,   110,   185,
     185,    85,   153,   106,    87,   106,   153,    85,   152,   153,
     153,   153,    64,   153,   209,   106,    88,   199,    82,   110,
      86,    86,    86,    86,   153,   152,    87,   122,   122,    88,
      90,   122,   124,   126,   127,    82,    88,   110,    84,    83,
     159,   110,   169,   187,    83,    82,    89,    94,   181,   195,
     196,   169,   146,    86,    86,   202,   205,    86,   199,    90,
      87,    90,    60,    87,    90,    88,   110,   199,    83,   146,
     147,   110,    83,   129,   152,   153,   110,   152,   132,   132,
     132,   133,   133,   134,   134,   135,   135,   135,   135,   136,
     136,   137,   138,   139,   140,   141,   153,   152,    82,    83,
      84,    83,    83,    86,   146,    87,   110,     1,    12,    57,
      58,    59,    86,   107,   110,   148,   149,   155,   157,   160,
     161,   162,   163,   169,   171,   174,    83,    87,    83,    83,
      83,    82,   106,   199,   129,    83,    83,    86,   152,    87,
      83,    90,    88,    60,    87,    90,    87,    90,    83,   110,
     122,   123,   124,   125,   199,   110,   160,    84,   132,    83,
     195,    90,   143,   154,   169,   181,   196,    82,    89,   106,
     132,    86,    90,   119,   152,    90,    90,   152,   152,   204,
      60,    83,    83,    87,    90,    88,    83,   147,   110,    86,
      87,   185,   106,   171,   110,   111,   154,    86,   149,   169,
     110,   111,   161,    82,   106,   110,   158,   177,   178,   181,
     129,   199,   204,   199,   153,   209,    83,    79,   122,   122,
      90,    90,   122,    90,   122,   127,    91,    83,    87,    87,
     110,   169,   170,    83,    90,   181,    59,    83,   110,   160,
     163,   169,   188,   189,   190,   191,   192,   193,   194,    90,
     154,   204,   143,   204,    83,    86,    87,   158,    85,   171,
      85,   177,    87,   106,    82,    89,   178,    83,    62,    83,
     106,    79,   122,   110,   110,   170,   110,   192,    83,    87,
      82,   110,   181,   195,   197,   198,   169,    90,   204,   110,
     106,    86,    87,   110,   175,   176,   158,     1,    86,   107,
     110,   165,   166,   167,   171,    83,   177,    83,   107,   156,
     182,   183,   184,    90,   154,   199,   106,   209,    91,   170,
      60,   190,   197,   198,    82,    89,    83,    86,    91,    86,
      87,   106,   106,   107,   154,    88,   166,   167,    86,   172,
     173,   177,   170,   154,   171,    83,    87,    90,    83,   122,
      83,    83,   188,    90,   154,   154,    86,   176,   154,   106,
     171,    86,    87,   106,    82,    89,   177,   179,   180,   181,
     170,    60,   184,   199,    83,    90,   171,   106,   172,   173,
      83,   179,    90,   154,    82,    89,   180,   173,   106,   170,
      83,    90,    83,   182,    90,   154,    88,   170,    83,    90,
     154,   170,   106
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
#line 96 "c.y"
    { ((*yyvalp).expr) = doidnsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 4:

/* Line 936 of glr.c  */
#line 102 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxunquote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 5:

/* Line 936 of glr.c  */
#line 104 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxunquote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 7:

/* Line 936 of glr.c  */
#line 110 "c.y"
    { ((*yyvalp).expr) = 0 ;}
    break;

  case 8:

/* Line 936 of glr.c  */
#line 115 "c.y"
    { ((*yyvalp).expr) = dotickesrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 10:

/* Line 936 of glr.c  */
#line 124 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elambda, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 11:

/* Line 936 of glr.c  */
#line 126 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elambda, nullelist(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 12:

/* Line 936 of glr.c  */
#line 131 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefrec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 13:

/* Line 936 of glr.c  */
#line 133 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefrec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 14:

/* Line 936 of glr.c  */
#line 135 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefrec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 15:

/* Line 936 of glr.c  */
#line 140 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 16:

/* Line 936 of glr.c  */
#line 145 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxquote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 17:

/* Line 936 of glr.c  */
#line 147 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxquote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 18:

/* Line 936 of glr.c  */
#line 149 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxquasi, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 19:

/* Line 936 of glr.c  */
#line 151 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxquasi, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 20:

/* Line 936 of glr.c  */
#line 156 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estx, doid("id"), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 21:

/* Line 936 of glr.c  */
#line 158 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 22:

/* Line 936 of glr.c  */
#line 163 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, Eelist,
				     (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0),
			  0, 0);
	;}
    break;

  case 23:

/* Line 936 of glr.c  */
#line 173 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 24:

/* Line 936 of glr.c  */
#line 175 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 27:

/* Line 936 of glr.c  */
#line 182 "c.y"
    { ((*yyvalp).expr) = dosym((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 28:

/* Line 936 of glr.c  */
#line 184 "c.y"
    { ((*yyvalp).expr) = doconst(ctx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 29:

/* Line 936 of glr.c  */
#line 186 "c.y"
    { ((*yyvalp).expr) = dostr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 30:

/* Line 936 of glr.c  */
#line 188 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 31:

/* Line 936 of glr.c  */
#line 190 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epair, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 32:

/* Line 936 of glr.c  */
#line 192 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, nullelist(), 0, 0, 0); ;}
    break;

  case 33:

/* Line 936 of glr.c  */
#line 194 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 34:

/* Line 936 of glr.c  */
#line 196 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 35:

/* Line 936 of glr.c  */
#line 198 "c.y"
    { Expr *ell = newexprsrc(&ctx->inp->src, Eellipsis, 0, 0, 0, 0);
          ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist,
                          invert(newexprsrc(&ctx->inp->src, Eelist,
                                            ell, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), 0, 0)), 0, 0, 0); ;}
    break;

  case 36:

/* Line 936 of glr.c  */
#line 203 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, nullelist(), 0, 0, 0); ;}
    break;

  case 37:

/* Line 936 of glr.c  */
#line 205 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 38:

/* Line 936 of glr.c  */
#line 207 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 45:

/* Line 936 of glr.c  */
#line 218 "c.y"
    { ((*yyvalp).expr) = doconst(ctx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 46:

/* Line 936 of glr.c  */
#line 220 "c.y"
    { ((*yyvalp).expr) = dostr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 47:

/* Line 936 of glr.c  */
#line 222 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 48:

/* Line 936 of glr.c  */
#line 224 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epair, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 49:

/* Line 936 of glr.c  */
#line 226 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, nullelist(), 0, 0, 0); ;}
    break;

  case 50:

/* Line 936 of glr.c  */
#line 228 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 51:

/* Line 936 of glr.c  */
#line 230 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 52:

/* Line 936 of glr.c  */
#line 232 "c.y"
    { Expr *ell = newexprsrc(&ctx->inp->src, Eellipsis, 0, 0, 0, 0);
          ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist,
                          invert(newexprsrc(&ctx->inp->src, Eelist,
                                            ell, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), 0, 0)), 0, 0, 0); ;}
    break;

  case 53:

/* Line 936 of glr.c  */
#line 237 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, nullelist(), 0, 0, 0); ;}
    break;

  case 54:

/* Line 936 of glr.c  */
#line 239 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 55:

/* Line 936 of glr.c  */
#line 241 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 56:

/* Line 936 of glr.c  */
#line 243 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 57:

/* Line 936 of glr.c  */
#line 245 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 60:

/* Line 936 of glr.c  */
#line 255 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 61:

/* Line 936 of glr.c  */
#line 257 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 62:

/* Line 936 of glr.c  */
#line 262 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, 
                          newbinopsrc(&ctx->inp->src, Eeq, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)),
                          nullelist(), 
                          0, 0); ;}
    break;

  case 63:

/* Line 936 of glr.c  */
#line 267 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, 
                          newbinopsrc(&ctx->inp->src, Eeq, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr)),
                          (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), 
                          0, 0); ;}
    break;

  case 64:

/* Line 936 of glr.c  */
#line 275 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 65:

/* Line 936 of glr.c  */
#line 277 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 66:

/* Line 936 of glr.c  */
#line 282 "c.y"
    { /* key may not actually bind variables, but this will get
             caught during compilation. */
          ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, Eelist,
				     (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0),
			  0, 0);
	;}
    break;

  case 68:

/* Line 936 of glr.c  */
#line 295 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 69:

/* Line 936 of glr.c  */
#line 297 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 70:

/* Line 936 of glr.c  */
#line 299 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 71:

/* Line 936 of glr.c  */
#line 301 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edot, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 72:

/* Line 936 of glr.c  */
#line 303 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earrow, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 73:

/* Line 936 of glr.c  */
#line 305 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epostinc, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 74:

/* Line 936 of glr.c  */
#line 307 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epostdec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 75:

/* Line 936 of glr.c  */
#line 309 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Econtainer, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 76:

/* Line 936 of glr.c  */
#line 314 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 77:

/* Line 936 of glr.c  */
#line 316 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 79:

/* Line 936 of glr.c  */
#line 322 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epreinc, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 80:

/* Line 936 of glr.c  */
#line 324 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epredec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 81:

/* Line 936 of glr.c  */
#line 326 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 82:

/* Line 936 of glr.c  */
#line 328 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Esizeofe, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 83:

/* Line 936 of glr.c  */
#line 330 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Esizeoft, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 84:

/* Line 936 of glr.c  */
#line 332 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypeofe, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 85:

/* Line 936 of glr.c  */
#line 334 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypeoft, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 86:

/* Line 936 of glr.c  */
#line 336 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Emkctype, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 87:

/* Line 936 of glr.c  */
#line 341 "c.y"
    { ((*yyvalp).kind) = Eref; ;}
    break;

  case 88:

/* Line 936 of glr.c  */
#line 343 "c.y"
    { ((*yyvalp).kind) = Ederef; ;}
    break;

  case 89:

/* Line 936 of glr.c  */
#line 345 "c.y"
    { ((*yyvalp).kind) = Euplus; ;}
    break;

  case 90:

/* Line 936 of glr.c  */
#line 347 "c.y"
    { ((*yyvalp).kind) = Euminus; ;}
    break;

  case 91:

/* Line 936 of glr.c  */
#line 349 "c.y"
    { ((*yyvalp).kind) = Eutwiddle; ;}
    break;

  case 92:

/* Line 936 of glr.c  */
#line 351 "c.y"
    { ((*yyvalp).kind) = Eunot; ;}
    break;

  case 94:

/* Line 936 of glr.c  */
#line 357 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecast, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 95:

/* Line 936 of glr.c  */
#line 359 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Excast, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 97:

/* Line 936 of glr.c  */
#line 365 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Emul, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 98:

/* Line 936 of glr.c  */
#line 367 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ediv, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 99:

/* Line 936 of glr.c  */
#line 369 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Emod, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 101:

/* Line 936 of glr.c  */
#line 375 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eadd, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 102:

/* Line 936 of glr.c  */
#line 377 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Esub, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 104:

/* Line 936 of glr.c  */
#line 383 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eshl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 105:

/* Line 936 of glr.c  */
#line 385 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eshr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 107:

/* Line 936 of glr.c  */
#line 391 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Elt, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 108:

/* Line 936 of glr.c  */
#line 393 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Egt, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 109:

/* Line 936 of glr.c  */
#line 395 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ele, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 110:

/* Line 936 of glr.c  */
#line 397 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ege, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 112:

/* Line 936 of glr.c  */
#line 403 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eeq, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 113:

/* Line 936 of glr.c  */
#line 405 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eneq, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 115:

/* Line 936 of glr.c  */
#line 411 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eband, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 117:

/* Line 936 of glr.c  */
#line 417 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ebxor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 119:

/* Line 936 of glr.c  */
#line 423 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ebor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 121:

/* Line 936 of glr.c  */
#line 429 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eland, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 123:

/* Line 936 of glr.c  */
#line 435 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 125:

/* Line 936 of glr.c  */
#line 441 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Econd, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 127:

/* Line 936 of glr.c  */
#line 447 "c.y"
    { if((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.kind) == Eg)
	  	((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0);
	  else
	  	((*yyvalp).expr) = newgopsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr));
	;}
    break;

  case 128:

/* Line 936 of glr.c  */
#line 456 "c.y"
    { ((*yyvalp).kind) = Eg; ;}
    break;

  case 129:

/* Line 936 of glr.c  */
#line 458 "c.y"
    { ((*yyvalp).kind) = Egadd; ;}
    break;

  case 130:

/* Line 936 of glr.c  */
#line 460 "c.y"
    { ((*yyvalp).kind) = Egband; ;}
    break;

  case 131:

/* Line 936 of glr.c  */
#line 462 "c.y"
    { ((*yyvalp).kind) = Egbor; ;}
    break;

  case 132:

/* Line 936 of glr.c  */
#line 464 "c.y"
    { ((*yyvalp).kind) = Egbxor; ;}
    break;

  case 133:

/* Line 936 of glr.c  */
#line 466 "c.y"
    { ((*yyvalp).kind) = Egdiv; ;}
    break;

  case 134:

/* Line 936 of glr.c  */
#line 468 "c.y"
    { ((*yyvalp).kind) = Egmod; ;}
    break;

  case 135:

/* Line 936 of glr.c  */
#line 470 "c.y"
    { ((*yyvalp).kind) = Egmul; ;}
    break;

  case 136:

/* Line 936 of glr.c  */
#line 472 "c.y"
    { ((*yyvalp).kind) = Egshl; ;}
    break;

  case 137:

/* Line 936 of glr.c  */
#line 474 "c.y"
    { ((*yyvalp).kind) = Egshr; ;}
    break;

  case 138:

/* Line 936 of glr.c  */
#line 476 "c.y"
    { ((*yyvalp).kind) = Egsub; ;}
    break;

  case 139:

/* Line 936 of glr.c  */
#line 481 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 140:

/* Line 936 of glr.c  */
#line 483 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 141:

/* Line 936 of glr.c  */
#line 488 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 142:

/* Line 936 of glr.c  */
#line 490 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
			  newexprsrc(&ctx->inp->src, Eellipsis, 0, 0, 0, 0),
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0);
	;}
    break;

  case 143:

/* Line 936 of glr.c  */
#line 498 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 144:

/* Line 936 of glr.c  */
#line 500 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 147:

/* Line 936 of glr.c  */
#line 507 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 149:

/* Line 936 of glr.c  */
#line 513 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Enames, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 150:

/* Line 936 of glr.c  */
#line 515 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Enames, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 151:

/* Line 936 of glr.c  */
#line 520 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elapply, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 152:

/* Line 936 of glr.c  */
#line 522 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elapply, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 156:

/* Line 936 of glr.c  */
#line 533 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecomma, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 158:

/* Line 936 of glr.c  */
#line 542 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 159:

/* Line 936 of glr.c  */
#line 547 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 160:

/* Line 936 of glr.c  */
#line 549 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 161:

/* Line 936 of glr.c  */
#line 554 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecls, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 162:

/* Line 936 of glr.c  */
#line 556 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecls, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 163:

/* Line 936 of glr.c  */
#line 558 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecls, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 164:

/* Line 936 of glr.c  */
#line 563 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 165:

/* Line 936 of glr.c  */
#line 565 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 166:

/* Line 936 of glr.c  */
#line 570 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Evoid, 0, 0, 0, 0); ;}
    break;

  case 167:

/* Line 936 of glr.c  */
#line 572 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebool, 0, 0, 0, 0); ;}
    break;

  case 168:

/* Line 936 of glr.c  */
#line 574 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Echar, 0, 0, 0, 0); ;}
    break;

  case 169:

/* Line 936 of glr.c  */
#line 576 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eshort, 0, 0, 0, 0); ;}
    break;

  case 170:

/* Line 936 of glr.c  */
#line 578 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eint, 0, 0, 0, 0); ;}
    break;

  case 171:

/* Line 936 of glr.c  */
#line 580 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elong, 0, 0, 0, 0); ;}
    break;

  case 172:

/* Line 936 of glr.c  */
#line 582 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efloat, 0, 0, 0, 0); ;}
    break;

  case 173:

/* Line 936 of glr.c  */
#line 584 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edouble, 0, 0, 0, 0); ;}
    break;

  case 174:

/* Line 936 of glr.c  */
#line 586 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Esigned, 0, 0, 0, 0); ;}
    break;

  case 175:

/* Line 936 of glr.c  */
#line 588 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eunsigned, 0, 0, 0, 0); ;}
    break;

  case 176:

/* Line 936 of glr.c  */
#line 592 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 177:

/* Line 936 of glr.c  */
#line 594 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 178:

/* Line 936 of glr.c  */
#line 599 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 179:

/* Line 936 of glr.c  */
#line 601 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 182:

/* Line 936 of glr.c  */
#line 608 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 183:

/* Line 936 of glr.c  */
#line 610 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), Znull(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 184:

/* Line 936 of glr.c  */
#line 612 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 185:

/* Line 936 of glr.c  */
#line 614 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 186:

/* Line 936 of glr.c  */
#line 619 "c.y"
    { ((*yyvalp).kind) = Estruct; ;}
    break;

  case 187:

/* Line 936 of glr.c  */
#line 621 "c.y"
    { ((*yyvalp).kind) = Eunion; ;}
    break;

  case 188:

/* Line 936 of glr.c  */
#line 626 "c.y"
    { ((*yyvalp).kind) = Estruct; ;}
    break;

  case 189:

/* Line 936 of glr.c  */
#line 628 "c.y"
    { ((*yyvalp).kind) = Eunion; ;}
    break;

  case 190:

/* Line 936 of glr.c  */
#line 630 "c.y"
    { ((*yyvalp).kind) = Eenum; ;}
    break;

  case 191:

/* Line 936 of glr.c  */
#line 635 "c.y"
    {
		/* labels yield null struct_declarations */
		if((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr))
			((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
					(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0);
		else
			((*yyvalp).expr) = Znull();
	;}
    break;

  case 192:

/* Line 936 of glr.c  */
#line 644 "c.y"
    {
		/* labels yield null struct_declarations */
	 	if((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr))
			((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0);
		else
			((*yyvalp).expr) = Znull();
	;}
    break;

  case 193:

/* Line 936 of glr.c  */
#line 655 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efields, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 194:

/* Line 936 of glr.c  */
#line 657 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efields, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), Znull(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 195:

/* Line 936 of glr.c  */
#line 659 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efields, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 196:

/* Line 936 of glr.c  */
#line 661 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebitfield, (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 197:

/* Line 936 of glr.c  */
#line 664 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 198:

/* Line 936 of glr.c  */
#line 666 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 199:

/* Line 936 of glr.c  */
#line 671 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 207:

/* Line 936 of glr.c  */
#line 693 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 208:

/* Line 936 of glr.c  */
#line 695 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 209:

/* Line 936 of glr.c  */
#line 700 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 210:

/* Line 936 of glr.c  */
#line 702 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 212:

/* Line 936 of glr.c  */
#line 711 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 213:

/* Line 936 of glr.c  */
#line 713 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 214:

/* Line 936 of glr.c  */
#line 715 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 215:

/* Line 936 of glr.c  */
#line 717 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 216:

/* Line 936 of glr.c  */
#line 719 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 217:

/* Line 936 of glr.c  */
#line 724 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 218:

/* Line 936 of glr.c  */
#line 726 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 219:

/* Line 936 of glr.c  */
#line 733 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenumel, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 220:

/* Line 936 of glr.c  */
#line 735 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenumel, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 221:

/* Line 936 of glr.c  */
#line 740 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 223:

/* Line 936 of glr.c  */
#line 746 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 224:

/* Line 936 of glr.c  */
#line 748 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 225:

/* Line 936 of glr.c  */
#line 750 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 226:

/* Line 936 of glr.c  */
#line 752 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 227:

/* Line 936 of glr.c  */
#line 754 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 228:

/* Line 936 of glr.c  */
#line 756 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 231:

/* Line 936 of glr.c  */
#line 763 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 232:

/* Line 936 of glr.c  */
#line 768 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 233:

/* Line 936 of glr.c  */
#line 770 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, 0, 0, 0); ;}
    break;

  case 234:

/* Line 936 of glr.c  */
#line 772 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 235:

/* Line 936 of glr.c  */
#line 774 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 236:

/* Line 936 of glr.c  */
#line 776 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 237:

/* Line 936 of glr.c  */
#line 778 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, 0, Znull(), 0, 0); ;}
    break;

  case 238:

/* Line 936 of glr.c  */
#line 780 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 239:

/* Line 936 of glr.c  */
#line 782 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 240:

/* Line 936 of glr.c  */
#line 787 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, 0, 0, 0, 0); ;}
    break;

  case 241:

/* Line 936 of glr.c  */
#line 789 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, 0, 0, 0, 0); ;}
    break;

  case 242:

/* Line 936 of glr.c  */
#line 791 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 243:

/* Line 936 of glr.c  */
#line 793 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 244:

/* Line 936 of glr.c  */
#line 798 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 245:

/* Line 936 of glr.c  */
#line 800 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 246:

/* Line 936 of glr.c  */
#line 805 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 247:

/* Line 936 of glr.c  */
#line 807 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 248:

/* Line 936 of glr.c  */
#line 812 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 249:

/* Line 936 of glr.c  */
#line 814 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 250:

/* Line 936 of glr.c  */
#line 816 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 251:

/* Line 936 of glr.c  */
#line 821 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypename, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 252:

/* Line 936 of glr.c  */
#line 823 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypename, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 254:

/* Line 936 of glr.c  */
#line 829 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 255:

/* Line 936 of glr.c  */
#line 831 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 256:

/* Line 936 of glr.c  */
#line 836 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 257:

/* Line 936 of glr.c  */
#line 838 "c.y"
    { ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0)); ;}
    break;

  case 258:

/* Line 936 of glr.c  */
#line 841 "c.y"
    { ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0)); ;}
    break;

  case 259:

/* Line 936 of glr.c  */
#line 844 "c.y"
    { ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, 0,
			  newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0)); ;}
    break;

  case 260:

/* Line 936 of glr.c  */
#line 847 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 261:

/* Line 936 of glr.c  */
#line 849 "c.y"
    { ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0)); ;}
    break;

  case 262:

/* Line 936 of glr.c  */
#line 854 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 263:

/* Line 936 of glr.c  */
#line 856 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 264:

/* Line 936 of glr.c  */
#line 861 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 265:

/* Line 936 of glr.c  */
#line 863 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 266:

/* Line 936 of glr.c  */
#line 868 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 267:

/* Line 936 of glr.c  */
#line 870 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 268:

/* Line 936 of glr.c  */
#line 872 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 270:

/* Line 936 of glr.c  */
#line 878 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 271:

/* Line 936 of glr.c  */
#line 880 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 272:

/* Line 936 of glr.c  */
#line 885 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 273:

/* Line 936 of glr.c  */
#line 887 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 276:

/* Line 936 of glr.c  */
#line 894 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 277:

/* Line 936 of glr.c  */
#line 899 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 280:

/* Line 936 of glr.c  */
#line 906 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 281:

/* Line 936 of glr.c  */
#line 911 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 282:

/* Line 936 of glr.c  */
#line 913 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, 0, 0, 0); ;}
    break;

  case 283:

/* Line 936 of glr.c  */
#line 915 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 284:

/* Line 936 of glr.c  */
#line 917 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 285:

/* Line 936 of glr.c  */
#line 919 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 286:

/* Line 936 of glr.c  */
#line 921 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, 0, nullelist(), 0, 0); ;}
    break;

  case 287:

/* Line 936 of glr.c  */
#line 923 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 288:

/* Line 936 of glr.c  */
#line 925 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 289:

/* Line 936 of glr.c  */
#line 930 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 291:

/* Line 936 of glr.c  */
#line 936 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 292:

/* Line 936 of glr.c  */
#line 938 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 293:

/* Line 936 of glr.c  */
#line 940 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 294:

/* Line 936 of glr.c  */
#line 942 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 295:

/* Line 936 of glr.c  */
#line 944 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 296:

/* Line 936 of glr.c  */
#line 946 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 306:

/* Line 936 of glr.c  */
#line 963 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxunquote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 307:

/* Line 936 of glr.c  */
#line 968 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eglobal, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 308:

/* Line 936 of glr.c  */
#line 973 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 309:

/* Line 936 of glr.c  */
#line 978 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 310:

/* Line 936 of glr.c  */
#line 980 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 311:

/* Line 936 of glr.c  */
#line 985 "c.y"
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

  case 312:

/* Line 936 of glr.c  */
#line 995 "c.y"
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

  case 313:

/* Line 936 of glr.c  */
#line 1005 "c.y"
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

  case 314:

/* Line 936 of glr.c  */
#line 1018 "c.y"
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

  case 315:

/* Line 936 of glr.c  */
#line 1034 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 316:

/* Line 936 of glr.c  */
#line 1036 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 317:

/* Line 936 of glr.c  */
#line 1041 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Enil, 0, 0, 0, 0); ;}
    break;

  case 318:

/* Line 936 of glr.c  */
#line 1043 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 319:

/* Line 936 of glr.c  */
#line 1045 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 320:

/* Line 936 of glr.c  */
#line 1050 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eif, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 321:

/* Line 936 of glr.c  */
#line 1052 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eif, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (7))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (7))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (7))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 322:

/* Line 936 of glr.c  */
#line 1055 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eswitch, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 323:

/* Line 936 of glr.c  */
#line 1060 "c.y"
    { ((*yyvalp).expr) = Zlabelsrc(&(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr)->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 324:

/* Line 936 of glr.c  */
#line 1062 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), NULL, NULL); ;}
    break;

  case 325:

/* Line 936 of glr.c  */
#line 1064 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ematch, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), NULL, NULL); ;}
    break;

  case 326:

/* Line 936 of glr.c  */
#line 1066 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefault, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), NULL, NULL, NULL); ;}
    break;

  case 328:

/* Line 936 of glr.c  */
#line 1072 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 329:

/* Line 936 of glr.c  */
#line 1077 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ewhile, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 330:

/* Line 936 of glr.c  */
#line 1079 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edo, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (7))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 331:

/* Line 936 of glr.c  */
#line 1081 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (9))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (9))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (9))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((9) - (9))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 332:

/* Line 936 of glr.c  */
#line 1086 "c.y"
    { ((*yyvalp).expr) = Zgotosrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 333:

/* Line 936 of glr.c  */
#line 1088 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Econtinue, 0, 0, 0, 0); ;}
    break;

  case 334:

/* Line 936 of glr.c  */
#line 1090 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebreak, 0, 0, 0, 0); ;}
    break;

  case 335:

/* Line 936 of glr.c  */
#line 1092 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eret, 0, 0, 0, 0); ;}
    break;

  case 336:

/* Line 936 of glr.c  */
#line 1094 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eret, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 337:

/* Line 936 of glr.c  */
#line 1099 "c.y"
    { ((*yyvalp).kind) = Edefine; ;}
    break;

  case 338:

/* Line 936 of glr.c  */
#line 1101 "c.y"
    { ((*yyvalp).kind) = Edefloc; ;}
    break;

  case 339:

/* Line 936 of glr.c  */
#line 1106 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr)->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (6))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 340:

/* Line 936 of glr.c  */
#line 1108 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr)->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), nullelist(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 345:

/* Line 936 of glr.c  */
#line 1122 "c.y"
    { dotop(ctx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr)); ;}
    break;



/* Line 936 of glr.c  */
#line 4341 "c.tab.c"
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
#line 1125 "c.y"

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

