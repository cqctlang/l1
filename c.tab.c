
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
#line 78 "c.y"

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
#define YYFINAL  204
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2526

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  100
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  102
/* YYNRULES -- Number of rules.  */
#define YYNRULES  321
/* YYNRULES -- Number of states.  */
#define YYNSTATES  585
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule.  */
#define YYMAXRHS 9
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule.  */
#define YYMAXLEFT 0

/* YYTRANSLATE(X) -- Bison symbol number corresponding to X.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   326

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    88,     2,    82,    98,    90,    83,     2,
      73,    74,    84,    85,    77,    86,    81,    89,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    78,    97,
      91,    96,    92,    95,    99,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    79,     2,    80,    93,     2,    72,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    75,    94,    76,    87,     2,     2,     2,
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
      65,    66,    67,    68,    69,    70,    71
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     5,     9,    11,    13,    19,    24,    30,
      37,    42,    45,    49,    51,    55,    57,    59,    61,    63,
      65,    69,    72,    76,    81,    85,    89,    94,    96,    98,
     100,   102,   107,   111,   116,   120,   124,   127,   130,   139,
     141,   145,   147,   149,   152,   155,   158,   161,   166,   169,
     174,   178,   180,   182,   184,   186,   188,   190,   192,   197,
     202,   204,   208,   212,   216,   218,   222,   226,   228,   232,
     236,   238,   242,   246,   250,   254,   256,   260,   264,   266,
     270,   272,   276,   278,   282,   284,   288,   290,   294,   296,
     302,   304,   308,   312,   316,   318,   320,   322,   324,   326,
     328,   330,   332,   334,   336,   338,   340,   344,   346,   349,
     351,   354,   356,   358,   361,   363,   369,   374,   382,   389,
     396,   401,   403,   405,   407,   411,   413,   418,   421,   422,
     425,   431,   435,   437,   441,   443,   445,   447,   449,   451,
     453,   455,   457,   459,   461,   463,   466,   468,   470,   472,
     474,   481,   487,   493,   498,   503,   507,   510,   512,   514,
     516,   519,   525,   534,   539,   543,   546,   549,   553,   555,
     557,   559,   562,   564,   565,   567,   570,   573,   575,   579,
     581,   586,   592,   598,   605,   609,   614,   619,   625,   628,
     630,   634,   636,   640,   643,   645,   647,   651,   656,   660,
     666,   671,   673,   676,   679,   683,   685,   689,   691,   695,
     699,   703,   706,   708,   710,   713,   717,   720,   724,   728,
     733,   737,   742,   748,   750,   753,   755,   758,   761,   763,
     767,   770,   774,   776,   778,   781,   786,   789,   794,   796,
     800,   802,   806,   809,   812,   814,   816,   819,   822,   824,
     826,   828,   830,   833,   836,   838,   840,   843,   847,   850,
     854,   858,   863,   867,   872,   878,   881,   883,   885,   889,
     894,   898,   903,   907,   909,   911,   913,   915,   917,   919,
     921,   923,   925,   929,   933,   935,   938,   941,   945,   949,
     954,   956,   959,   961,   964,   967,   973,   981,   987,   991,
     996,  1000,  1002,  1003,  1009,  1017,  1027,  1031,  1034,  1037,
    1040,  1044,  1046,  1053,  1059,  1069,  1078,  1084,  1086,  1087,
    1089,  1092
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const short int yyrhs[] =
{
     199,     0,    -1,     3,    -1,   101,    72,   101,    -1,   101,
      -1,    42,    -1,   104,    73,   131,    74,   188,    -1,   104,
      73,    74,   188,    -1,    14,   101,    75,   130,    76,    -1,
      14,   101,    75,   130,    77,    76,    -1,    14,   101,    75,
      76,    -1,    44,   188,    -1,   136,    78,   136,    -1,   108,
      -1,   109,    77,   108,    -1,   101,    -1,   102,    -1,     4,
      -1,     5,    -1,     6,    -1,    73,   137,    74,    -1,    79,
      80,    -1,    79,   112,    80,    -1,    79,   112,    77,    80,
      -1,    79,    78,    80,    -1,    79,   109,    80,    -1,    79,
     109,    77,    80,    -1,   105,    -1,   106,    -1,   107,    -1,
     110,    -1,   111,    79,   137,    80,    -1,   111,    73,    74,
      -1,   111,    73,   112,    74,    -1,   111,    81,   101,    -1,
     111,    16,   101,    -1,   111,    17,    -1,   111,    18,    -1,
      15,    73,   137,    77,   168,    77,   101,    74,    -1,   136,
      -1,   112,    77,   136,    -1,    10,    -1,   111,    -1,    17,
     114,    -1,    18,   114,    -1,   115,   116,    -1,     9,   114,
      -1,     9,    73,   168,    74,    -1,   113,   114,    -1,   113,
      73,   168,    74,    -1,    82,   168,    82,    -1,    83,    -1,
      84,    -1,    85,    -1,    86,    -1,    87,    -1,    88,    -1,
     114,    -1,    73,   168,    74,   116,    -1,    75,   137,    76,
     116,    -1,   116,    -1,   117,    84,   116,    -1,   117,    89,
     116,    -1,   117,    90,   116,    -1,   117,    -1,   118,    85,
     117,    -1,   118,    86,   117,    -1,   118,    -1,   119,    19,
     118,    -1,   119,    20,   118,    -1,   119,    -1,   120,    91,
     119,    -1,   120,    92,   119,    -1,   120,    21,   119,    -1,
     120,    22,   119,    -1,   120,    -1,   121,    23,   120,    -1,
     121,    24,   120,    -1,   121,    -1,   122,    83,   121,    -1,
     122,    -1,   123,    93,   122,    -1,   123,    -1,   124,    94,
     123,    -1,   124,    -1,   125,    25,   124,    -1,   125,    -1,
     126,    26,   125,    -1,   126,    -1,   126,    95,   137,    78,
     127,    -1,   127,    -1,   114,    37,   168,    -1,   114,    38,
     136,    -1,   114,   129,   136,    -1,    96,    -1,    27,    -1,
      28,    -1,    29,    -1,    30,    -1,    31,    -1,    32,    -1,
      33,    -1,    34,    -1,    35,    -1,    36,    -1,   101,    -1,
     130,    77,   101,    -1,   130,    -1,   130,    59,    -1,   133,
      -1,   132,   133,    -1,   141,    -1,   139,    -1,     1,    97,
      -1,   128,    -1,    43,   137,    75,   132,    76,    -1,    43,
     137,    75,    76,    -1,    43,   137,    98,   137,    75,   132,
      76,    -1,    43,   137,    98,   137,    75,    76,    -1,    45,
      73,   136,    77,   112,    74,    -1,    45,    73,   136,    74,
      -1,   134,    -1,   135,    -1,   136,    -1,   137,    77,   136,
      -1,   127,    -1,    11,   154,   160,    97,    -1,    99,   138,
      -1,    -1,   154,    97,    -1,    99,   138,   154,   142,    97,
      -1,   154,   142,    97,    -1,   160,    -1,   142,    77,   160,
      -1,    55,    -1,    46,    -1,    47,    -1,    48,    -1,    49,
      -1,    50,    -1,    53,    -1,    54,    -1,    51,    -1,    52,
      -1,   143,    -1,   144,   143,    -1,   144,    -1,   101,    -1,
     146,    -1,   157,    -1,   147,   103,    75,   148,   150,    76,
      -1,   147,    75,   148,   150,    76,    -1,   147,   103,    75,
     150,    76,    -1,   147,    75,   150,    76,    -1,   147,   103,
      75,    76,    -1,   147,    75,    76,    -1,   147,   103,    -1,
      56,    -1,    57,    -1,   149,    -1,   148,   149,    -1,    99,
     138,   154,   155,    97,    -1,    99,    99,   138,   154,   156,
      78,   138,    97,    -1,    99,   138,   154,    97,    -1,   154,
     155,    97,    -1,   101,    78,    -1,     1,    97,    -1,    99,
     138,    97,    -1,     7,    -1,     8,    -1,   151,    -1,   151,
     152,    -1,   152,    -1,    -1,   145,    -1,   152,   145,    -1,
     145,   152,    -1,   156,    -1,   155,    77,   156,    -1,   160,
      -1,    58,    75,   158,    76,    -1,    58,   103,    75,   158,
      76,    -1,    58,    75,   158,    77,    76,    -1,    58,   103,
      75,   158,    77,    76,    -1,    58,    75,    76,    -1,    58,
     103,    75,    76,    -1,    58,    75,    77,    76,    -1,    58,
     103,    75,    77,    76,    -1,    58,   103,    -1,   159,    -1,
     158,    77,   159,    -1,   101,    -1,   101,    96,   138,    -1,
     162,   161,    -1,   161,    -1,   101,    -1,    73,   160,    74,
      -1,   161,    79,   138,    80,    -1,   161,    79,    80,    -1,
     161,    73,   163,    74,   153,    -1,   161,    73,    74,   153,
      -1,    84,    -1,    84,   152,    -1,    84,   162,    -1,    84,
     152,   162,    -1,   164,    -1,   164,    77,    59,    -1,   165,
      -1,   164,    77,   165,    -1,   140,   154,   160,    -1,   140,
     154,   166,    -1,   140,   154,    -1,   162,    -1,   167,    -1,
     162,   167,    -1,    73,   166,    74,    -1,    79,    80,    -1,
      79,   138,    80,    -1,   167,    79,    80,    -1,   167,    79,
     138,    80,    -1,    73,    74,   153,    -1,   167,    73,    74,
     153,    -1,   167,    73,   163,    74,   153,    -1,   169,    -1,
     169,   180,    -1,   170,    -1,   152,   170,    -1,   170,   152,
      -1,   144,    -1,   101,    72,   144,    -1,    72,   101,    -1,
     101,    72,   101,    -1,   171,    -1,   172,    -1,   147,   103,
      -1,   147,   101,    72,   103,    -1,    58,   103,    -1,    58,
     101,    72,   103,    -1,   174,    -1,   174,    77,    59,    -1,
     175,    -1,   174,    77,   175,    -1,   176,   182,    -1,   176,
     180,    -1,   176,    -1,   177,    -1,   177,   152,    -1,   152,
     177,    -1,   144,    -1,   101,    -1,   178,    -1,   179,    -1,
     147,   103,    -1,    58,   103,    -1,   162,    -1,   181,    -1,
     162,   181,    -1,    73,   180,    74,    -1,    79,    80,    -1,
      79,   138,    80,    -1,   181,    79,    80,    -1,   181,    79,
     138,    80,    -1,    73,    74,   153,    -1,   181,    73,    74,
     153,    -1,   181,    73,   173,    74,   153,    -1,   162,   183,
      -1,   183,    -1,   101,    -1,    73,   182,    74,    -1,   183,
      79,   138,    80,    -1,   183,    79,    80,    -1,   183,    73,
     173,    74,    -1,   183,    73,    74,    -1,   188,    -1,   190,
      -1,   191,    -1,   194,    -1,   195,    -1,   197,    -1,   192,
      -1,   198,    -1,   185,    -1,    40,   130,    97,    -1,    41,
     130,    97,    -1,   186,    -1,   187,   186,    -1,    75,    76,
      -1,    75,   187,    76,    -1,    75,   189,    76,    -1,    75,
     187,   189,    76,    -1,   184,    -1,   189,   184,    -1,    97,
      -1,   137,    97,    -1,     1,    97,    -1,    61,    73,   137,
      74,   184,    -1,    61,    73,   137,    74,   184,    62,   184,
      -1,    63,    73,   137,    74,   188,    -1,   101,    78,   184,
      -1,    70,   137,    78,   184,    -1,    71,    78,   184,    -1,
     137,    -1,    -1,    64,    73,   137,    74,   184,    -1,    65,
     184,    64,    73,   137,    74,    97,    -1,    66,    73,   193,
      97,   193,    97,   193,    74,   184,    -1,    39,   101,    97,
      -1,    67,    97,    -1,    68,    97,    -1,    69,    97,    -1,
      69,   137,    97,    -1,    12,    -1,   196,   101,    73,   131,
      74,   188,    -1,   196,   101,    73,    74,   188,    -1,   196,
     101,    73,   131,    74,    79,   137,    80,   188,    -1,   196,
     101,    73,    74,    79,   137,    80,   188,    -1,    60,   101,
      96,   137,    97,    -1,   200,    -1,    -1,   201,    -1,   200,
     201,    -1,   184,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,    88,    88,    92,    96,    99,   103,   105,   110,   112,
     114,   119,   124,   134,   136,   141,   142,   143,   145,   147,
     149,   151,   153,   155,   157,   159,   161,   163,   164,   165,
     169,   170,   172,   174,   177,   179,   181,   183,   185,   190,
     192,   197,   201,   202,   204,   206,   208,   210,   212,   214,
     216,   221,   223,   225,   227,   229,   231,   236,   237,   239,
     244,   245,   247,   249,   254,   255,   257,   262,   263,   265,
     270,   271,   273,   275,   277,   282,   283,   285,   290,   291,
     296,   297,   302,   303,   308,   309,   314,   315,   320,   321,
     326,   327,   338,   345,   354,   356,   358,   360,   362,   364,
     366,   368,   370,   372,   374,   379,   381,   386,   388,   396,
     398,   403,   404,   405,   410,   411,   413,   415,   417,   422,
     424,   429,   430,   434,   435,   440,   444,   449,   452,   456,
     458,   460,   465,   467,   472,   474,   476,   478,   480,   482,
     484,   486,   488,   490,   494,   496,   501,   503,   505,   506,
     510,   512,   514,   516,   518,   520,   522,   527,   529,   534,
     536,   541,   543,   545,   547,   550,   552,   557,   564,   565,
     569,   570,   574,   575,   579,   580,   582,   587,   589,   594,
     598,   600,   602,   604,   606,   608,   610,   612,   614,   619,
     621,   628,   630,   635,   637,   641,   643,   645,   647,   649,
     651,   656,   658,   660,   662,   667,   669,   678,   680,   685,
     687,   689,   694,   695,   696,   701,   703,   705,   707,   709,
     711,   733,   735,   740,   742,   747,   748,   750,   755,   757,
     759,   761,   763,   764,   768,   770,   775,   777,   782,   784,
     793,   795,   800,   802,   804,   809,   810,   812,   817,   819,
     821,   822,   826,   831,   836,   837,   838,   843,   845,   847,
     849,   851,   853,   855,   857,   862,   864,   868,   870,   872,
     874,   876,   878,   883,   884,   885,   886,   887,   888,   889,
     890,   891,   895,   900,   905,   907,   912,   914,   916,   923,
     933,   935,   940,   942,   944,   949,   951,   953,   959,   961,
     963,   968,   970,   974,   976,   978,   983,   985,   987,   989,
     991,   996,  1000,  1002,  1004,  1006,  1011,  1016,  1017,  1021,
    1022,  1026
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENTIFIER", "SYMBOL", "CONSTANT",
  "STRING_LITERAL", "CONST", "VOLATILE", "SIZEOF", "TYPEOF", "TYPEDEF",
  "DEFINE", "DEFLOCAL", "DEFREC", "CONTAINEROF", "PTR_OP", "INC_OP",
  "DEC_OP", "LEFT_OP", "RIGHT_OP", "LE_OP", "GE_OP", "EQ_OP", "NE_OP",
  "AND_OP", "OR_OP", "MUL_ASSIGN", "DIV_ASSIGN", "MOD_ASSIGN",
  "ADD_ASSIGN", "SUB_ASSIGN", "LEFT_ASSIGN", "RIGHT_ASSIGN", "AND_ASSIGN",
  "XOR_ASSIGN", "OR_ASSIGN", "CAST_ASSIGN", "XCAST_ASSIGN", "GOTO",
  "GLOBAL", "LOCAL", "LAMBDA", "NAMES", "LET", "LAPPLY", "BOOL", "CHAR",
  "SHORT", "INT", "LONG", "SIGNED", "UNSIGNED", "FLOAT", "DOUBLE", "VOID",
  "STRUCT", "UNION", "ENUM", "ELLIPSIS", "DEFCONST", "IF", "ELSE",
  "SWITCH", "WHILE", "DO", "FOR", "CONTINUE", "BREAK", "RETURN", "CASE",
  "DEFAULT", "'`'", "'('", "')'", "'{'", "'}'", "','", "':'", "'['", "']'",
  "'.'", "'#'", "'&'", "'*'", "'+'", "'-'", "'~'", "'!'", "'/'", "'%'",
  "'<'", "'>'", "'^'", "'|'", "'?'", "'='", "';'", "'$'", "'@'", "$accept",
  "id", "tickid", "tag", "lambda", "lambda_expression",
  "defrec_expression", "let_expression", "table_init", "table_init_list",
  "primary_expression", "postfix_expression", "argument_expression_list",
  "typeof", "unary_expression", "unary_operator", "cast_expression",
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
  "struct_declaration_list", "struct_declaration", "struct_size",
  "type_qualifier", "type_qualifier_list", "maybe_type_qualifier",
  "specifier_list", "struct_declarator_list", "struct_declarator",
  "enum_specifier", "enumerator_list", "enumerator", "declarator",
  "direct_declarator", "pointer", "parameter_type_list", "parameter_list",
  "parameter_declaration", "abstract_declarator",
  "direct_abstract_declarator", "type_name", "tn_type_qual_specifier",
  "tn_type_specifier", "tn_struct_or_union_specifier", "tn_enum_specifier",
  "tn_parameter_type_list", "tn_parameter_list",
  "tn_parameter_declaration", "tn_param_type_qual_specifier",
  "tn_param_type_specifier", "tn_param_struct_or_union_specifier",
  "tn_param_enum_specifier", "tn_abstract_declarator",
  "tn_direct_abstract_declarator", "tn_declarator", "tn_direct_declarator",
  "statement", "global_statement", "local", "local_list",
  "compound_statement", "statement_list", "expression_statement",
  "selection_statement", "labeled_statement", "maybe_expression",
  "iteration_statement", "jump_statement", "define", "define_statement",
  "defconst_statement", "translation_unit_seq", "translation_unit",
  "external_declaration", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   100,   101,   102,   103,   104,   105,   105,   106,   106,
     106,   107,   108,   109,   109,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     111,   111,   111,   111,   111,   111,   111,   111,   111,   112,
     112,   113,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   115,   115,   115,   115,   115,   115,   116,   116,   116,
     117,   117,   117,   117,   118,   118,   118,   119,   119,   119,
     120,   120,   120,   120,   120,   121,   121,   121,   122,   122,
     123,   123,   124,   124,   125,   125,   126,   126,   127,   127,
     128,   128,   128,   128,   129,   129,   129,   129,   129,   129,
     129,   129,   129,   129,   129,   130,   130,   131,   131,   132,
     132,   133,   133,   133,   134,   134,   134,   134,   134,   135,
     135,   136,   136,   137,   137,   138,   139,   140,   140,   141,
     141,   141,   142,   142,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   144,   144,   145,   145,   145,   145,
     146,   146,   146,   146,   146,   146,   146,   147,   147,   148,
     148,   149,   149,   149,   149,   149,   149,   150,   151,   151,
     152,   152,   153,   153,   154,   154,   154,   155,   155,   156,
     157,   157,   157,   157,   157,   157,   157,   157,   157,   158,
     158,   159,   159,   160,   160,   161,   161,   161,   161,   161,
     161,   162,   162,   162,   162,   163,   163,   164,   164,   165,
     165,   165,   166,   166,   166,   167,   167,   167,   167,   167,
     167,   167,   167,   168,   168,   169,   169,   169,   170,   170,
     170,   170,   170,   170,   171,   171,   172,   172,   173,   173,
     174,   174,   175,   175,   175,   176,   176,   176,   177,   177,
     177,   177,   178,   179,   180,   180,   180,   181,   181,   181,
     181,   181,   181,   181,   181,   182,   182,   183,   183,   183,
     183,   183,   183,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   185,   186,   187,   187,   188,   188,   188,   188,
     189,   189,   190,   190,   190,   191,   191,   191,   192,   192,
     192,   193,   193,   194,   194,   194,   195,   195,   195,   195,
     195,   196,   197,   197,   197,   197,   198,   199,   199,   200,
     200,   201
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     3,     1,     1,     5,     4,     5,     6,
       4,     2,     3,     1,     3,     1,     1,     1,     1,     1,
       3,     2,     3,     4,     3,     3,     4,     1,     1,     1,
       1,     4,     3,     4,     3,     3,     2,     2,     8,     1,
       3,     1,     1,     2,     2,     2,     2,     4,     2,     4,
       3,     1,     1,     1,     1,     1,     1,     1,     4,     4,
       1,     3,     3,     3,     1,     3,     3,     1,     3,     3,
       1,     3,     3,     3,     3,     1,     3,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     5,
       1,     3,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     1,     2,     1,
       2,     1,     1,     2,     1,     5,     4,     7,     6,     6,
       4,     1,     1,     1,     3,     1,     4,     2,     0,     2,
       5,     3,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     1,     1,     1,
       6,     5,     5,     4,     4,     3,     2,     1,     1,     1,
       2,     5,     8,     4,     3,     2,     2,     3,     1,     1,
       1,     2,     1,     0,     1,     2,     2,     1,     3,     1,
       4,     5,     5,     6,     3,     4,     4,     5,     2,     1,
       3,     1,     3,     2,     1,     1,     3,     4,     3,     5,
       4,     1,     2,     2,     3,     1,     3,     1,     3,     3,
       3,     2,     1,     1,     2,     3,     2,     3,     3,     4,
       3,     4,     5,     1,     2,     1,     2,     2,     1,     3,
       2,     3,     1,     1,     2,     4,     2,     4,     1,     3,
       1,     3,     2,     2,     1,     1,     2,     2,     1,     1,
       1,     1,     2,     2,     1,     1,     2,     3,     2,     3,
       3,     4,     3,     4,     5,     2,     1,     1,     3,     4,
       3,     4,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     1,     2,     2,     3,     3,     4,
       1,     2,     1,     2,     2,     5,     7,     5,     3,     4,
       3,     1,     0,     5,     7,     9,     3,     2,     2,     2,
       3,     1,     6,     5,     9,     8,     5,     1,     0,     1,
       2,     1
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0
};

/* YYMERGER[RULE-NUM] -- Index of merging function for rule #RULE-NUM.  */
static const unsigned char yymerger[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     1,     1,     1,     1,
       0,     0,     0,     0,     0,     0,     0,     2,     2,     0,
       3,     3,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     4,     4,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error.  */
static const unsigned short int yydefact[] =
{
       0,     0,     2,    17,    18,    19,     0,    41,   311,     0,
       0,     0,     0,     0,     0,     5,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    51,    52,    53,    54,    55,    56,
     292,    15,    16,     0,    27,    28,    29,    30,    42,     0,
      57,     0,    60,    64,    67,    70,    75,    78,    80,    82,
      84,    86,    88,    90,   114,   121,   122,   123,     0,   321,
     281,   273,   274,   275,   279,   276,   277,     0,   278,   280,
       0,     0,   319,   294,     0,    15,    46,     0,     0,     0,
      43,    44,     0,   105,     0,     0,     0,     0,    11,     0,
       0,     0,     0,     0,     0,   302,   307,   308,   309,     0,
       0,     0,   168,   169,   135,   136,   137,   138,   139,   142,
     143,   140,   141,   134,   157,   158,     0,     0,    15,     0,
     144,   228,     0,   170,     0,     0,   223,   225,   232,   233,
       0,   286,     0,   290,   284,     0,     0,     0,    21,    13,
       0,     0,    39,     0,     0,     0,     0,     0,     0,    36,
      37,     0,     0,     0,     0,    48,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,     0,     0,    94,     0,
      57,    45,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   293,     0,     1,   320,     0,     0,     0,   306,
       0,   282,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   301,     0,   310,     0,   300,     4,   236,   230,     0,
      20,   145,     4,   234,   171,   226,     0,     0,     0,   201,
     254,   224,   255,   227,     0,     0,   287,   285,     0,   288,
     291,    24,     0,    25,     0,    22,     0,     0,    50,     3,
     298,     0,   107,     0,    35,    32,     0,    39,     0,    34,
       0,    91,    92,    93,    61,    62,    63,    65,    66,    68,
      69,    73,    74,    71,    72,    76,    77,    79,    81,    83,
      85,    87,     0,   124,     0,    47,    10,     0,     0,   106,
       0,     0,     0,   116,     0,   147,     0,   109,   112,   111,
     146,   174,   148,     0,     0,     0,   149,     0,   120,     0,
       0,     0,     0,     0,     0,   302,   299,     0,     3,   229,
       0,    58,   173,     0,   258,   125,     0,   202,   203,   256,
       0,     0,   283,    59,   289,    26,    14,     0,    23,    40,
      12,   231,     7,   108,     0,    33,     0,    31,    49,     0,
       0,     0,     8,     0,     0,   113,     0,     0,     4,   188,
       0,   115,   110,   176,     0,   156,   175,     0,   129,   195,
       0,   132,   194,     0,     0,     0,   316,   295,   297,   303,
       0,     0,   237,   235,   172,   262,   257,   259,   204,     0,
     173,   249,   248,     0,     0,     0,   238,   240,   244,   245,
     250,   251,   260,     0,     6,    89,     0,   313,     0,     9,
       0,     0,   184,     0,   191,     0,   189,     0,     0,     0,
     155,     0,   147,     0,   159,     0,     0,     0,     0,     0,
     131,   128,     0,   193,   118,     0,   119,     0,     0,   302,
     253,   263,   252,   247,   173,     0,     0,   267,   254,   243,
     242,   266,   246,   261,     0,     0,   312,     0,   126,   186,
       0,   180,     0,   185,     0,     0,     0,   166,     0,     0,
     165,   160,     0,   153,     0,   177,   179,   154,     0,     0,
     196,   133,   173,     0,     0,     0,   205,   207,   198,     0,
     117,   296,   304,     0,   264,   239,   241,     0,   265,     0,
       0,     0,     0,    38,   192,   182,   190,   187,   181,     0,
     130,     0,   167,     0,   151,     0,   164,     0,   152,   200,
     127,   211,   173,   128,   197,     0,   268,   272,     0,   270,
       0,   315,     0,   183,     0,   163,     0,   178,   150,     0,
       0,   209,   212,   210,   213,   199,   206,   208,   305,   271,
     269,   314,     0,   161,   173,     0,   216,     0,   214,   128,
       0,     0,   220,   215,   217,   173,     0,   218,     0,     0,
     221,   173,   219,   162,   222
};

/* YYPDEFGOTO[NTERM-NUM].  */
static const short int yydefgoto[] =
{
      -1,    85,    42,   227,    43,    44,    45,    46,   149,   150,
      47,    48,   151,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,   179,
     262,   263,   306,   307,    65,    66,    67,    68,   336,   308,
     494,   309,   380,   130,   310,   311,   312,   313,   433,   434,
     435,   133,   314,   395,   315,   484,   485,   316,   425,   426,
     486,   382,   383,   495,   496,   497,   553,   554,   135,   136,
     137,   138,   139,   405,   406,   407,   408,   409,   410,   411,
     333,   242,   460,   461,   143,    70,   144,   145,    71,   146,
      72,    73,    74,   222,    75,    76,    77,    78,    79,    80,
      81,    82
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -442
static const short int yypact[] =
{
     918,   -42,  -442,  -442,  -442,  -442,  2253,  -442,  -442,    85,
      91,  2339,  2339,    85,    85,  -442,  1845,   121,   148,    85,
     158,   167,   196,  1452,   210,   165,   209,  1538,  1845,   215,
    1624,  1096,  1641,  2412,  -442,  -442,  -442,  -442,  -442,  -442,
    -442,    96,  -442,   246,  -442,  -442,  -442,  -442,   283,  2355,
     753,  2237,  -442,   270,   -53,    60,   190,   281,   238,   239,
     233,   311,   119,  -442,  -442,  -442,  -442,  -442,   -41,  -442,
    -442,  -442,  -442,  -442,  -442,  -442,  -442,    85,  -442,  -442,
     340,  1007,  -442,  -442,  1624,   286,  -442,   275,  1845,  1845,
    -442,  -442,   280,  -442,   -26,  1845,   -35,  1096,  -442,  1845,
     290,  1845,  1845,  1845,   338,  1845,  -442,  -442,  -442,   141,
     361,  1452,  -442,  -442,  -442,  -442,  -442,  -442,  -442,  -442,
    -442,  -442,  -442,  -442,  -442,  -442,    85,    85,   325,   -36,
    -442,   808,    85,   467,  1076,   334,   294,   467,  -442,  -442,
      85,  -442,   125,  -442,  -442,  1185,  1274,   332,  -442,  -442,
     295,   324,   336,   348,   350,    85,  1452,    80,    85,  -442,
    -442,  1727,  1845,    85,  1624,  -442,  -442,  -442,  -442,  -442,
    -442,  -442,  -442,  -442,  -442,  -442,  2412,  1845,  -442,  1845,
    -442,  -442,  2237,  2237,  2237,  2237,  2237,  2237,  2237,  2237,
    2237,  2237,  2237,  2237,  2237,  2237,  2237,  2237,  2237,  2237,
    1845,  1845,  -442,   349,  -442,  -442,   360,    20,   353,  -442,
      85,  -442,   401,   678,  1845,   333,  1845,   347,   352,   372,
     378,   380,   395,  -442,  1452,  -442,   416,  -442,  -442,   668,
    -442,  -442,   425,  -442,  -442,  -442,  2237,   297,  1931,    21,
     188,  -442,   278,  -442,   156,  2237,  -442,  -442,  1363,  -442,
    -442,  -442,  1743,  -442,  1829,  -442,  1845,   668,  -442,  -442,
    -442,   121,   193,   427,  -442,  -442,   373,  -442,   379,  -442,
     429,  -442,  -442,  -442,  -442,  -442,  -442,   270,   270,   -53,
     -53,    60,    60,    60,    60,   190,   190,   281,   238,   239,
     233,   311,   404,  -442,   172,  -442,  -442,   409,  1624,  -442,
     410,  2468,    14,  -442,  2237,  -442,   694,  -442,  -442,  -442,
     808,   467,  -442,    24,  1165,    11,  -442,    88,  -442,  1845,
     166,  1452,   121,  1452,  1845,  1845,  -442,    85,   234,   808,
      85,  -442,   467,   434,  -442,  -442,   432,   435,  -442,   278,
    1521,  1947,  -442,  -442,  -442,  -442,  -442,   336,  -442,  -442,
    -442,  -442,  -442,  -442,   121,  -442,  1845,  -442,  -442,  2237,
       2,   444,  -442,    41,   443,  -442,    18,    73,  -442,   446,
    2468,  -442,  -442,  -442,   826,   449,  -442,    18,  -442,  -442,
     180,  -442,   301,    67,   754,   384,  -442,   465,  -442,  -442,
     391,   431,  -442,  -442,  -442,  -442,  -442,  -442,  -442,    85,
     467,  -442,   808,    85,  1254,   457,   455,  -442,    27,   467,
    -442,  -442,  -442,   454,  -442,  -442,  1845,  -442,   149,  -442,
      85,   440,  -442,   463,   445,   414,  -442,   129,    18,   448,
    -442,    43,   462,   897,  -442,   470,    18,   839,   475,    18,
    -442,   -49,  2033,   301,  -442,   768,  -442,  1452,   453,  1845,
    -442,  -442,  -442,  -442,   467,  1551,    19,  -442,    34,  -442,
    -442,   316,  -442,  -442,   390,  1845,  -442,   477,  -442,  -442,
    2237,  -442,    62,  -442,   478,   419,   237,  -442,  2237,   987,
    -442,  -442,   479,  -442,   248,  -442,  -442,  -442,   897,   480,
    -442,  -442,   467,  2237,  2468,   484,   483,  -442,  -442,   481,
    -442,  -442,  -442,   488,  -442,  -442,  -442,   494,   316,  2398,
    2049,   121,   396,  -442,  -442,  -442,  -442,  -442,  -442,    65,
    -442,  2468,  -442,    13,  -442,    18,  -442,   493,  -442,  -442,
    -442,    36,   467,   -24,  -442,  1452,  -442,  -442,   496,  -442,
     491,  -442,   121,  -442,    18,  -442,   251,  -442,  -442,    87,
    2135,  -442,    64,  -442,   320,  -442,  -442,  -442,  -442,  -442,
    -442,  -442,   495,  -442,   467,   500,  -442,   497,   320,   -40,
    2151,  2237,  -442,  -442,  -442,   467,   505,  -442,   501,   485,
    -442,   467,  -442,  -442,  -442
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -442,   139,  -442,   -38,  -442,  -442,  -442,  -442,   331,  -442,
    -442,  -442,  -152,  -442,    -6,  -442,    15,   314,   323,   227,
     321,   392,   389,   393,   394,   397,  -442,   -87,  -442,  -442,
      -7,   299,   204,  -286,  -442,  -442,   356,   228,   189,  -442,
    -442,  -442,   161,  -116,   -20,   277,  -442,    39,   162,  -414,
    -146,  -442,    71,  -166,   -94,    75,  -315,  -442,   173,  -441,
    -304,  -357,  -118,    32,  -442,    69,    55,    53,   -30,  -442,
     472,  -442,  -442,    98,  -442,   154,  -442,   211,  -442,  -442,
    -134,  -232,   157,   159,     1,  -442,   469,  -442,   -13,   471,
    -442,  -442,  -442,  -313,  -442,  -442,  -442,  -442,  -442,  -442,
    -442,   538
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -319
static const short int yytable[] =
{
      86,    69,   241,   154,    98,    90,    91,    94,   339,   266,
     131,   381,   391,   131,     2,   231,     2,     2,   240,   481,
     372,     2,     2,     2,   104,   492,   443,     2,   112,   113,
       2,   516,   185,   186,   575,   556,   201,     2,   230,     2,
     213,   201,   201,   165,     2,   180,     2,     3,     4,     5,
     493,   210,     6,     7,   206,    83,   202,     9,    10,   493,
      11,    12,   421,   214,   131,     2,   181,     2,     2,   132,
       2,   211,   132,   438,   481,   493,     2,    97,   516,   187,
     188,   416,    69,     2,   377,    15,   377,    17,     2,   367,
       2,   377,   456,   332,   233,   239,   296,   239,   238,   374,
     456,   134,   239,   239,   134,   239,   238,   456,   378,   549,
     545,   239,   225,   238,   131,   550,    30,   419,    95,   240,
     239,   338,    32,   132,   381,    33,    34,    35,    36,    37,
      38,    39,     2,   244,   270,   491,   503,   549,   515,    41,
     377,   543,   478,   550,   131,   199,   271,   250,    87,   422,
     423,   335,    92,    93,   261,   134,   131,   260,   100,   372,
     549,   564,    41,   384,    88,   201,   550,   385,   155,   128,
      41,   239,   153,   132,   156,     2,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   231,   443,    97,   274,   275,   276,
     297,   245,   201,   132,   234,   473,   474,   366,   243,   329,
     547,   189,   190,   231,   200,   132,   203,   335,   201,   398,
      41,    99,   202,   128,    97,   326,   339,   551,   465,   562,
     180,   101,   180,   210,   451,   134,    41,   329,   223,   180,
     102,  -231,  -231,   201,    96,   438,   360,   134,   352,   250,
      41,   331,   353,   342,   335,   109,   110,   439,   129,   142,
     343,   237,   106,   386,   369,   226,   228,   238,   364,   103,
     210,   232,   415,   153,   459,   375,   428,   440,   131,    93,
     436,   191,   192,   105,    41,    41,   231,   482,   504,   392,
     458,   489,   393,   111,   259,    41,    93,   264,   180,   158,
     159,   160,   269,   128,   193,   194,   107,    -3,    -3,   388,
     337,    -3,   129,    -3,   439,   153,   208,   129,    -3,   157,
     402,   195,   387,   212,   389,   525,   529,   197,   525,   217,
     218,   219,   196,   221,   520,   180,   198,   132,   458,   436,
     204,   414,   527,   436,   335,   526,    93,   417,   563,   299,
     207,   340,   305,   180,   182,   335,   161,   341,   155,   183,
     184,   450,   162,    41,   163,   452,   555,   237,   328,   134,
     237,   332,   252,   238,   441,   253,   238,   209,   239,   403,
     442,   239,   373,   335,   402,   523,   216,    41,   152,   509,
     268,   335,   129,   569,   436,   510,   351,   229,   572,   570,
     531,   254,   220,   394,   255,   466,   335,   318,   236,   580,
     319,   404,   251,   552,   256,   584,   281,   282,   283,   284,
     257,   321,   294,   335,   201,   180,   322,   544,   292,   201,
     298,   552,   258,    93,   295,   402,   180,   128,   201,   224,
     305,   368,   317,   403,   320,   305,   323,   355,   501,   201,
     356,   324,   368,   305,   379,   215,   201,   201,   446,   357,
      41,   356,    41,   335,   180,   448,   368,   201,   201,   368,
     511,   394,   180,   201,   112,   113,   542,   245,   201,   401,
     462,   201,   359,   335,   335,   362,   363,   180,   327,   402,
     471,   472,   325,   370,   403,   518,   519,   330,   541,   277,
     278,   354,   299,   358,   180,   379,   424,   365,   396,   305,
     279,   280,   397,   432,   285,   286,   379,   267,   418,   239,
     420,   427,   379,   305,   437,   394,   404,   447,   449,   561,
     413,   454,   455,   272,   463,   273,   558,   468,   368,   469,
     480,   470,   368,   401,   180,   477,   483,   457,   403,   490,
     502,   513,   390,   221,   517,   524,   528,   293,   532,   467,
     533,   534,   535,   394,   180,   180,   424,   379,   536,   548,
     559,   560,   432,   571,   573,   379,   432,   574,   379,   581,
     404,   582,   583,   346,   305,   288,    41,   287,   445,   476,
     289,   376,   290,   361,   401,   457,   291,   457,   546,   488,
     475,   576,   557,   394,   565,   568,   235,   538,   347,   506,
     349,   424,   350,   507,   247,   453,   248,   508,   305,   205,
     479,     0,     0,     0,     0,     0,     0,   432,     0,     0,
       0,   499,     0,   305,     0,   394,     0,     0,     0,     0,
       0,     0,     0,     0,   464,     0,   394,     0,   401,     0,
       0,     0,   394,     0,   293,     0,     0,     0,   424,   514,
     305,     0,   379,     0,   379,     0,     0,   521,     0,     0,
     379,     2,     0,     0,    41,   267,     0,   221,     0,   300,
       0,     2,   530,   379,     0,   112,   113,     0,   379,   301,
       0,   379,     0,   512,     0,   300,     0,     2,     0,   540,
       0,   112,   113,     0,     0,   301,     0,     0,     0,     0,
       0,     0,   349,     0,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   302,     0,     0,   567,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   302,     0,   303,   300,     0,     2,     0,   578,
     579,   112,   113,     0,     0,   301,     0,     0,     0,   300,
     371,     2,     0,     0,     0,   112,   113,   304,     0,   301,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,     0,   304,     0,     0,     0,     0,     0,     0,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   302,     0,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   302,   429,     0,     2,
     444,     0,     0,   112,   113,     0,     0,     0,     0,     0,
     429,     0,     2,     0,   500,     0,   112,   113,     0,   178,
       0,     0,     0,   304,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,     0,     0,     0,   304,     0,     0,
       0,     0,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   302,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   302,   429,     0,
       2,     0,   430,     0,   112,   113,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   487,     0,     0,  -318,     1,
       0,     2,     3,     4,     5,   431,     0,     6,     7,     0,
       8,     0,     9,    10,     0,    11,    12,     0,   431,     0,
       0,     0,     0,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   302,     0,    13,    14,     0,
      15,    16,    17,    18,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    19,    20,
       0,    21,    22,    23,    24,    25,    26,    27,    28,    29,
       2,    30,     0,    31,   112,   113,   431,    32,     0,     0,
      33,    34,    35,    36,    37,    38,    39,  -317,     1,     0,
       2,     3,     4,     5,     0,    40,     6,     7,     0,     8,
       0,     9,    10,     0,    11,    12,     0,     0,     0,     0,
       0,     0,     0,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   302,    13,    14,     0,    15,
      16,    17,    18,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    19,    20,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    29,     2,
      30,     0,    31,     0,   522,     0,    32,     0,     0,    33,
      34,    35,    36,    37,    38,    39,     0,     1,     0,     2,
       3,     4,     5,     0,    40,     6,     7,     0,     8,     0,
       9,    10,     0,    11,    12,     0,     0,     0,     0,     0,
       0,     0,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,    13,    14,   140,    15,    16,
      17,    18,     0,     0,     0,     0,     0,     0,   127,     0,
       0,     0,     0,     0,     0,     0,    19,    20,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,     2,    30,
       0,    31,   141,     0,     0,    32,     0,     0,    33,    34,
      35,    36,    37,    38,    39,     0,     1,     0,     2,     3,
       4,     5,     0,    40,     6,     7,     0,     8,     0,     9,
      10,     0,    11,    12,     0,     0,     0,     0,     0,     0,
       0,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   302,    13,    14,   140,    15,    16,    17,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    19,    20,     0,    21,    22,
      23,    24,    25,    26,    27,    28,    29,     2,    30,     0,
      31,   246,     0,     0,    32,     0,     0,    33,    34,    35,
      36,    37,    38,    39,     0,     1,     0,     2,     3,     4,
       5,     0,    40,     6,     7,     0,     8,     0,     9,    10,
       0,    11,    12,     0,     0,     0,     0,     0,     0,     0,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   399,    13,    14,     0,    15,    16,    17,    18,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    19,    20,     0,    21,    22,    23,
      24,    25,    26,    27,    28,    29,     0,    30,     0,    31,
     249,     0,     0,    32,     0,     0,    33,    34,    35,    36,
      37,    38,    39,     0,     1,     0,     2,     3,     4,     5,
       0,    40,     6,     7,     0,     8,     0,     9,    10,     0,
      11,    12,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    13,    14,     0,    15,    16,    17,    18,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    19,    20,     0,    21,    22,    23,    24,
      25,    26,    27,    28,    29,     0,    30,     0,    31,   344,
       0,     0,    32,     0,     0,    33,    34,    35,    36,    37,
      38,    39,     0,     1,     0,     2,     3,     4,     5,     0,
      40,     6,     7,     0,     8,     0,     9,    10,     0,    11,
      12,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    13,    14,     0,    15,    16,    17,    18,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    19,    20,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,     2,    30,     0,    31,   112,   113,
       0,    32,     0,     0,    33,    34,    35,    36,    37,    38,
      39,     2,     3,     4,     5,     0,     0,     6,     7,    40,
       0,     0,     9,    10,     2,    11,    12,     0,   112,   113,
       0,     0,     0,     0,     0,     0,     0,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   399,
      15,    16,    17,    18,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   400,     0,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   399,
     505,    30,     0,    95,     0,     0,     0,    32,     0,     0,
      33,    34,    35,    36,    37,    38,    39,     2,     3,     4,
       5,   112,   113,     6,     7,   108,     0,     0,     9,    10,
       0,    11,    12,     0,     2,     3,     4,     5,     0,     0,
       6,     7,     0,     0,     0,     9,    10,     0,    11,    12,
       0,     0,     0,     0,     0,     0,    15,    16,    17,    18,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,    15,    16,    17,    18,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   127,    30,     0,    95,
       0,     0,     0,    32,     0,     0,    33,    34,    35,    36,
      37,    38,    39,     0,    30,     0,    95,     0,     0,   147,
      32,   148,     0,    33,    34,    35,    36,    37,    38,    39,
       2,     3,     4,     5,     0,     0,     6,     7,     0,     0,
       0,     9,    10,     0,    11,    12,     2,     3,     4,     5,
       0,     0,     6,     7,     0,     0,     0,     9,    10,     0,
      11,    12,     0,     0,     0,     0,     0,     0,     0,    15,
      16,    17,    18,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    15,    16,    17,    18,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,   265,    95,     0,     0,     0,    32,     0,     0,    33,
      34,    35,    36,    37,    38,    39,    30,     0,    95,     0,
       0,     0,    32,   345,     0,    33,    34,    35,    36,    37,
      38,    39,     2,     3,     4,     5,     0,     0,     6,     7,
       0,     0,     0,     9,    10,     0,    11,    12,     2,     3,
       4,     5,     0,     0,     6,     7,     0,     0,     0,     9,
      10,     0,    11,    12,     0,     0,     0,     0,     0,     0,
       0,    15,    16,    17,    18,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    15,    16,    17,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    30,     0,    95,     0,     0,     0,    32,   348,
       0,    33,    34,    35,    36,    37,    38,    39,    30,     0,
      95,     0,     0,     0,    32,     0,     0,    33,    34,    35,
      36,    37,    38,    39,     2,     3,     4,     5,     0,     0,
       6,     7,     0,     0,     0,     9,    10,     0,    11,    12,
       2,     3,     4,     5,     0,     0,     6,     7,     0,     0,
       0,     9,    10,     0,    11,    12,     0,     0,     0,     0,
       0,     0,     0,    15,     0,    17,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    15,
       0,    17,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    30,     0,    95,     0,     0,     0,
      32,   334,     0,    33,    34,    35,    36,    37,    38,    39,
      30,     0,    95,     0,     0,     0,    32,   412,     0,    33,
      34,    35,    36,    37,    38,    39,     2,     3,     4,     5,
       0,     0,     6,     7,     0,     0,     0,     9,    10,     0,
      11,    12,     2,     3,     4,     5,     0,     0,     6,     7,
       0,     0,     0,     9,    10,     0,    11,    12,     0,     0,
       0,     0,     0,     0,     0,    15,     0,    17,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    15,     0,    17,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    30,     0,    95,     0,
       0,     0,    32,   498,     0,    33,    34,    35,    36,    37,
      38,    39,    30,     0,    95,     0,     0,     0,    32,   539,
       0,    33,    34,    35,    36,    37,    38,    39,     2,     3,
       4,     5,     0,     0,     6,     7,     0,     0,     0,     9,
      10,     0,    11,    12,     2,     3,     4,     5,     0,     0,
       6,     7,     0,     0,     0,     9,    10,     0,    11,    12,
       0,     0,     0,     0,     0,     0,     0,    15,     0,    17,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    15,     0,    17,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    30,     0,
      95,     0,     0,     0,    32,   566,     0,    33,    34,    35,
      36,    37,    38,    39,    30,     0,    95,     0,     0,     0,
      32,   577,     0,    33,    34,    35,    36,    37,    38,    39,
       2,     3,     4,     5,     0,     0,     6,     7,     0,     0,
       0,     9,    10,     0,    11,    12,     2,     3,     4,     5,
       0,     0,     6,     7,     0,     0,     0,     9,    10,     0,
      11,    12,     0,     0,     0,     0,     0,     0,     0,    15,
       0,    17,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    15,     0,    17,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,     0,    95,     0,     0,     0,    32,     0,     0,    33,
      34,    35,    36,    37,    38,    39,    84,     0,     0,     0,
       0,     0,    32,     0,     0,    33,    34,    35,    36,    37,
      38,    39,     2,     3,     4,     5,     0,     0,     6,     7,
       0,     0,     0,     9,    10,     0,    11,    12,     2,     3,
       4,     5,     0,     0,     6,     7,     0,     0,     0,     9,
      10,     0,    11,    12,     0,     0,     0,     0,     0,     0,
       0,    15,     0,    17,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    15,     0,    17,
       0,     2,     0,     0,     0,   112,   113,     0,     0,     0,
       0,     0,    89,     0,     0,     2,     0,     0,    32,   112,
     113,    33,    34,    35,    36,    37,    38,    39,   164,     0,
       0,     0,     0,     0,    32,     0,     0,    33,    34,    35,
      36,    37,    38,    39,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   399,     0,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,     2,   537,     0,     0,   112,   113,     0,     0,     0,
       0,     0,     0,     0,   127,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   302
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
       0,     0,     0,     0,     0,     0,     0,     1,     3,     0,
       0,     5,     0,     7,     0,     0,     0,     0,     9,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    11,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
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
       0,   231,     0,   231,     0,   231,     0,   231,     0,   231,
       0,   295,     0
};

static const short int yycheck[] =
{
       6,     0,   136,    33,    17,    11,    12,    14,   240,   161,
      30,   315,   325,    33,     3,   131,     3,     3,   136,   433,
     306,     3,     3,     3,    23,    74,   383,     3,     7,     8,
       3,   472,    85,    86,    74,    59,    77,     3,    74,     3,
      75,    77,    77,    49,     3,    51,     3,     4,     5,     6,
      99,    77,     9,    10,    84,    97,    97,    14,    15,    99,
      17,    18,   366,    98,    84,     3,    51,     3,     3,    30,
       3,    97,    33,   377,   488,    99,     3,    75,   519,    19,
      20,    79,    81,     3,    73,    42,    73,    44,     3,    75,
       3,    73,    73,    74,   132,    84,    76,    84,    79,    75,
      73,    30,    84,    84,    33,    84,    79,    73,    97,    73,
      97,    84,   111,    79,   134,    79,    73,    76,    75,   237,
      84,   239,    79,    84,   428,    82,    83,    84,    85,    86,
      87,    88,     3,   140,   164,   439,   449,    73,    76,     0,
      73,    76,    99,    79,   164,    26,   176,   146,     9,    76,
      77,   238,    13,    14,    74,    84,   176,   156,    19,   445,
      73,    74,    23,    75,    73,    77,    79,   319,    72,    30,
      31,    84,    33,   134,    78,     3,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   310,   552,    75,   182,   183,   184,
     207,    76,    77,   164,   133,    76,    77,   301,   137,   229,
     525,    21,    22,   329,    95,   176,    77,   304,    77,   337,
      81,    73,    97,    84,    75,   224,   458,   531,    79,   544,
     236,    73,   238,    77,   400,   164,    97,   257,    97,   245,
      73,     7,     8,    77,    16,   549,    74,   176,   261,   248,
     111,   236,    59,    97,   341,    27,    28,    77,    30,    31,
     245,    73,    97,    97,   302,   126,   127,    79,   298,    73,
      77,   132,   359,   134,   408,   313,   370,    97,   298,   140,
     374,    91,    92,    73,   145,   146,   402,   433,   454,   327,
     408,   437,   330,    78,   155,   156,   157,   158,   304,    16,
      17,    18,   163,   164,    23,    24,    97,    73,    74,   322,
     239,    77,    84,    79,    77,   176,    88,    89,    84,    73,
     340,    83,   321,    95,   323,    77,   492,    94,    77,   101,
     102,   103,    93,   105,    97,   341,    25,   298,   456,   433,
       0,   354,   488,   437,   431,    97,   207,   360,    97,   210,
      75,    73,   213,   359,    84,   442,    73,    79,    72,    89,
      90,   399,    79,   224,    81,   403,   532,    73,   229,   298,
      73,    74,    77,    79,    73,    80,    79,    97,    84,   340,
      79,    84,   311,   470,   404,   479,    96,   248,    32,    73,
     162,   478,   164,    73,   488,    79,   257,    72,   564,    79,
     494,    77,    64,   332,    80,   418,   493,    74,    74,   575,
      77,   340,    80,   531,    78,   581,   189,   190,   191,   192,
      72,    74,    73,   510,    77,   431,    74,   521,   200,    77,
      77,   549,    82,   294,    74,   455,   442,   298,    77,    78,
     301,   302,   214,   404,   216,   306,    74,    74,   447,    77,
      77,    73,   313,   314,   315,    99,    77,    77,    74,    80,
     321,    77,   323,   550,   470,    74,   327,    77,    77,   330,
      80,   400,   478,    77,     7,     8,    80,    76,    77,   340,
     409,    77,    78,   570,   571,    76,    77,   493,    72,   509,
      76,    77,    97,   304,   455,    76,    77,    72,   511,   185,
     186,    74,   363,    74,   510,   366,   367,    97,    74,   370,
     187,   188,    80,   374,   193,   194,   377,   161,    74,    84,
      77,    75,   383,   384,    75,   454,   455,    62,    97,   542,
     341,    74,    77,   177,    80,   179,   535,    97,   399,    76,
      78,    96,   403,   404,   550,    97,    76,   408,   509,    74,
      97,    74,   324,   325,    76,    76,    76,   201,    74,   420,
      77,    80,    74,   492,   570,   571,   427,   428,    74,    76,
      74,    80,   433,    78,    74,   436,   437,    80,   439,    74,
     509,    80,    97,   252,   445,   196,   447,   195,   384,   428,
     197,   314,   198,   294,   455,   456,   199,   458,   523,   437,
     427,   569,   533,   532,   549,   552,   134,   509,   252,   455,
     254,   472,   256,   456,   145,   404,   145,   458,   479,    81,
     431,    -1,    -1,    -1,    -1,    -1,    -1,   488,    -1,    -1,
      -1,   442,    -1,   494,    -1,   564,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   416,    -1,   575,    -1,   509,    -1,
      -1,    -1,   581,    -1,   298,    -1,    -1,    -1,   519,   470,
     521,    -1,   523,    -1,   525,    -1,    -1,   478,    -1,    -1,
     531,     3,    -1,    -1,   535,   319,    -1,   449,    -1,     1,
      -1,     3,   493,   544,    -1,     7,     8,    -1,   549,    11,
      -1,   552,    -1,   465,    -1,     1,    -1,     3,    -1,   510,
      -1,     7,     8,    -1,    -1,    11,    -1,    -1,    -1,    -1,
      -1,    -1,   356,    -1,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    -1,   550,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    76,     1,    -1,     3,    -1,   570,
     571,     7,     8,    -1,    -1,    11,    -1,    -1,    -1,     1,
      76,     3,    -1,    -1,    -1,     7,     8,    99,    -1,    11,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    -1,    99,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,     1,    -1,     3,
      76,    -1,    -1,     7,     8,    -1,    -1,    -1,    -1,    -1,
       1,    -1,     3,    -1,    76,    -1,     7,     8,    -1,    96,
      -1,    -1,    -1,    99,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    -1,    -1,    -1,    99,    -1,    -1,
      -1,    -1,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,     1,    -1,
       3,    -1,    76,    -1,     7,     8,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    76,    -1,    -1,     0,     1,
      -1,     3,     4,     5,     6,    99,    -1,     9,    10,    -1,
      12,    -1,    14,    15,    -1,    17,    18,    -1,    99,    -1,
      -1,    -1,    -1,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    -1,    39,    40,    -1,
      42,    43,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    61,
      -1,    63,    64,    65,    66,    67,    68,    69,    70,    71,
       3,    73,    -1,    75,     7,     8,    99,    79,    -1,    -1,
      82,    83,    84,    85,    86,    87,    88,     0,     1,    -1,
       3,     4,     5,     6,    -1,    97,     9,    10,    -1,    12,
      -1,    14,    15,    -1,    17,    18,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    39,    40,    -1,    42,
      43,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    61,    -1,
      63,    64,    65,    66,    67,    68,    69,    70,    71,     3,
      73,    -1,    75,    -1,    97,    -1,    79,    -1,    -1,    82,
      83,    84,    85,    86,    87,    88,    -1,     1,    -1,     3,
       4,     5,     6,    -1,    97,     9,    10,    -1,    12,    -1,
      14,    15,    -1,    17,    18,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    39,    40,    41,    42,    43,
      44,    45,    -1,    -1,    -1,    -1,    -1,    -1,    72,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    60,    61,    -1,    63,
      64,    65,    66,    67,    68,    69,    70,    71,     3,    73,
      -1,    75,    76,    -1,    -1,    79,    -1,    -1,    82,    83,
      84,    85,    86,    87,    88,    -1,     1,    -1,     3,     4,
       5,     6,    -1,    97,     9,    10,    -1,    12,    -1,    14,
      15,    -1,    17,    18,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    39,    40,    41,    42,    43,    44,
      45,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    60,    61,    -1,    63,    64,
      65,    66,    67,    68,    69,    70,    71,     3,    73,    -1,
      75,    76,    -1,    -1,    79,    -1,    -1,    82,    83,    84,
      85,    86,    87,    88,    -1,     1,    -1,     3,     4,     5,
       6,    -1,    97,     9,    10,    -1,    12,    -1,    14,    15,
      -1,    17,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    39,    40,    -1,    42,    43,    44,    45,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    60,    61,    -1,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    -1,    73,    -1,    75,
      76,    -1,    -1,    79,    -1,    -1,    82,    83,    84,    85,
      86,    87,    88,    -1,     1,    -1,     3,     4,     5,     6,
      -1,    97,     9,    10,    -1,    12,    -1,    14,    15,    -1,
      17,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    39,    40,    -1,    42,    43,    44,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    60,    61,    -1,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    -1,    73,    -1,    75,    76,
      -1,    -1,    79,    -1,    -1,    82,    83,    84,    85,    86,
      87,    88,    -1,     1,    -1,     3,     4,     5,     6,    -1,
      97,     9,    10,    -1,    12,    -1,    14,    15,    -1,    17,
      18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    39,    40,    -1,    42,    43,    44,    45,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    60,    61,    -1,    63,    64,    65,    66,    67,
      68,    69,    70,    71,     3,    73,    -1,    75,     7,     8,
      -1,    79,    -1,    -1,    82,    83,    84,    85,    86,    87,
      88,     3,     4,     5,     6,    -1,    -1,     9,    10,    97,
      -1,    -1,    14,    15,     3,    17,    18,    -1,     7,     8,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      42,    43,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    74,    -1,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    73,    -1,    75,    -1,    -1,    -1,    79,    -1,    -1,
      82,    83,    84,    85,    86,    87,    88,     3,     4,     5,
       6,     7,     8,     9,    10,    97,    -1,    -1,    14,    15,
      -1,    17,    18,    -1,     3,     4,     5,     6,    -1,    -1,
       9,    10,    -1,    -1,    -1,    14,    15,    -1,    17,    18,
      -1,    -1,    -1,    -1,    -1,    -1,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    42,    43,    44,    45,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    72,    73,    -1,    75,
      -1,    -1,    -1,    79,    -1,    -1,    82,    83,    84,    85,
      86,    87,    88,    -1,    73,    -1,    75,    -1,    -1,    78,
      79,    80,    -1,    82,    83,    84,    85,    86,    87,    88,
       3,     4,     5,     6,    -1,    -1,     9,    10,    -1,    -1,
      -1,    14,    15,    -1,    17,    18,     3,     4,     5,     6,
      -1,    -1,     9,    10,    -1,    -1,    -1,    14,    15,    -1,
      17,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,
      43,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    42,    43,    44,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      73,    74,    75,    -1,    -1,    -1,    79,    -1,    -1,    82,
      83,    84,    85,    86,    87,    88,    73,    -1,    75,    -1,
      -1,    -1,    79,    80,    -1,    82,    83,    84,    85,    86,
      87,    88,     3,     4,     5,     6,    -1,    -1,     9,    10,
      -1,    -1,    -1,    14,    15,    -1,    17,    18,     3,     4,
       5,     6,    -1,    -1,     9,    10,    -1,    -1,    -1,    14,
      15,    -1,    17,    18,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    42,    43,    44,    45,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    43,    44,
      45,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    73,    -1,    75,    -1,    -1,    -1,    79,    80,
      -1,    82,    83,    84,    85,    86,    87,    88,    73,    -1,
      75,    -1,    -1,    -1,    79,    -1,    -1,    82,    83,    84,
      85,    86,    87,    88,     3,     4,     5,     6,    -1,    -1,
       9,    10,    -1,    -1,    -1,    14,    15,    -1,    17,    18,
       3,     4,     5,     6,    -1,    -1,     9,    10,    -1,    -1,
      -1,    14,    15,    -1,    17,    18,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    42,    -1,    44,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,
      -1,    44,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    73,    -1,    75,    -1,    -1,    -1,
      79,    80,    -1,    82,    83,    84,    85,    86,    87,    88,
      73,    -1,    75,    -1,    -1,    -1,    79,    80,    -1,    82,
      83,    84,    85,    86,    87,    88,     3,     4,     5,     6,
      -1,    -1,     9,    10,    -1,    -1,    -1,    14,    15,    -1,
      17,    18,     3,     4,     5,     6,    -1,    -1,     9,    10,
      -1,    -1,    -1,    14,    15,    -1,    17,    18,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    42,    -1,    44,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    73,    -1,    75,    -1,
      -1,    -1,    79,    80,    -1,    82,    83,    84,    85,    86,
      87,    88,    73,    -1,    75,    -1,    -1,    -1,    79,    80,
      -1,    82,    83,    84,    85,    86,    87,    88,     3,     4,
       5,     6,    -1,    -1,     9,    10,    -1,    -1,    -1,    14,
      15,    -1,    17,    18,     3,     4,     5,     6,    -1,    -1,
       9,    10,    -1,    -1,    -1,    14,    15,    -1,    17,    18,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    44,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    42,    -1,    44,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    73,    -1,
      75,    -1,    -1,    -1,    79,    80,    -1,    82,    83,    84,
      85,    86,    87,    88,    73,    -1,    75,    -1,    -1,    -1,
      79,    80,    -1,    82,    83,    84,    85,    86,    87,    88,
       3,     4,     5,     6,    -1,    -1,     9,    10,    -1,    -1,
      -1,    14,    15,    -1,    17,    18,     3,     4,     5,     6,
      -1,    -1,     9,    10,    -1,    -1,    -1,    14,    15,    -1,
      17,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,
      -1,    44,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    42,    -1,    44,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      73,    -1,    75,    -1,    -1,    -1,    79,    -1,    -1,    82,
      83,    84,    85,    86,    87,    88,    73,    -1,    -1,    -1,
      -1,    -1,    79,    -1,    -1,    82,    83,    84,    85,    86,
      87,    88,     3,     4,     5,     6,    -1,    -1,     9,    10,
      -1,    -1,    -1,    14,    15,    -1,    17,    18,     3,     4,
       5,     6,    -1,    -1,     9,    10,    -1,    -1,    -1,    14,
      15,    -1,    17,    18,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    42,    -1,    44,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    44,
      -1,     3,    -1,    -1,    -1,     7,     8,    -1,    -1,    -1,
      -1,    -1,    73,    -1,    -1,     3,    -1,    -1,    79,     7,
       8,    82,    83,    84,    85,    86,    87,    88,    73,    -1,
      -1,    -1,    -1,    -1,    79,    -1,    -1,    82,    83,    84,
      85,    86,    87,    88,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,     3,    74,    -1,    -1,     7,     8,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    72,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     1,     3,     4,     5,     6,     9,    10,    12,    14,
      15,    17,    18,    39,    40,    42,    43,    44,    45,    60,
      61,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      73,    75,    79,    82,    83,    84,    85,    86,    87,    88,
      97,   101,   102,   104,   105,   106,   107,   110,   111,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   134,   135,   136,   137,   184,
     185,   188,   190,   191,   192,   194,   195,   196,   197,   198,
     199,   200,   201,    97,    73,   101,   114,   101,    73,    73,
     114,   114,   101,   101,   130,    75,   137,    75,   188,    73,
     101,    73,    73,    73,   184,    73,    97,    97,    97,   137,
     137,    78,     7,     8,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    72,   101,   137,
     143,   144,   147,   151,   152,   168,   169,   170,   171,   172,
      41,    76,   137,   184,   186,   187,   189,    78,    80,   108,
     109,   112,   136,   101,   168,    72,    78,    73,    16,    17,
      18,    73,    79,    81,    73,   114,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    96,   129,
     114,   116,    84,    89,    90,    85,    86,    19,    20,    21,
      22,    91,    92,    23,    24,    83,    93,    94,    25,    26,
      95,    77,    97,   101,     0,   201,   168,    75,   137,    97,
      77,    97,   137,    75,    98,   136,    96,   137,   137,   137,
      64,   137,   193,    97,    78,   184,   101,   103,   101,    72,
      74,   143,   101,   103,   152,   170,    74,    73,    79,    84,
     162,   180,   181,   152,   130,    76,    76,   186,   189,    76,
     184,    80,    77,    80,    77,    80,    78,    72,    82,   101,
     184,    74,   130,   131,   101,    74,   112,   136,   137,   101,
     168,   168,   136,   136,   116,   116,   116,   117,   117,   118,
     118,   119,   119,   119,   119,   120,   120,   121,   122,   123,
     124,   125,   137,   136,    73,    74,    76,   130,    77,   101,
       1,    11,    58,    76,    99,   101,   132,   133,   139,   141,
     144,   145,   146,   147,   152,   154,   157,   137,    74,    77,
     137,    74,    74,    74,    73,    97,   184,    72,   101,   144,
      72,   116,    74,   180,    80,   127,   138,   152,   162,   181,
      73,    79,    97,   116,    76,    80,   108,   136,    80,   136,
     136,   101,   188,    59,    74,    74,    77,    80,    74,    78,
      74,   131,    76,    77,   168,    97,   154,    75,   101,   103,
     138,    76,   133,   152,    75,   103,   145,    73,    97,   101,
     142,   160,   161,   162,    75,   112,    97,   184,   188,   184,
     137,   193,   103,   103,   152,   153,    74,    80,   162,    58,
      74,   101,   144,   147,   152,   173,   174,   175,   176,   177,
     178,   179,    80,   138,   188,   127,    79,   188,    74,    76,
      77,   160,    76,    77,   101,   158,   159,    75,   154,     1,
      76,    99,   101,   148,   149,   150,   154,    75,   160,    77,
      97,    73,    79,   161,    76,   132,    74,    62,    74,    97,
     103,   153,   103,   177,    74,    77,    73,   101,   162,   180,
     182,   183,   152,    80,   137,    79,   188,   101,    97,    76,
      96,    76,    77,    76,    77,   158,   142,    97,    99,   138,
      78,   149,   150,    76,   155,   156,   160,    76,   148,   150,
      74,   160,    74,    99,   140,   163,   164,   165,    80,   138,
      76,   184,    97,   193,   153,    59,   175,   182,   183,    73,
      79,    80,   137,    74,   138,    76,   159,    76,    76,    77,
      97,   138,    97,   154,    76,    77,    97,   150,    76,   153,
     138,   154,    74,    77,    80,    74,    74,    74,   173,    80,
     138,   188,    80,    76,   154,    97,   155,   156,    76,    73,
      79,   160,   162,   166,   167,   153,    59,   165,   184,    74,
      80,   188,   156,    97,    74,   166,    80,   138,   167,    73,
      79,    78,   153,    74,    80,    74,   163,    80,   138,   138,
     153,    74,    80,    97,   153
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
#line 89 "c.y"
    { ((*yyvalp).expr) = doidnsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 3:

/* Line 936 of glr.c  */
#line 93 "c.y"
    { ((*yyvalp).expr) = doticksrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 6:

/* Line 936 of glr.c  */
#line 104 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elambda, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 7:

/* Line 936 of glr.c  */
#line 106 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elambda, nullelist(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 8:

/* Line 936 of glr.c  */
#line 111 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefrec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 9:

/* Line 936 of glr.c  */
#line 113 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefrec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 10:

/* Line 936 of glr.c  */
#line 115 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefrec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 11:

/* Line 936 of glr.c  */
#line 120 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 12:

/* Line 936 of glr.c  */
#line 125 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, Eelist,
				     (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0),
			  0, 0);
	;}
    break;

  case 13:

/* Line 936 of glr.c  */
#line 135 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 14:

/* Line 936 of glr.c  */
#line 137 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 17:

/* Line 936 of glr.c  */
#line 144 "c.y"
    { ((*yyvalp).expr) = dosymsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 18:

/* Line 936 of glr.c  */
#line 146 "c.y"
    { ((*yyvalp).expr) = doconst(ctx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 19:

/* Line 936 of glr.c  */
#line 148 "c.y"
    { ((*yyvalp).expr) = doconstssrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 20:

/* Line 936 of glr.c  */
#line 150 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 21:

/* Line 936 of glr.c  */
#line 152 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, nullelist(), 0, 0, 0); ;}
    break;

  case 22:

/* Line 936 of glr.c  */
#line 154 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 23:

/* Line 936 of glr.c  */
#line 156 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 24:

/* Line 936 of glr.c  */
#line 158 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, nullelist(), 0, 0, 0); ;}
    break;

  case 25:

/* Line 936 of glr.c  */
#line 160 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 26:

/* Line 936 of glr.c  */
#line 162 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 31:

/* Line 936 of glr.c  */
#line 171 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 32:

/* Line 936 of glr.c  */
#line 173 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 33:

/* Line 936 of glr.c  */
#line 176 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 34:

/* Line 936 of glr.c  */
#line 178 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edot, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 35:

/* Line 936 of glr.c  */
#line 180 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earrow, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 36:

/* Line 936 of glr.c  */
#line 182 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epostinc, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 37:

/* Line 936 of glr.c  */
#line 184 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epostdec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 38:

/* Line 936 of glr.c  */
#line 186 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Econtainer, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 39:

/* Line 936 of glr.c  */
#line 191 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 40:

/* Line 936 of glr.c  */
#line 193 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 43:

/* Line 936 of glr.c  */
#line 203 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epreinc, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 44:

/* Line 936 of glr.c  */
#line 205 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epredec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 45:

/* Line 936 of glr.c  */
#line 207 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 46:

/* Line 936 of glr.c  */
#line 209 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Esizeofe, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 47:

/* Line 936 of glr.c  */
#line 211 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Esizeoft, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 48:

/* Line 936 of glr.c  */
#line 213 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypeofe, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 49:

/* Line 936 of glr.c  */
#line 215 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypeoft, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 50:

/* Line 936 of glr.c  */
#line 217 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypeoft, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 51:

/* Line 936 of glr.c  */
#line 222 "c.y"
    { ((*yyvalp).kind) = Eref; ;}
    break;

  case 52:

/* Line 936 of glr.c  */
#line 224 "c.y"
    { ((*yyvalp).kind) = Ederef; ;}
    break;

  case 53:

/* Line 936 of glr.c  */
#line 226 "c.y"
    { ((*yyvalp).kind) = Euplus; ;}
    break;

  case 54:

/* Line 936 of glr.c  */
#line 228 "c.y"
    { ((*yyvalp).kind) = Euminus; ;}
    break;

  case 55:

/* Line 936 of glr.c  */
#line 230 "c.y"
    { ((*yyvalp).kind) = Eutwiddle; ;}
    break;

  case 56:

/* Line 936 of glr.c  */
#line 232 "c.y"
    { ((*yyvalp).kind) = Eunot; ;}
    break;

  case 58:

/* Line 936 of glr.c  */
#line 238 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecast, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 59:

/* Line 936 of glr.c  */
#line 240 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Excast, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 61:

/* Line 936 of glr.c  */
#line 246 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Emul, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 62:

/* Line 936 of glr.c  */
#line 248 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ediv, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 63:

/* Line 936 of glr.c  */
#line 250 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Emod, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 65:

/* Line 936 of glr.c  */
#line 256 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eadd, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 66:

/* Line 936 of glr.c  */
#line 258 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Esub, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 68:

/* Line 936 of glr.c  */
#line 264 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eshl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 69:

/* Line 936 of glr.c  */
#line 266 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eshr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 71:

/* Line 936 of glr.c  */
#line 272 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Elt, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 72:

/* Line 936 of glr.c  */
#line 274 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Egt, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 73:

/* Line 936 of glr.c  */
#line 276 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ele, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 74:

/* Line 936 of glr.c  */
#line 278 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ege, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 76:

/* Line 936 of glr.c  */
#line 284 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eeq, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 77:

/* Line 936 of glr.c  */
#line 286 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eneq, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 79:

/* Line 936 of glr.c  */
#line 292 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eband, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 81:

/* Line 936 of glr.c  */
#line 298 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ebxor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 83:

/* Line 936 of glr.c  */
#line 304 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ebor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 85:

/* Line 936 of glr.c  */
#line 310 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eland, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 87:

/* Line 936 of glr.c  */
#line 316 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 89:

/* Line 936 of glr.c  */
#line 322 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Econd, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 91:

/* Line 936 of glr.c  */
#line 328 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src,
			  Eg,
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src,
				     Ecast,
				     (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr),
				     copyexpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr)),
				     0, 0),
			  0, 0);
	;}
    break;

  case 92:

/* Line 936 of glr.c  */
#line 339 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src,
			  Eg,
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newbinopsrc(&ctx->inp->src, Excast, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), copyexpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr))),
			  0, 0);
	;}
    break;

  case 93:

/* Line 936 of glr.c  */
#line 346 "c.y"
    { if((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.kind) == Eg)
	  	((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0);
	  else
	  	((*yyvalp).expr) = newgopsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr));
	;}
    break;

  case 94:

/* Line 936 of glr.c  */
#line 355 "c.y"
    { ((*yyvalp).kind) = Eg; ;}
    break;

  case 95:

/* Line 936 of glr.c  */
#line 357 "c.y"
    { ((*yyvalp).kind) = Emul; ;}
    break;

  case 96:

/* Line 936 of glr.c  */
#line 359 "c.y"
    { ((*yyvalp).kind) = Ediv; ;}
    break;

  case 97:

/* Line 936 of glr.c  */
#line 361 "c.y"
    { ((*yyvalp).kind) = Emod; ;}
    break;

  case 98:

/* Line 936 of glr.c  */
#line 363 "c.y"
    { ((*yyvalp).kind) = Eadd; ;}
    break;

  case 99:

/* Line 936 of glr.c  */
#line 365 "c.y"
    { ((*yyvalp).kind) = Esub; ;}
    break;

  case 100:

/* Line 936 of glr.c  */
#line 367 "c.y"
    { ((*yyvalp).kind) = Eshl; ;}
    break;

  case 101:

/* Line 936 of glr.c  */
#line 369 "c.y"
    { ((*yyvalp).kind) = Eshr; ;}
    break;

  case 102:

/* Line 936 of glr.c  */
#line 371 "c.y"
    { ((*yyvalp).kind) = Eband; ;}
    break;

  case 103:

/* Line 936 of glr.c  */
#line 373 "c.y"
    { ((*yyvalp).kind) = Ebxor; ;}
    break;

  case 104:

/* Line 936 of glr.c  */
#line 375 "c.y"
    { ((*yyvalp).kind) = Ebor; ;}
    break;

  case 105:

/* Line 936 of glr.c  */
#line 380 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 106:

/* Line 936 of glr.c  */
#line 382 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 107:

/* Line 936 of glr.c  */
#line 387 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 108:

/* Line 936 of glr.c  */
#line 389 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
			  newexprsrc(&ctx->inp->src, Eellipsis, 0, 0, 0, 0),
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0);
	;}
    break;

  case 109:

/* Line 936 of glr.c  */
#line 397 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 110:

/* Line 936 of glr.c  */
#line 399 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 113:

/* Line 936 of glr.c  */
#line 406 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 115:

/* Line 936 of glr.c  */
#line 412 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ens, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 116:

/* Line 936 of glr.c  */
#line 414 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ens, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 117:

/* Line 936 of glr.c  */
#line 416 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ens, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (7))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (7))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 118:

/* Line 936 of glr.c  */
#line 418 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ens, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), nullelist(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 119:

/* Line 936 of glr.c  */
#line 423 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elapply, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 120:

/* Line 936 of glr.c  */
#line 425 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elapply, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 124:

/* Line 936 of glr.c  */
#line 436 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecomma, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 126:

/* Line 936 of glr.c  */
#line 445 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 127:

/* Line 936 of glr.c  */
#line 450 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 128:

/* Line 936 of glr.c  */
#line 452 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 129:

/* Line 936 of glr.c  */
#line 457 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecls, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 130:

/* Line 936 of glr.c  */
#line 459 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecls, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 131:

/* Line 936 of glr.c  */
#line 461 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecls, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 132:

/* Line 936 of glr.c  */
#line 466 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 133:

/* Line 936 of glr.c  */
#line 468 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 134:

/* Line 936 of glr.c  */
#line 473 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Evoid, 0, 0, 0, 0); ;}
    break;

  case 135:

/* Line 936 of glr.c  */
#line 475 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebool, 0, 0, 0, 0); ;}
    break;

  case 136:

/* Line 936 of glr.c  */
#line 477 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Echar, 0, 0, 0, 0); ;}
    break;

  case 137:

/* Line 936 of glr.c  */
#line 479 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eshort, 0, 0, 0, 0); ;}
    break;

  case 138:

/* Line 936 of glr.c  */
#line 481 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eint, 0, 0, 0, 0); ;}
    break;

  case 139:

/* Line 936 of glr.c  */
#line 483 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elong, 0, 0, 0, 0); ;}
    break;

  case 140:

/* Line 936 of glr.c  */
#line 485 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efloat, 0, 0, 0, 0); ;}
    break;

  case 141:

/* Line 936 of glr.c  */
#line 487 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edouble, 0, 0, 0, 0); ;}
    break;

  case 142:

/* Line 936 of glr.c  */
#line 489 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Esigned, 0, 0, 0, 0); ;}
    break;

  case 143:

/* Line 936 of glr.c  */
#line 491 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eunsigned, 0, 0, 0, 0); ;}
    break;

  case 144:

/* Line 936 of glr.c  */
#line 495 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 145:

/* Line 936 of glr.c  */
#line 497 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 146:

/* Line 936 of glr.c  */
#line 502 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 147:

/* Line 936 of glr.c  */
#line 504 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etid, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 150:

/* Line 936 of glr.c  */
#line 511 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert(newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr), 0, 0)), 0, 0); ;}
    break;

  case 151:

/* Line 936 of glr.c  */
#line 513 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.kind), 0, invert(newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), 0, 0)), 0, 0); ;}
    break;

  case 152:

/* Line 936 of glr.c  */
#line 515 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0), 0, 0); ;}
    break;

  case 153:

/* Line 936 of glr.c  */
#line 517 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.kind), 0, newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0), 0, 0); ;}
    break;

  case 154:

/* Line 936 of glr.c  */
#line 519 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 155:

/* Line 936 of glr.c  */
#line 521 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.kind), 0, nullelist(), 0, 0); ;}
    break;

  case 156:

/* Line 936 of glr.c  */
#line 523 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 157:

/* Line 936 of glr.c  */
#line 528 "c.y"
    { ((*yyvalp).kind) = Estruct; ;}
    break;

  case 158:

/* Line 936 of glr.c  */
#line 530 "c.y"
    { ((*yyvalp).kind) = Eunion; ;}
    break;

  case 159:

/* Line 936 of glr.c  */
#line 535 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 160:

/* Line 936 of glr.c  */
#line 537 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 161:

/* Line 936 of glr.c  */
#line 542 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efields, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 162:

/* Line 936 of glr.c  */
#line 544 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebitfield, (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 163:

/* Line 936 of glr.c  */
#line 546 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efields, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), nullelist(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 164:

/* Line 936 of glr.c  */
#line 548 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efields, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 165:

/* Line 936 of glr.c  */
#line 551 "c.y"
    { freeexpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr)); ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Enop, 0, 0, 0, 0); ;}
    break;

  case 166:

/* Line 936 of glr.c  */
#line 553 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 167:

/* Line 936 of glr.c  */
#line 558 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efieldoff, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 175:

/* Line 936 of glr.c  */
#line 581 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 176:

/* Line 936 of glr.c  */
#line 583 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 177:

/* Line 936 of glr.c  */
#line 588 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 178:

/* Line 936 of glr.c  */
#line 590 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 180:

/* Line 936 of glr.c  */
#line 599 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, 0, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 181:

/* Line 936 of glr.c  */
#line 601 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 182:

/* Line 936 of glr.c  */
#line 603 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, 0, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 183:

/* Line 936 of glr.c  */
#line 605 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 184:

/* Line 936 of glr.c  */
#line 607 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, 0, nullelist(), 0, 0); ;}
    break;

  case 185:

/* Line 936 of glr.c  */
#line 609 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 186:

/* Line 936 of glr.c  */
#line 611 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, 0, nullelist(), 0, 0); ;}
    break;

  case 187:

/* Line 936 of glr.c  */
#line 613 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 188:

/* Line 936 of glr.c  */
#line 615 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 189:

/* Line 936 of glr.c  */
#line 620 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 190:

/* Line 936 of glr.c  */
#line 622 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 191:

/* Line 936 of glr.c  */
#line 629 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenumel, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 192:

/* Line 936 of glr.c  */
#line 631 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenumel, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 193:

/* Line 936 of glr.c  */
#line 636 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 195:

/* Line 936 of glr.c  */
#line 642 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 196:

/* Line 936 of glr.c  */
#line 644 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 197:

/* Line 936 of glr.c  */
#line 646 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 198:

/* Line 936 of glr.c  */
#line 648 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 199:

/* Line 936 of glr.c  */
#line 650 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 200:

/* Line 936 of glr.c  */
#line 652 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 201:

/* Line 936 of glr.c  */
#line 657 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, 0, 0, 0, 0); ;}
    break;

  case 202:

/* Line 936 of glr.c  */
#line 659 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, 0, 0, 0, 0); ;}
    break;

  case 203:

/* Line 936 of glr.c  */
#line 661 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 204:

/* Line 936 of glr.c  */
#line 663 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 205:

/* Line 936 of glr.c  */
#line 668 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 206:

/* Line 936 of glr.c  */
#line 674 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 207:

/* Line 936 of glr.c  */
#line 679 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 208:

/* Line 936 of glr.c  */
#line 681 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 209:

/* Line 936 of glr.c  */
#line 686 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 210:

/* Line 936 of glr.c  */
#line 688 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 211:

/* Line 936 of glr.c  */
#line 690 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 214:

/* Line 936 of glr.c  */
#line 697 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 215:

/* Line 936 of glr.c  */
#line 702 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 216:

/* Line 936 of glr.c  */
#line 704 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, 0, 0, 0); ;}
    break;

  case 217:

/* Line 936 of glr.c  */
#line 706 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 218:

/* Line 936 of glr.c  */
#line 708 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 219:

/* Line 936 of glr.c  */
#line 710 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 220:

/* Line 936 of glr.c  */
#line 712 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, 0, nullelist(), 0, 0); ;}
    break;

  case 221:

/* Line 936 of glr.c  */
#line 734 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 222:

/* Line 936 of glr.c  */
#line 736 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 223:

/* Line 936 of glr.c  */
#line 741 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 224:

/* Line 936 of glr.c  */
#line 743 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 226:

/* Line 936 of glr.c  */
#line 749 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 227:

/* Line 936 of glr.c  */
#line 751 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 228:

/* Line 936 of glr.c  */
#line 756 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 229:

/* Line 936 of glr.c  */
#line 758 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 230:

/* Line 936 of glr.c  */
#line 760 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etid, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 231:

/* Line 936 of glr.c  */
#line 762 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etid, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 234:

/* Line 936 of glr.c  */
#line 769 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 235:

/* Line 936 of glr.c  */
#line 771 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 236:

/* Line 936 of glr.c  */
#line 776 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 237:

/* Line 936 of glr.c  */
#line 778 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 238:

/* Line 936 of glr.c  */
#line 783 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 239:

/* Line 936 of glr.c  */
#line 789 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 240:

/* Line 936 of glr.c  */
#line 794 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 241:

/* Line 936 of glr.c  */
#line 796 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 242:

/* Line 936 of glr.c  */
#line 801 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 243:

/* Line 936 of glr.c  */
#line 803 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 244:

/* Line 936 of glr.c  */
#line 805 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 246:

/* Line 936 of glr.c  */
#line 811 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 247:

/* Line 936 of glr.c  */
#line 813 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 248:

/* Line 936 of glr.c  */
#line 818 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 249:

/* Line 936 of glr.c  */
#line 820 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etid, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 252:

/* Line 936 of glr.c  */
#line 827 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 253:

/* Line 936 of glr.c  */
#line 832 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 256:

/* Line 936 of glr.c  */
#line 839 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 257:

/* Line 936 of glr.c  */
#line 844 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 258:

/* Line 936 of glr.c  */
#line 846 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, 0, 0, 0); ;}
    break;

  case 259:

/* Line 936 of glr.c  */
#line 848 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 260:

/* Line 936 of glr.c  */
#line 850 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 261:

/* Line 936 of glr.c  */
#line 852 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 262:

/* Line 936 of glr.c  */
#line 854 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, 0, nullelist(), 0, 0); ;}
    break;

  case 263:

/* Line 936 of glr.c  */
#line 856 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 264:

/* Line 936 of glr.c  */
#line 858 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 265:

/* Line 936 of glr.c  */
#line 863 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 267:

/* Line 936 of glr.c  */
#line 869 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 268:

/* Line 936 of glr.c  */
#line 871 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 269:

/* Line 936 of glr.c  */
#line 873 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 270:

/* Line 936 of glr.c  */
#line 875 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 271:

/* Line 936 of glr.c  */
#line 877 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 272:

/* Line 936 of glr.c  */
#line 879 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 282:

/* Line 936 of glr.c  */
#line 896 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eglobal, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 283:

/* Line 936 of glr.c  */
#line 901 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 284:

/* Line 936 of glr.c  */
#line 906 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 285:

/* Line 936 of glr.c  */
#line 908 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 286:

/* Line 936 of glr.c  */
#line 913 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eblock, nullelist(), nullelist(), 0, 0); ;}
    break;

  case 287:

/* Line 936 of glr.c  */
#line 915 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eblock, flatten((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), nullelist(), 0, 0); ;}
    break;

  case 288:

/* Line 936 of glr.c  */
#line 917 "c.y"
    {
		/* use src of first statement */
		Expr *sl;
		sl = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr));
		((*yyvalp).expr) = newexprsrc(&sl->src, Eblock, nullelist(), sl, 0, 0);
	;}
    break;

  case 289:

/* Line 936 of glr.c  */
#line 924 "c.y"
    {
		/* use src of first statement */
		Expr *sl;
		sl = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr));
		((*yyvalp).expr) = newexprsrc(&sl->src, Eblock, flatten((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), sl, 0, 0);
	;}
    break;

  case 290:

/* Line 936 of glr.c  */
#line 934 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 291:

/* Line 936 of glr.c  */
#line 936 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 292:

/* Line 936 of glr.c  */
#line 941 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Enil, 0, 0, 0, 0); ;}
    break;

  case 293:

/* Line 936 of glr.c  */
#line 943 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 294:

/* Line 936 of glr.c  */
#line 945 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 295:

/* Line 936 of glr.c  */
#line 950 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eif, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 296:

/* Line 936 of glr.c  */
#line 952 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eif, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (7))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (7))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (7))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 297:

/* Line 936 of glr.c  */
#line 955 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eswitch, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 298:

/* Line 936 of glr.c  */
#line 960 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr)->src, Elabel, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), NULL, NULL); ;}
    break;

  case 299:

/* Line 936 of glr.c  */
#line 962 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), NULL, NULL); ;}
    break;

  case 300:

/* Line 936 of glr.c  */
#line 964 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefault, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), NULL, NULL, NULL); ;}
    break;

  case 302:

/* Line 936 of glr.c  */
#line 970 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 303:

/* Line 936 of glr.c  */
#line 975 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ewhile, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 304:

/* Line 936 of glr.c  */
#line 977 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edo, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (7))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 305:

/* Line 936 of glr.c  */
#line 979 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (9))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (9))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (9))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((9) - (9))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 306:

/* Line 936 of glr.c  */
#line 984 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Egoto, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 307:

/* Line 936 of glr.c  */
#line 986 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Econtinue, 0, 0, 0, 0); ;}
    break;

  case 308:

/* Line 936 of glr.c  */
#line 988 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebreak, 0, 0, 0, 0); ;}
    break;

  case 309:

/* Line 936 of glr.c  */
#line 990 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eret, 0, 0, 0, 0); ;}
    break;

  case 310:

/* Line 936 of glr.c  */
#line 992 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eret, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 312:

/* Line 936 of glr.c  */
#line 1001 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr)->src, Edefine, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (6))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 313:

/* Line 936 of glr.c  */
#line 1003 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr)->src, Edefine, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), nullelist(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 314:

/* Line 936 of glr.c  */
#line 1005 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (9))].yystate.yysemantics.yysval.expr)->src, Edefine, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (9))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (9))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((9) - (9))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (9))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 315:

/* Line 936 of glr.c  */
#line 1007 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (8))].yystate.yysemantics.yysval.expr)->src, Edefine, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (8))].yystate.yysemantics.yysval.expr), nullelist(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((8) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (8))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 316:

/* Line 936 of glr.c  */
#line 1012 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefconst, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 321:

/* Line 936 of glr.c  */
#line 1026 "c.y"
    { dotop(ctx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr)); ;}
    break;



/* Line 936 of glr.c  */
#line 3946 "c.tab.c"
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
#line 1029 "c.y"

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
	if(e->kind == Etick){
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
	if(other->e1->kind != Etick)
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

