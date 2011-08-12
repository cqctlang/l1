
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
#define YYFINAL  224
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2986

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  108
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  108
/* YYNRULES -- Number of rules.  */
#define YYNRULES  344
/* YYNRULES -- Number of states.  */
#define YYNSTATES  632
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
      83,    84,    94,    95,    87,    96,    92,    99,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    88,   106,
     101,    91,   102,   105,   107,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    89,     2,    90,   103,     2,    82,     2,     2,     2,
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
       0,     0,     3,     5,     7,     8,    12,    14,    20,    25,
      31,    38,    43,    46,    50,    54,    58,    62,    65,    70,
      75,    81,    85,    87,    91,    93,    95,    97,    99,   101,
     105,   111,   114,   118,   123,   128,   132,   136,   141,   143,
     145,   147,   149,   151,   153,   155,   157,   161,   167,   170,
     174,   179,   184,   188,   192,   197,   202,   206,   208,   210,
     212,   216,   220,   226,   228,   232,   236,   238,   243,   247,
     252,   256,   260,   263,   266,   275,   277,   281,   283,   286,
     289,   292,   295,   300,   303,   308,   313,   315,   317,   319,
     321,   323,   325,   327,   332,   337,   339,   343,   347,   351,
     353,   357,   361,   363,   367,   371,   373,   377,   381,   385,
     389,   391,   395,   399,   401,   405,   407,   411,   413,   417,
     419,   423,   425,   429,   431,   437,   439,   443,   445,   447,
     449,   451,   453,   455,   457,   459,   461,   463,   465,   467,
     471,   473,   476,   478,   481,   483,   485,   488,   490,   496,
     501,   508,   513,   515,   517,   519,   523,   525,   530,   533,
     534,   537,   543,   547,   549,   553,   555,   557,   559,   561,
     563,   565,   567,   569,   571,   573,   575,   578,   580,   582,
     584,   586,   593,   599,   604,   607,   609,   611,   613,   615,
     617,   619,   622,   628,   633,   637,   646,   649,   652,   656,
     658,   660,   662,   665,   667,   668,   670,   673,   676,   678,
     682,   684,   690,   697,   702,   708,   711,   713,   717,   719,
     723,   726,   728,   730,   734,   739,   743,   749,   754,   756,
     758,   761,   765,   768,   772,   776,   781,   785,   790,   796,
     798,   801,   804,   808,   810,   814,   816,   820,   824,   828,
     831,   833,   836,   838,   841,   844,   846,   850,   854,   857,
     860,   865,   867,   871,   873,   877,   880,   883,   885,   887,
     890,   893,   895,   897,   899,   901,   904,   907,   909,   911,
     914,   918,   921,   925,   929,   934,   938,   943,   949,   952,
     954,   956,   960,   965,   969,   974,   978,   980,   982,   984,
     986,   988,   990,   992,   994,   996,  1001,  1005,  1009,  1011,
    1014,  1017,  1021,  1025,  1030,  1032,  1035,  1037,  1040,  1043,
    1049,  1057,  1063,  1067,  1072,  1077,  1081,  1083,  1084,  1090,
    1098,  1108,  1112,  1115,  1118,  1121,  1125,  1127,  1129,  1136,
    1142,  1144,  1145,  1147,  1150
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const short int yyrhs[] =
{
     213,     0,    -1,     3,    -1,   109,    -1,    -1,   109,    82,
     109,    -1,    43,    -1,   112,    83,   146,    84,   203,    -1,
     112,    83,    84,   203,    -1,    15,   109,    85,   145,    86,
      -1,    15,   109,    85,   145,    87,    86,    -1,    15,   109,
      85,    86,    -1,    45,   203,    -1,    74,   198,    86,    -1,
      74,   152,    86,    -1,    75,   198,    86,    -1,    75,   152,
      86,    -1,    76,   109,    -1,    76,    83,   152,    84,    -1,
      73,    83,   109,    84,    -1,    72,   109,    83,   128,    84,
      -1,   151,    88,   151,    -1,   118,    -1,   119,    87,   118,
      -1,   109,    -1,   111,    -1,     4,    -1,     5,    -1,     6,
      -1,    83,   152,    84,    -1,    78,   151,    87,   151,    79,
      -1,    89,    90,    -1,    89,   128,    90,    -1,    89,   128,
      87,    90,    -1,    89,   128,    60,    90,    -1,    89,    88,
      90,    -1,    89,   119,    90,    -1,    89,   119,    87,    90,
      -1,   113,    -1,   114,    -1,   115,    -1,   116,    -1,   117,
      -1,   109,    -1,     5,    -1,     6,    -1,    83,   121,    84,
      -1,    78,   121,    87,   121,    79,    -1,    89,    90,    -1,
      89,   123,    90,    -1,    89,   123,    87,    90,    -1,    89,
     123,    60,    90,    -1,    89,    88,    90,    -1,    89,   125,
      90,    -1,    89,   125,    87,    90,    -1,   109,    83,   122,
      84,    -1,   109,    83,    84,    -1,   123,    -1,   124,    -1,
     121,    -1,   123,    87,   121,    -1,   109,    91,   121,    -1,
     124,    87,   109,    91,   121,    -1,   126,    -1,   125,    87,
     126,    -1,   121,    88,   121,    -1,   120,    -1,   127,    89,
     152,    90,    -1,   127,    83,    84,    -1,   127,    83,   128,
      84,    -1,   127,    92,   109,    -1,   127,    17,   109,    -1,
     127,    18,    -1,   127,    19,    -1,    16,    83,   152,    87,
     184,    87,   109,    84,    -1,   151,    -1,   128,    87,   151,
      -1,   127,    -1,    18,   129,    -1,    19,   129,    -1,   130,
     131,    -1,     9,   129,    -1,     9,    83,   184,    84,    -1,
      11,   129,    -1,    11,    83,   184,    84,    -1,    10,    83,
     184,    84,    -1,    93,    -1,    94,    -1,    95,    -1,    96,
      -1,    97,    -1,    98,    -1,   129,    -1,    83,   184,    84,
     131,    -1,    85,   152,    86,   131,    -1,   131,    -1,   132,
      94,   131,    -1,   132,    99,   131,    -1,   132,   100,   131,
      -1,   132,    -1,   133,    95,   132,    -1,   133,    96,   132,
      -1,   133,    -1,   134,    20,   133,    -1,   134,    21,   133,
      -1,   134,    -1,   135,   101,   134,    -1,   135,   102,   134,
      -1,   135,    22,   134,    -1,   135,    23,   134,    -1,   135,
      -1,   136,    24,   135,    -1,   136,    25,   135,    -1,   136,
      -1,   137,    93,   136,    -1,   137,    -1,   138,   103,   137,
      -1,   138,    -1,   139,   104,   138,    -1,   139,    -1,   140,
      26,   139,    -1,   140,    -1,   141,    27,   140,    -1,   141,
      -1,   141,   105,   152,    88,   142,    -1,   142,    -1,   129,
     144,   151,    -1,    91,    -1,    31,    -1,    35,    -1,    37,
      -1,    36,    -1,    29,    -1,    30,    -1,    28,    -1,    33,
      -1,    34,    -1,    32,    -1,   109,    -1,   145,    87,   109,
      -1,   145,    -1,   145,    60,    -1,   148,    -1,   147,   148,
      -1,   156,    -1,   154,    -1,     1,   106,    -1,   143,    -1,
      44,   152,    85,   147,    86,    -1,    44,   152,    85,    86,
      -1,    46,    83,   151,    87,   128,    84,    -1,    46,    83,
     151,    84,    -1,   149,    -1,   150,    -1,   151,    -1,   152,
      87,   151,    -1,   142,    -1,    12,   170,   157,   106,    -1,
     107,   153,    -1,    -1,   170,   106,    -1,   107,   153,   170,
     157,   106,    -1,   170,   157,   106,    -1,   176,    -1,   157,
      87,   176,    -1,    56,    -1,    47,    -1,    48,    -1,    49,
      -1,    50,    -1,    51,    -1,    54,    -1,    55,    -1,    52,
      -1,    53,    -1,   158,    -1,   159,   158,    -1,   159,    -1,
     109,    -1,   161,    -1,   173,    -1,   162,   110,    85,   164,
     166,    86,    -1,   162,   110,    85,   166,    86,    -1,   162,
     110,    85,    86,    -1,   162,   109,    -1,    57,    -1,    58,
      -1,    57,    -1,    58,    -1,    59,    -1,   165,    -1,   164,
     165,    -1,   107,   153,   170,   171,   106,    -1,   107,   153,
     170,   106,    -1,   170,   171,   106,    -1,   107,   107,   153,
     170,   172,    88,   153,   106,    -1,   109,    88,    -1,     1,
     106,    -1,   107,   153,   106,    -1,     7,    -1,     8,    -1,
     167,    -1,   167,   168,    -1,   168,    -1,    -1,   160,    -1,
     168,   160,    -1,   160,   168,    -1,   172,    -1,   171,    87,
     172,    -1,   176,    -1,    59,   110,    85,   174,    86,    -1,
      59,   110,    85,   174,    87,    86,    -1,    59,   110,    85,
      86,    -1,    59,   110,    85,    87,    86,    -1,    59,   109,
      -1,   175,    -1,   174,    87,   175,    -1,   109,    -1,   109,
      91,   153,    -1,   180,   177,    -1,   177,    -1,   109,    -1,
      83,   176,    84,    -1,   177,    89,   153,    90,    -1,   177,
      89,    90,    -1,   177,    83,   181,    84,   169,    -1,   177,
      83,    84,   169,    -1,   180,    -1,   179,    -1,   180,   179,
      -1,    83,   178,    84,    -1,    89,    90,    -1,    89,   153,
      90,    -1,   179,    89,    90,    -1,   179,    89,   153,    90,
      -1,    83,    84,   169,    -1,   179,    83,    84,   169,    -1,
     179,    83,   181,    84,   169,    -1,    94,    -1,    94,   168,
      -1,    94,   180,    -1,    94,   168,   180,    -1,   182,    -1,
     182,    87,    60,    -1,   183,    -1,   182,    87,   183,    -1,
     155,   170,   176,    -1,   155,   170,   178,    -1,   155,   170,
      -1,   185,    -1,   185,   194,    -1,   186,    -1,   168,   186,
      -1,   186,   168,    -1,   159,    -1,   109,    82,   159,    -1,
     109,    82,   109,    -1,    82,   109,    -1,   163,   109,    -1,
     163,   109,    82,   109,    -1,   188,    -1,   188,    87,    60,
      -1,   189,    -1,   188,    87,   189,    -1,   190,   196,    -1,
     190,   194,    -1,   190,    -1,   191,    -1,   191,   168,    -1,
     168,   191,    -1,   159,    -1,   109,    -1,   192,    -1,   193,
      -1,   162,   109,    -1,    59,   109,    -1,   180,    -1,   195,
      -1,   180,   195,    -1,    83,   194,    84,    -1,    89,    90,
      -1,    89,   153,    90,    -1,   195,    89,    90,    -1,   195,
      89,   153,    90,    -1,    83,    84,   169,    -1,   195,    83,
      84,   169,    -1,   195,    83,   187,    84,   169,    -1,   180,
     197,    -1,   197,    -1,   109,    -1,    83,   196,    84,    -1,
     197,    89,   153,    90,    -1,   197,    89,    90,    -1,   197,
      83,   187,    84,    -1,   197,    83,    84,    -1,   203,    -1,
     205,    -1,   206,    -1,   209,    -1,   210,    -1,   212,    -1,
     207,    -1,   200,    -1,   199,    -1,    76,    85,   109,    86,
      -1,    41,   145,   106,    -1,    42,   145,   106,    -1,   201,
      -1,   202,   201,    -1,    85,    86,    -1,    85,   202,    86,
      -1,    85,   204,    86,    -1,    85,   202,   204,    86,    -1,
     198,    -1,   204,   198,    -1,   106,    -1,   152,   106,    -1,
       1,   106,    -1,    61,    83,   152,    84,   198,    -1,    61,
      83,   152,    84,   198,    62,   198,    -1,    63,    83,   152,
      84,   203,    -1,   109,    88,   198,    -1,    70,   152,    88,
     198,    -1,    81,   121,    88,   198,    -1,    71,    88,   198,
      -1,   152,    -1,    -1,    64,    83,   152,    84,   198,    -1,
      65,   198,    64,    83,   152,    84,   106,    -1,    66,    83,
     208,   106,   208,   106,   208,    84,   198,    -1,    40,   109,
     106,    -1,    67,   106,    -1,    68,   106,    -1,    69,   106,
      -1,    69,   152,   106,    -1,    13,    -1,    14,    -1,   211,
     109,    83,   146,    84,   203,    -1,   211,   109,    83,    84,
     203,    -1,   214,    -1,    -1,   215,    -1,   214,   215,    -1,
     198,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,    95,    95,    99,   101,   105,   110,   114,   116,   121,
     123,   125,   130,   135,   137,   139,   141,   143,   145,   150,
     152,   157,   167,   169,   174,   175,   176,   178,   180,   182,
     184,   186,   188,   190,   192,   197,   199,   201,   203,   204,
     205,   206,   207,   211,   212,   214,   216,   218,   220,   222,
     224,   226,   231,   233,   235,   237,   239,   244,   245,   249,
     251,   256,   261,   269,   271,   276,   288,   289,   291,   293,
     295,   297,   299,   301,   303,   308,   310,   315,   316,   318,
     320,   322,   324,   326,   328,   330,   335,   337,   339,   341,
     343,   345,   350,   351,   353,   358,   359,   361,   363,   368,
     369,   371,   376,   377,   379,   384,   385,   387,   389,   391,
     396,   397,   399,   404,   405,   410,   411,   416,   417,   422,
     423,   428,   429,   434,   435,   440,   441,   450,   452,   454,
     456,   458,   460,   462,   464,   466,   468,   470,   475,   477,
     482,   484,   492,   494,   499,   500,   501,   506,   507,   509,
     514,   516,   521,   522,   526,   527,   532,   536,   541,   544,
     548,   550,   552,   557,   559,   564,   566,   568,   570,   572,
     574,   576,   578,   580,   582,   586,   588,   593,   595,   597,
     598,   602,   604,   606,   608,   613,   615,   620,   622,   624,
     629,   638,   649,   651,   653,   655,   658,   660,   665,   671,
     672,   676,   677,   681,   682,   686,   687,   689,   694,   696,
     701,   705,   707,   709,   711,   713,   718,   720,   727,   729,
     734,   736,   740,   742,   744,   746,   748,   750,   755,   756,
     757,   762,   764,   766,   768,   770,   772,   774,   776,   781,
     783,   785,   787,   792,   794,   799,   801,   806,   808,   810,
     815,   817,   822,   823,   825,   830,   832,   835,   838,   841,
     843,   848,   850,   855,   857,   862,   864,   866,   871,   872,
     874,   879,   881,   883,   884,   888,   893,   898,   899,   900,
     905,   907,   909,   911,   913,   915,   917,   919,   924,   926,
     930,   932,   934,   936,   938,   940,   945,   946,   947,   948,
     949,   950,   951,   952,   953,   957,   962,   967,   972,   974,
     979,   989,   999,  1012,  1028,  1030,  1035,  1037,  1039,  1044,
    1046,  1048,  1054,  1056,  1058,  1060,  1065,  1067,  1071,  1073,
    1075,  1080,  1082,  1084,  1086,  1088,  1093,  1095,  1100,  1102,
    1107,  1108,  1112,  1113,  1117
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
  "SYNTAXSPLICE", "LPAIR", "RPAIR", "NOBIND_PRE", "MATCH", "'`'", "'('",
  "')'", "'{'", "'}'", "','", "':'", "'['", "']'", "'='", "'.'", "'&'",
  "'*'", "'+'", "'-'", "'~'", "'!'", "'/'", "'%'", "'<'", "'>'", "'^'",
  "'|'", "'?'", "';'", "'@'", "$accept", "id", "maybeid", "tickid",
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
       0,   108,   109,   110,   110,   111,   112,   113,   113,   114,
     114,   114,   115,   116,   116,   116,   116,   116,   116,   117,
     117,   118,   119,   119,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   122,   122,   123,
     123,   124,   124,   125,   125,   126,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   128,   128,   129,   129,   129,
     129,   129,   129,   129,   129,   129,   130,   130,   130,   130,
     130,   130,   131,   131,   131,   132,   132,   132,   132,   133,
     133,   133,   134,   134,   134,   135,   135,   135,   135,   135,
     136,   136,   136,   137,   137,   138,   138,   139,   139,   140,
     140,   141,   141,   142,   142,   143,   143,   144,   144,   144,
     144,   144,   144,   144,   144,   144,   144,   144,   145,   145,
     146,   146,   147,   147,   148,   148,   148,   149,   149,   149,
     150,   150,   151,   151,   152,   152,   153,   154,   155,   155,
     156,   156,   156,   157,   157,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   159,   159,   160,   160,   160,
     160,   161,   161,   161,   161,   162,   162,   163,   163,   163,
     164,   164,   165,   165,   165,   165,   165,   165,   166,   167,
     167,   168,   168,   169,   169,   170,   170,   170,   171,   171,
     172,   173,   173,   173,   173,   173,   174,   174,   175,   175,
     176,   176,   177,   177,   177,   177,   177,   177,   178,   178,
     178,   179,   179,   179,   179,   179,   179,   179,   179,   180,
     180,   180,   180,   181,   181,   182,   182,   183,   183,   183,
     184,   184,   185,   185,   185,   186,   186,   186,   186,   186,
     186,   187,   187,   188,   188,   189,   189,   189,   190,   190,
     190,   191,   191,   191,   191,   192,   193,   194,   194,   194,
     195,   195,   195,   195,   195,   195,   195,   195,   196,   196,
     197,   197,   197,   197,   197,   197,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   199,   200,   201,   202,   202,
     203,   203,   203,   203,   204,   204,   205,   205,   205,   206,
     206,   206,   207,   207,   207,   207,   208,   208,   209,   209,
     209,   210,   210,   210,   210,   210,   211,   211,   212,   212,
     213,   213,   214,   214,   215
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     1,     0,     3,     1,     5,     4,     5,
       6,     4,     2,     3,     3,     3,     3,     2,     4,     4,
       5,     3,     1,     3,     1,     1,     1,     1,     1,     3,
       5,     2,     3,     4,     4,     3,     3,     4,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     5,     2,     3,
       4,     4,     3,     3,     4,     4,     3,     1,     1,     1,
       3,     3,     5,     1,     3,     3,     1,     4,     3,     4,
       3,     3,     2,     2,     8,     1,     3,     1,     2,     2,
       2,     2,     4,     2,     4,     4,     1,     1,     1,     1,
       1,     1,     1,     4,     4,     1,     3,     3,     3,     1,
       3,     3,     1,     3,     3,     1,     3,     3,     3,     3,
       1,     3,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     5,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     2,     1,     2,     1,     1,     2,     1,     5,     4,
       6,     4,     1,     1,     1,     3,     1,     4,     2,     0,
       2,     5,     3,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     1,     1,
       1,     6,     5,     4,     2,     1,     1,     1,     1,     1,
       1,     2,     5,     4,     3,     8,     2,     2,     3,     1,
       1,     1,     2,     1,     0,     1,     2,     2,     1,     3,
       1,     5,     6,     4,     5,     2,     1,     3,     1,     3,
       2,     1,     1,     3,     4,     3,     5,     4,     1,     1,
       2,     3,     2,     3,     3,     4,     3,     4,     5,     1,
       2,     2,     3,     1,     3,     1,     3,     3,     3,     2,
       1,     2,     1,     2,     2,     1,     3,     3,     2,     2,
       4,     1,     3,     1,     3,     2,     2,     1,     1,     2,
       2,     1,     1,     1,     1,     2,     2,     1,     1,     2,
       3,     2,     3,     3,     4,     3,     4,     5,     2,     1,
       1,     3,     4,     3,     4,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     4,     3,     3,     1,     2,
       2,     3,     3,     4,     1,     2,     1,     2,     2,     5,
       7,     5,     3,     4,     4,     3,     1,     0,     5,     7,
       9,     3,     2,     2,     2,     3,     1,     1,     6,     5,
       1,     0,     1,     2,     1
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,     2,
       1,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0
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
       0,     1,     1,     1,     1,     0,     0,     0,     0,     0,
       0,     0,     2,     2,     0,     3,     3,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     4,     4,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error.  */
static const unsigned short int yydefact[] =
{
       0,     0,     2,    26,    27,    28,     0,     0,     0,   336,
     337,     0,     0,     0,     0,     0,     0,     6,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    86,    87,    88,    89,    90,    91,   316,    24,    25,
       0,    38,    39,    40,    41,    42,    66,    77,    92,     0,
      95,    99,   102,   105,   110,   113,   115,   117,   119,   121,
     123,   125,   147,   152,   153,   154,     0,   344,   304,   303,
     296,   297,   298,   302,   299,   300,     0,   301,     0,     0,
     342,   318,     0,     0,    24,    81,     0,     0,    83,     0,
       0,     0,    78,    79,     0,   138,     0,     0,     0,     0,
      12,     0,     0,     0,     0,     0,   327,   332,   333,   334,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    17,     0,    44,    45,     0,     0,     0,    43,     0,
     199,   200,   166,   167,   168,   169,   170,   173,   174,   171,
     172,   165,   187,   188,   189,     0,    24,     0,   175,   255,
       0,   201,     0,     0,   250,   252,     0,   310,     0,   314,
     308,     0,     0,     0,    31,    22,     0,     0,    75,     0,
       0,     0,     0,    72,    73,     0,     0,     0,   134,   132,
     133,   128,   137,   135,   136,   129,   131,   130,   127,     0,
      92,    80,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   317,     0,     1,   343,     0,     0,     0,     0,
       0,     0,   331,     0,   306,     0,     0,     0,     0,     0,
       0,     0,   326,     0,   335,     0,   325,     0,     0,    14,
      13,    16,    15,     0,     0,     0,     0,     0,     0,    48,
      59,     0,     0,    63,     0,     0,   258,     0,    29,   176,
     259,   202,   253,     0,     0,     0,   239,   277,   251,   278,
     254,     0,     0,   311,   309,     0,   312,   315,    35,     0,
      36,     0,     0,    32,     0,     5,   322,     0,   140,     0,
      71,    68,     0,    75,     0,    70,   126,    96,    97,    98,
     100,   101,   103,   104,   108,   109,   106,   107,   111,   112,
     114,   116,   118,   120,   122,     0,   155,     0,    82,     0,
      85,    84,    11,     0,     0,   139,     0,     0,   185,   186,
       4,   149,     0,   178,     0,   142,   145,   144,   177,   205,
     179,     4,     0,     0,   180,   151,     0,     0,     0,     0,
       0,   327,   323,     0,    19,    18,   305,     0,     0,    46,
      52,     0,     0,     0,    49,     0,    53,    56,    43,    59,
       0,    57,    58,   324,     5,   256,     0,    93,   204,     0,
     281,   156,     0,   240,   241,   279,     0,     0,   307,    94,
     313,    37,    23,     0,    34,    33,    76,    21,     8,   141,
       0,    69,     0,    67,     0,     0,     0,   257,     9,     0,
       0,   146,     0,   215,     0,     0,   148,   143,   207,   184,
       0,   206,     0,   160,   222,     0,   163,   221,     0,     0,
     319,   321,   328,     0,     0,    20,    30,     0,    65,    51,
      50,    60,    54,     0,    64,     0,    55,     0,     0,   260,
     203,   285,   280,   282,   242,     0,   204,   272,   271,     0,
       0,     0,   261,   263,   267,   268,   273,   274,   283,     0,
       7,   124,   339,     0,    10,     0,     0,     0,     0,     0,
       0,     0,   162,   159,     0,   220,   150,     0,     0,   327,
      47,    61,     0,   276,   286,   275,   270,   204,     0,     0,
     290,   277,   266,   265,   289,   269,   284,   338,     0,   157,
     213,     0,   218,     0,   216,     0,     0,   183,     0,   178,
       0,   190,     0,     0,   223,   164,   204,     0,     0,     0,
     243,   245,   225,     0,   320,   329,     0,     0,   287,   262,
     264,     0,   288,     0,     0,    74,   214,     0,   211,     0,
     161,   197,     0,     0,   196,   191,     0,   182,     0,   208,
     210,   227,   158,   249,   204,   159,   224,     0,    62,   291,
     295,     0,   293,     0,   219,   212,   217,     0,   198,     0,
     181,     0,   194,     0,     0,   247,   248,   229,   228,   226,
     244,   246,   330,   294,   292,     0,   193,     0,   209,   204,
       0,   232,     0,   159,     0,   230,     0,   192,   236,   231,
     233,   204,     0,   234,     0,     0,   237,   204,   235,     0,
     238,   195
};

/* YYPDEFGOTO[NTERM-NUM].  */
static const short int yydefgoto[] =
{
      -1,    94,   424,    49,    50,    51,    52,    53,    54,    55,
     175,   176,    56,   451,   380,   261,   382,   262,   263,    57,
     177,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,   199,   298,   299,   344,
     345,    73,    74,    75,    76,   392,   346,   538,   347,   435,
     158,   348,   349,   350,   351,   160,   530,   531,   532,   161,
     352,   461,   353,   568,   569,   354,   523,   524,   570,   437,
     596,   597,   438,   539,   540,   541,   163,   164,   165,   471,
     472,   473,   474,   475,   476,   477,   389,   279,   513,   514,
     169,    78,    79,   170,   171,    80,   172,    81,    82,    83,
     243,    84,    85,    86,    87,    88,    89,    90
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -558
static const short int yypact[] =
{
     697,   -71,  -558,  -558,  -558,  -558,  2749,   -32,  2792,  -558,
    -558,    66,    10,  2888,  2888,    66,    66,  -558,  2191,    15,
      21,    79,   140,   145,  1410,   152,    85,   138,  1607,  2191,
      34,    66,   217,  1410,  1410,    98,  2191,    82,  1711,  1018,
    1807,  -558,  -558,  -558,  -558,  -558,  -558,  -558,   259,  -558,
     230,  -558,  -558,  -558,  -558,  -558,  -558,   366,   592,  2653,
    -558,   272,   -31,   257,   224,   393,   163,   250,   247,   329,
     157,  -558,  -558,  -558,  -558,  -558,    -9,  -558,  -558,  -558,
    -558,  -558,  -558,  -558,  -558,  -558,    66,  -558,   360,   796,
    -558,  -558,    76,  1711,   282,  -558,   776,  1711,  -558,   288,
    2191,  2191,  -558,  -558,   280,  -558,   143,  2191,    68,  1018,
    -558,  2191,  2191,  2191,  2191,   324,  2191,  -558,  -558,  -558,
     168,   359,  1410,   295,    66,   166,   319,   173,   368,  2191,
      66,  -558,   313,  -558,  -558,    82,    82,   418,   330,   353,
    -558,  -558,  -558,  -558,  -558,  -558,  -558,  -558,  -558,  -558,
    -558,  -558,  -558,  -558,  -558,    66,   325,   108,  -558,   616,
      66,   458,   598,   373,   255,   458,    66,  -558,   178,  -558,
    -558,  1116,  1214,   370,  -558,  -558,   -38,   -27,   374,    66,
    1410,    24,    66,  -558,  -558,  1903,  2191,    66,  -558,  -558,
    -558,  -558,  -558,  -558,  -558,  -558,  -558,  -558,  -558,  2191,
    -558,  -558,  2653,  2653,  2653,  2653,  2653,  2653,  2653,  2653,
    2653,  2653,  2653,  2653,  2653,  2653,  2653,  2653,  2653,  2653,
    2191,  2191,  -558,   384,  -558,  -558,   386,   396,   397,   399,
      39,   405,  -558,    66,  -558,   412,   875,   179,   243,   309,
     328,   417,   415,   380,  -558,  1410,  -558,  2191,   421,  -558,
    -558,  -558,  -558,   335,   426,  2191,   429,   443,   451,  -558,
     455,    -1,   339,  -558,   484,  1410,  -558,   637,  -558,  -558,
     465,  -558,  -558,  2653,   226,  2236,    16,   279,  -558,   293,
    -558,   174,  2653,  -558,  -558,  1312,  -558,  -558,  -558,  1999,
    -558,   462,  2095,  -558,  2191,  -558,  -558,    15,   -13,   471,
    -558,  -558,   343,  -558,   346,  -558,  -558,  -558,  -558,  -558,
     272,   272,   -31,   -31,   257,   257,   257,   257,   224,   224,
     393,   163,   250,   247,   329,   423,  -558,    65,  -558,   637,
    -558,  -558,  -558,   433,  1711,  -558,   452,  1881,  -558,  -558,
      66,  -558,  2653,  -558,   897,  -558,  -558,  -558,   616,   458,
    -558,    66,   937,    13,  -558,  -558,  2191,  1410,    15,  1410,
    2191,  2191,  -558,   350,  -558,  -558,  -558,   478,    82,  -558,
    -558,    82,   473,   214,  -558,   445,  -558,  -558,   248,  -558,
     486,   487,   491,  -558,   477,   616,    66,  -558,   458,   496,
    -558,  -558,   492,   490,  -558,   293,  1582,  2332,  -558,  -558,
    -558,  -558,  -558,   374,  -558,  -558,  -558,  -558,  -558,  -558,
      15,  -558,  2191,  -558,  2653,    15,   497,  -558,  -558,    47,
     499,  -558,    26,   502,   504,  1881,  -558,  -558,  -558,   502,
     505,  -558,    26,  -558,  -558,   181,  -558,   306,    81,   355,
     530,  -558,  -558,   369,   488,  -558,  -558,   517,  -558,  -558,
    -558,  -558,  -558,   455,  -558,    82,  -558,    82,    66,  -558,
    -558,  -558,  -558,  -558,  -558,    66,   458,  -558,   616,    66,
    1092,   514,   516,  -558,    22,   458,  -558,  -558,  -558,   519,
    -558,  -558,  -558,    15,  -558,    66,   201,    70,    26,   959,
     522,    26,  -558,   -52,  2375,   306,  -558,  1410,   501,  2191,
    -558,  -558,   525,  -558,  -558,  -558,  -558,   458,  1812,   212,
    -558,    35,  -558,  -558,   314,  -558,  -558,  -558,   534,  -558,
    -558,   544,   540,   439,  -558,   211,   529,  -558,  1508,   548,
     674,  -558,   551,    26,  -558,  -558,   458,  2653,  1881,   554,
     556,  -558,  -558,   568,  -558,  -558,   557,    82,  -558,  -558,
    -558,   560,   314,  1690,  2471,  -558,  -558,  2653,  -558,    72,
    -558,  -558,  2653,  1489,  -558,  -558,   573,  -558,   218,  -558,
    -558,  -558,  -558,    43,   458,   -24,  -558,  1410,  -558,  -558,
    -558,   576,  -558,   584,  -558,  -558,  -558,  1881,  -558,    23,
    -558,    26,  -558,   286,  2514,  -558,  -558,   321,    51,  -558,
    -558,  -558,  -558,  -558,  -558,    26,  -558,   231,  -558,   458,
     595,  -558,   586,   141,  2610,   321,   606,  -558,  -558,  -558,
    -558,   458,   612,  -558,   614,  2653,  -558,   458,  -558,   599,
    -558,  -558
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -558,     6,   358,  -558,  -558,  -558,  -558,  -558,  -558,  -558,
     425,  -558,  -558,   -23,  -558,   453,  -558,  -558,   344,  -558,
    -167,    -6,  -558,   -28,   327,   338,   403,   336,   503,   520,
     518,   521,   526,  -558,   -85,  -558,  -558,     3,   420,  -558,
     376,  -558,  -558,   283,    38,    48,  -558,  -558,  -558,  -394,
    -154,   -35,   392,  -558,  -326,  -558,  -558,   219,   220,  -558,
     125,  -293,  -255,   159,  -557,  -558,  -558,   192,  -206,  -418,
     160,   156,  -153,   142,  -558,   182,   -84,  -558,   594,   206,
    -558,   266,  -558,   307,  -558,  -558,  -163,  -273,   267,   274,
      57,  -558,  -558,   617,  -558,    -4,   618,  -558,  -558,  -558,
    -351,  -558,  -558,  -558,  -558,  -558,  -558,   698
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -342
static const short int yytable[] =
{
      95,   278,    98,   159,   395,   269,    48,   102,   103,   226,
     444,   277,   228,   229,   139,   110,     2,    99,   302,   106,
     495,   104,   105,   140,   141,     2,     2,     2,   486,     2,
      48,   201,   536,   291,   608,    91,   600,   123,     2,    48,
      48,   131,     2,   138,   156,    48,     2,   409,   616,   289,
       2,    96,   290,   200,     2,   537,   108,    77,   159,   372,
     292,   159,   159,   293,   205,   206,   120,   121,     2,     2,
     469,   125,   127,     2,   233,     2,   157,   168,   221,     2,
     363,   115,   422,   537,     2,     2,   373,   133,   134,   374,
     126,   128,   223,   100,   525,    48,   432,   222,   131,   156,
     109,     2,   227,   156,   111,   509,   432,   276,   297,   432,
     276,   275,   256,   257,   260,    48,   276,   276,   509,   433,
     276,   277,   122,   394,   275,   332,   593,   159,    48,   606,
     248,   157,   594,   484,   593,   157,   254,   276,   231,   157,
     594,   138,   138,   138,   469,   235,    77,   436,   546,   415,
     238,   239,   240,   236,   242,   221,   520,   521,   585,   129,
     135,   266,   112,   162,   432,   136,   270,   253,   227,   281,
     488,   137,   105,   504,   307,   308,   309,    48,    48,   246,
     495,   129,   469,   130,   219,   295,    48,   105,   300,   439,
     391,   117,   268,   305,   269,   221,   200,   200,   200,   200,
     200,   200,   200,   200,   200,   200,   200,   200,   200,   200,
     200,   200,   200,   200,   548,     2,   436,     2,   162,   133,
     134,   162,   162,   113,   304,   621,   490,   469,   114,   287,
     233,   269,   385,   333,   533,   116,   105,   296,   395,   335,
     464,   379,   343,   571,   118,   387,   209,   210,   537,   234,
     420,    48,   249,   221,   399,   221,   215,   391,   325,   251,
     221,   233,   220,   355,   282,   221,   356,   200,   491,   200,
     378,    48,   222,   384,   244,   533,   200,   207,   208,   222,
     398,   599,   436,   573,   222,   535,   271,   492,   491,     2,
     280,    48,   135,   408,   385,   509,   388,   136,   491,   159,
     124,   275,   362,   137,   450,   591,   276,   519,   589,   274,
     388,   512,   391,   181,   269,   275,   618,   560,   591,   132,
     276,   511,   383,   178,   592,   211,   212,   357,   626,   481,
     221,   264,   605,   105,   630,   417,   200,   617,   274,   455,
     156,   179,   287,   343,   275,   447,   423,   180,   448,   276,
     343,   217,   453,   216,   441,   218,   511,   429,   343,   434,
     224,   468,   274,    48,   179,    48,   202,   595,   275,   593,
     609,   203,   204,   230,   138,   594,   396,   138,   247,   138,
     276,   138,   397,   182,   183,   184,   232,   490,   241,   493,
     425,   200,   459,   358,   237,   494,   221,   553,   443,   242,
     255,   393,   467,   554,   613,   250,   480,   267,   200,   391,
     614,   482,   359,   264,   440,   221,   442,   213,   214,   365,
     598,     2,   221,   133,   134,   335,   375,   411,   434,   376,
     412,   343,   501,   221,   445,   468,   413,   412,   434,   496,
     598,   265,   412,   391,   434,   479,   221,   245,     2,   185,
     133,   134,   391,   498,   252,   186,   221,   273,   187,   162,
     288,   138,   294,   138,   502,   140,   141,   327,   303,   391,
     328,   503,   391,   468,   428,   505,   467,   391,   329,   517,
     510,   330,   306,   331,  -257,  -257,   361,     2,   200,   133,
     134,   518,   334,   522,   434,   529,   135,   434,   282,   221,
     360,   136,   221,    48,   326,   364,   258,   137,   259,   391,
     221,   414,   366,   460,   467,   510,   368,   510,   468,   418,
     419,   470,   200,   135,   578,   558,   559,   369,   136,   391,
     303,   200,   310,   311,   137,   452,   529,   242,   367,   434,
     391,   370,   543,   371,   343,   312,   313,   386,   200,   318,
     319,   200,   404,   138,   544,   410,   200,   446,   421,   467,
      -5,    -5,   135,   449,    -5,   522,    -5,   136,   377,   343,
     456,    -5,   403,   137,   457,   406,   563,   407,   458,   434,
     462,   483,   463,    48,   276,   572,   485,    -3,   200,   487,
     489,   460,   497,   343,   499,   434,   500,   434,   507,   434,
     515,     2,   583,   508,   434,   584,   534,   545,   200,   516,
     587,   434,   314,   315,   316,   317,   547,   326,   555,   200,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     556,   557,   460,   470,   602,   561,   564,   567,   574,   303,
       2,   577,   612,   575,   579,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   154,   576,   590,
     603,   460,   624,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   629,   604,   526,   620,     2,   470,   619,
     155,   140,   141,   198,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   625,   406,   627,  -341,     1,   460,
       2,     3,     4,     5,   628,   631,     6,     7,     8,   430,
       9,    10,    11,    12,   402,    13,    14,   381,   320,   454,
     427,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   338,   339,   340,   460,   322,   321,    15,    16,   323,
      17,    18,    19,    20,   431,   324,   460,   416,   607,   565,
     566,   586,   460,   610,   615,   622,   272,   601,    21,   581,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,   550,    36,   551,   506,    37,     2,
      38,   528,    39,   140,   141,   552,    40,   225,   284,   285,
      41,    42,    43,    44,    45,    46,  -340,     1,     0,     2,
       3,     4,     5,    47,     0,     6,     7,     8,     0,     9,
      10,    11,    12,     0,    13,    14,     0,     0,     0,     0,
       0,     0,     0,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,    15,    16,     0,    17,
      18,    19,    20,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    21,   155,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,     0,    36,     0,   336,    37,     2,    38,
       0,    39,   140,   141,     0,    40,     0,   337,     0,    41,
      42,    43,    44,    45,    46,     0,     0,     0,   336,     0,
       2,     0,    47,     0,   140,   141,     0,     0,     0,   337,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   338,   339,   340,     0,     0,     0,     0,     0,
       2,     0,     0,     0,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   338,   339,   340,     0,     0,     0,
     526,   341,     2,     0,     0,     0,   140,   141,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   342,   426,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   338,   339,   340,     0,     0,     0,
       0,     0,     0,     0,   342,     0,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   338,   339,   340,     1,
       0,     2,     3,     4,     5,     0,     0,     6,     7,     8,
       0,     9,    10,    11,    12,     0,    13,    14,     0,     0,
       0,     0,     0,     0,     0,   527,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    15,    16,
     166,    17,    18,    19,    20,     0,   528,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    21,
       0,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,     2,    36,     0,     0,    37,
       0,    38,     0,    39,   167,     0,     0,    40,     0,     0,
       0,    41,    42,    43,    44,    45,    46,     1,     0,     2,
       3,     4,     5,     0,    47,     6,     7,     8,     0,     9,
      10,    11,    12,     0,    13,    14,     0,     0,     0,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   338,
     339,   465,     0,     0,     0,     0,    15,    16,   166,    17,
      18,    19,    20,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    21,     0,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,     0,    36,     0,     0,    37,     0,    38,
       0,    39,   283,     0,     0,    40,     0,     0,     0,    41,
      42,    43,    44,    45,    46,     1,     0,     2,     3,     4,
       5,     0,    47,     6,     7,     8,     0,     9,    10,    11,
      12,     0,    13,    14,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    15,    16,     0,    17,    18,    19,
      20,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    21,     0,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,     0,    36,     0,     0,    37,     0,    38,     0,    39,
     286,     0,     0,    40,     0,     0,     0,    41,    42,    43,
      44,    45,    46,     1,     0,     2,     3,     4,     5,     0,
      47,     6,     7,     8,     0,     9,    10,    11,    12,     0,
      13,    14,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    15,    16,     0,    17,    18,    19,    20,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    21,     0,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,     0,
      36,     0,     0,    37,     0,    38,     0,    39,   400,     0,
       0,    40,     0,     0,     0,    41,    42,    43,    44,    45,
      46,     1,     0,     2,     3,     4,     5,     0,    47,     6,
       7,     8,     0,     9,    10,    11,    12,     0,    13,    14,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      15,    16,     0,    17,    18,    19,    20,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    21,     0,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,     0,    36,     0,
       0,    37,     2,    38,     0,    39,   140,   141,     0,    40,
       0,     0,     0,    41,    42,    43,    44,    45,    46,     0,
       0,     2,     3,     4,     5,     0,    47,     6,     7,     8,
       0,     0,     0,    11,    12,     0,    13,    14,     0,     0,
       0,     0,     0,     0,     0,     0,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   338,   339,   340,     0,
       0,    17,     0,    19,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      31,    32,    33,    34,    92,     2,    36,     0,     0,   140,
     141,    38,     0,   107,     0,   588,     0,    40,     0,     0,
       0,    41,    42,    43,    44,    45,    46,     0,     0,     0,
       2,     3,     4,     5,     0,   562,     6,     7,     8,     0,
       0,     0,    11,    12,     0,    13,    14,     0,     0,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   338,
     339,   465,     0,     0,     0,     0,     0,     0,     0,     0,
      17,    18,    19,    20,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   466,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    31,
      32,    33,    34,    92,     0,    36,     0,     0,     0,     0,
      38,     0,   107,     2,     0,     0,    40,   140,   141,     0,
      41,    42,    43,    44,    45,    46,     0,     0,     0,     0,
       0,     0,     0,   119,     2,     3,     4,     5,   140,   141,
       6,     7,     8,     0,     0,     0,    11,    12,     0,    13,
      14,     0,     0,     0,     0,     0,     0,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   338,   339,   465,
       0,     0,     0,     0,    17,    18,    19,    20,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,     0,     0,     0,   580,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    32,    33,    34,    92,     0,    36,
       0,     0,     0,   155,    38,     0,   107,     0,     0,     0,
      40,     0,     0,     0,    41,    42,    43,    44,    45,    46,
       2,     3,     4,     5,     0,     2,     6,     7,     8,   140,
     141,     0,    11,    12,     0,    13,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      17,    18,    19,    20,     0,     0,     0,     0,     0,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   338,
     339,   465,   549,     0,     0,     0,     0,     0,     0,    31,
      32,    33,    34,    92,     2,    36,     0,     0,   140,   141,
      38,     0,   107,     0,     0,   173,    40,   174,     0,     0,
      41,    42,    43,    44,    45,    46,     2,     3,     4,     5,
       0,     0,     6,     7,     8,     0,     0,     0,    11,    12,
       0,    13,    14,     0,     0,     0,     0,     0,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   338,   339,
     340,     0,     0,     0,     0,     0,    17,    18,    19,    20,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    31,    32,    33,    34,    92,
       0,    36,     0,     0,     0,     0,    38,   301,   107,     0,
       0,     0,    40,     0,     0,     0,    41,    42,    43,    44,
      45,    46,     2,     3,     4,     5,     0,     0,     6,     7,
       8,     0,     0,     0,    11,    12,     0,    13,    14,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    17,    18,    19,    20,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    31,    32,    33,    34,    92,     0,    36,     0,     0,
       0,     0,    38,     0,   107,     0,     0,     0,    40,   401,
       0,     0,    41,    42,    43,    44,    45,    46,     2,     3,
       4,     5,     0,     0,     6,     7,     8,     0,     0,     0,
      11,    12,     0,    13,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    17,    18,
      19,    20,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    31,    32,    33,
      34,    92,     0,    36,     0,     0,     0,     0,    38,     0,
     107,     0,     0,     0,    40,   405,     0,     0,    41,    42,
      43,    44,    45,    46,     2,     3,     4,     5,     0,     0,
       6,     7,     8,     0,     0,     0,    11,    12,     0,    13,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    17,    18,    19,    20,     0,     2,
       3,     4,     5,     0,     0,     6,     7,     8,     0,     0,
       0,    11,    12,     0,    13,    14,     0,     0,     0,     0,
       0,     0,     0,    31,    32,    33,    34,    92,     0,    36,
       0,     0,     0,     0,    38,     0,   107,     0,     0,    17,
      40,    19,     0,     0,    41,    42,    43,    44,    45,    46,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    31,    32,
      33,    34,    92,     0,    36,     0,     0,     0,     0,    38,
       0,   107,     0,     0,     0,    40,   390,     0,     0,    41,
      42,    43,    44,    45,    46,     2,     3,     4,     5,     0,
       0,     6,     7,     8,     0,     0,     0,    11,    12,     0,
      13,    14,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    17,     0,    19,     2,     3,
       4,     5,     0,     0,     6,     7,     8,     0,     0,     0,
      11,    12,     0,    13,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    31,    32,    33,    34,    92,     0,
      36,     0,     0,     0,     0,    38,     0,   107,    17,     0,
      19,    40,   478,     0,     0,    41,    42,    43,    44,    45,
      46,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    31,    32,    33,
      34,    92,     0,    36,     0,     0,     0,     0,    38,     0,
     107,     0,     0,     0,    40,   542,     0,     0,    41,    42,
      43,    44,    45,    46,     2,     3,     4,     5,     0,     0,
       6,     7,     8,     0,     0,     0,    11,    12,     0,    13,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    17,     0,    19,     2,     3,     4,
       5,     0,     0,     6,     7,     8,     0,     0,     0,    11,
      12,     0,    13,    14,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    32,    33,    34,    92,     0,    36,
       0,     0,     0,     0,    38,     0,   107,    17,     0,    19,
      40,   582,     0,     0,    41,    42,    43,    44,    45,    46,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    31,    32,    33,    34,
      92,     0,    36,     0,     0,     0,     0,    38,     0,   107,
       0,     0,     0,    40,   611,     0,     0,    41,    42,    43,
      44,    45,    46,     2,     3,     4,     5,     0,     0,     6,
       7,     8,     0,     0,     0,    11,    12,     0,    13,    14,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    17,     0,    19,     2,     3,     4,     5,
       0,     0,     6,     7,     8,     0,     0,     0,    11,    12,
       0,    13,    14,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    31,    32,    33,    34,    92,     0,    36,     0,
       0,     0,     0,    38,     0,   107,    17,     0,    19,    40,
     623,     0,     0,    41,    42,    43,    44,    45,    46,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    31,    32,    33,    34,    92,
       0,    36,     0,     0,     0,     0,    38,     0,   107,     0,
       0,     0,    40,     0,     0,     0,    41,    42,    43,    44,
      45,    46,     2,     3,     4,     5,     0,     0,     6,     7,
       8,     0,     0,     0,    11,    12,     0,    13,    14,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    17,     0,    19,     2,     3,     4,     5,     0,
       0,     6,     7,     8,     0,     0,     0,    11,    12,     0,
      13,    14,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    31,    32,    33,    34,    92,     0,    36,     0,     0,
       0,     0,    93,     0,     0,    17,     0,    19,    40,     0,
       0,     0,    41,    42,    43,    44,    45,    46,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    31,    32,    33,    34,    92,     0,
      36,     0,     0,     0,     0,    97,     0,     0,     0,     0,
       0,    40,     0,     0,     0,    41,    42,    43,    44,    45,
      46,     2,     3,     4,     5,     0,     0,     6,     7,     8,
       0,     0,     0,    11,    12,     0,    13,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    17,     0,    19,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      31,    32,    33,    34,    92,     0,    36,     0,     0,     0,
       0,   101,     0,     0,     0,     0,     0,    40,     0,     0,
       0,    41,    42,    43,    44,    45,    46
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       1,     3,     0,     0,     5,     0,     7,     0,     0,     0,
       0,     9,     0,     0,     0,     0,     0,     0,     0,     0,
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
       0,     0,     0,     0,     0,     0,     0
};

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short int yyconfl[] =
{
       0,   257,     0,   257,     0,   257,     0,   257,     0,   257,
       0,   319,     0
};

static const short int yycheck[] =
{
       6,   164,     8,    38,   277,   159,     0,    13,    14,    93,
     361,   164,    96,    97,    37,    19,     3,    11,   185,    16,
     438,    15,    16,     7,     8,     3,     3,     3,   422,     3,
      24,    59,    84,    60,   591,   106,    60,    31,     3,    33,
      34,    35,     3,    37,    38,    39,     3,    60,   605,    87,
       3,    83,    90,    59,     3,   107,    18,     0,    93,    60,
      87,    96,    97,    90,    95,    96,    28,    29,     3,     3,
     396,    33,    34,     3,    87,     3,    38,    39,    87,     3,
     247,    24,   337,   107,     3,     3,    87,     5,     6,    90,
      33,    34,    86,    83,   488,    89,    83,   106,    92,    93,
      85,     3,    96,    97,    83,    83,    83,    94,    84,    83,
      94,    89,   135,   136,   137,   109,    94,    94,    83,   106,
      94,   274,    88,   276,    89,    86,    83,   162,   122,   106,
     124,    93,    89,    86,    83,    97,   130,    94,   100,   101,
      89,   135,   136,   137,   470,   107,    89,   353,   499,    84,
     112,   113,   114,    85,   116,    87,    86,    87,    86,    83,
      78,   155,    83,    38,    83,    83,   160,   129,   162,   166,
     425,    89,   166,   466,   202,   203,   204,   171,   172,   122,
     598,    83,   508,    85,    27,   179,   180,   181,   182,   356,
     275,   106,    84,   187,   348,    87,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   507,     3,   422,     3,    93,     5,
       6,    96,    97,    83,   186,    84,   432,   553,    83,   172,
      87,   385,   267,   230,   489,    83,   230,   180,   511,   233,
     393,   264,   236,   536,   106,   273,    22,    23,   107,   106,
     334,   245,    86,    87,   282,    87,    93,   342,   220,    86,
      87,    87,   105,    84,    86,    87,    87,   273,    87,   275,
     264,   265,   106,   267,   106,   530,   282,    20,    21,   106,
     106,   574,   488,   538,   106,   491,   161,   106,    87,     3,
     165,   285,    78,   297,   329,    83,    84,    83,    87,   334,
      83,    89,   245,    89,    90,    87,    94,   106,   563,    83,
      84,   474,   397,    83,   468,    89,   609,   106,    87,    36,
      94,   474,   265,    40,   106,   101,   102,    84,   621,   414,
      87,    83,   587,   327,   627,   329,   342,   106,    83,    91,
     334,    82,   285,   337,    89,   368,   340,    88,   371,    94,
     344,   104,   375,   103,   358,    26,   509,   351,   352,   353,
       0,   396,    83,   357,    82,   359,    94,   573,    89,    83,
      84,    99,   100,    85,   368,    89,    83,   371,    83,   373,
      94,   375,    89,    17,    18,    19,   106,   593,    64,    83,
     342,   397,   386,    84,   111,    89,    87,    83,   360,   361,
      87,   276,   396,    89,    83,    86,   410,    82,   414,   494,
      89,   415,    84,    83,   357,    87,   359,    24,    25,    84,
     573,     3,    87,     5,     6,   419,    87,    84,   422,    90,
      87,   425,   455,    87,    84,   470,    90,    87,   432,    84,
     593,    88,    87,   528,   438,   397,    87,    88,     3,    83,
       5,     6,   537,    84,    86,    89,    87,    84,    92,   334,
      90,   455,    88,   457,   458,     7,     8,    83,   185,   554,
      84,   465,   557,   508,   349,   469,   470,   562,    82,   483,
     474,    84,   199,    84,     7,     8,   106,     3,   494,     5,
       6,   485,    87,   487,   488,   489,    78,   491,    86,    87,
      83,    83,    87,   497,   221,    84,    88,    89,    90,   594,
      87,    88,    86,   388,   508,   509,    87,   511,   553,    86,
      87,   396,   528,    78,   547,    86,    87,    84,    83,   614,
     247,   537,   205,   206,    89,    90,   530,   499,   255,   533,
     625,    90,   494,    88,   538,   207,   208,    82,   554,   213,
     214,   557,    90,   547,   497,    84,   562,    79,   106,   553,
      83,    84,    78,    90,    87,   559,    89,    83,    84,   563,
      84,    94,   289,    89,    87,   292,   528,   294,    87,   573,
      84,    84,    90,   577,    94,   537,    87,    85,   594,    85,
      85,   466,    62,   587,   106,   589,    79,   591,    84,   593,
     475,     3,   554,    87,   598,   557,    84,   106,   614,    90,
     562,   605,   209,   210,   211,   212,    91,   334,    84,   625,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      86,    91,   507,   508,   577,   106,    88,    86,    84,   356,
       3,    84,   594,    87,    84,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    90,    86,
      84,   536,   614,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,   625,    90,     1,    90,     3,   553,    84,
      82,     7,     8,    91,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    88,   412,    84,     0,     1,   574,
       3,     4,     5,     6,    90,   106,     9,    10,    11,   351,
      13,    14,    15,    16,   289,    18,    19,   264,   215,   375,
     344,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,   609,   217,   216,    40,    41,   218,
      43,    44,    45,    46,   352,   219,   621,   327,   589,   530,
     530,   559,   627,   593,   598,   613,   162,   575,    61,   553,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,   508,    78,   509,   470,    81,     3,
      83,   107,    85,     7,     8,   511,    89,    89,   171,   171,
      93,    94,    95,    96,    97,    98,     0,     1,    -1,     3,
       4,     5,     6,   106,    -1,     9,    10,    11,    -1,    13,
      14,    15,    16,    -1,    18,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    40,    41,    -1,    43,
      44,    45,    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    82,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    -1,    78,    -1,     1,    81,     3,    83,
      -1,    85,     7,     8,    -1,    89,    -1,    12,    -1,    93,
      94,    95,    96,    97,    98,    -1,    -1,    -1,     1,    -1,
       3,    -1,   106,    -1,     7,     8,    -1,    -1,    -1,    12,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    -1,    -1,    -1,
       3,    -1,    -1,    -1,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    -1,
       1,    86,     3,    -1,    -1,    -1,     7,     8,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   107,    86,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   107,    -1,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,     1,
      -1,     3,     4,     5,     6,    -1,    -1,     9,    10,    11,
      -1,    13,    14,    15,    16,    -1,    18,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,    41,
      42,    43,    44,    45,    46,    -1,   107,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      -1,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,     3,    78,    -1,    -1,    81,
      -1,    83,    -1,    85,    86,    -1,    -1,    89,    -1,    -1,
      -1,    93,    94,    95,    96,    97,    98,     1,    -1,     3,
       4,     5,     6,    -1,   106,     9,    10,    11,    -1,    13,
      14,    15,    16,    -1,    18,    19,    -1,    -1,    -1,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    40,    41,    42,    43,
      44,    45,    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    -1,    78,    -1,    -1,    81,    -1,    83,
      -1,    85,    86,    -1,    -1,    89,    -1,    -1,    -1,    93,
      94,    95,    96,    97,    98,     1,    -1,     3,     4,     5,
       6,    -1,   106,     9,    10,    11,    -1,    13,    14,    15,
      16,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    40,    41,    -1,    43,    44,    45,
      46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    -1,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    -1,    78,    -1,    -1,    81,    -1,    83,    -1,    85,
      86,    -1,    -1,    89,    -1,    -1,    -1,    93,    94,    95,
      96,    97,    98,     1,    -1,     3,     4,     5,     6,    -1,
     106,     9,    10,    11,    -1,    13,    14,    15,    16,    -1,
      18,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    40,    41,    -1,    43,    44,    45,    46,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    61,    -1,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    -1,
      78,    -1,    -1,    81,    -1,    83,    -1,    85,    86,    -1,
      -1,    89,    -1,    -1,    -1,    93,    94,    95,    96,    97,
      98,     1,    -1,     3,     4,     5,     6,    -1,   106,     9,
      10,    11,    -1,    13,    14,    15,    16,    -1,    18,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      40,    41,    -1,    43,    44,    45,    46,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    61,    -1,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    -1,    78,    -1,
      -1,    81,     3,    83,    -1,    85,     7,     8,    -1,    89,
      -1,    -1,    -1,    93,    94,    95,    96,    97,    98,    -1,
      -1,     3,     4,     5,     6,    -1,   106,     9,    10,    11,
      -1,    -1,    -1,    15,    16,    -1,    18,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    -1,
      -1,    43,    -1,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      72,    73,    74,    75,    76,     3,    78,    -1,    -1,     7,
       8,    83,    -1,    85,    -1,   106,    -1,    89,    -1,    -1,
      -1,    93,    94,    95,    96,    97,    98,    -1,    -1,    -1,
       3,     4,     5,     6,    -1,   107,     9,    10,    11,    -1,
      -1,    -1,    15,    16,    -1,    18,    19,    -1,    -1,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    44,    45,    46,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    72,
      73,    74,    75,    76,    -1,    78,    -1,    -1,    -1,    -1,
      83,    -1,    85,     3,    -1,    -1,    89,     7,     8,    -1,
      93,    94,    95,    96,    97,    98,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   106,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    -1,    -1,    -1,    15,    16,    -1,    18,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      -1,    -1,    -1,    -1,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    -1,    -1,    -1,    84,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    72,    73,    74,    75,    76,    -1,    78,
      -1,    -1,    -1,    82,    83,    -1,    85,    -1,    -1,    -1,
      89,    -1,    -1,    -1,    93,    94,    95,    96,    97,    98,
       3,     4,     5,     6,    -1,     3,     9,    10,    11,     7,
       8,    -1,    15,    16,    -1,    18,    19,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    44,    45,    46,    -1,    -1,    -1,    -1,    -1,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    -1,    -1,    -1,    -1,    -1,    -1,    72,
      73,    74,    75,    76,     3,    78,    -1,    -1,     7,     8,
      83,    -1,    85,    -1,    -1,    88,    89,    90,    -1,    -1,
      93,    94,    95,    96,    97,    98,     3,     4,     5,     6,
      -1,    -1,     9,    10,    11,    -1,    -1,    -1,    15,    16,
      -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    -1,    -1,    -1,    -1,    -1,    43,    44,    45,    46,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    72,    73,    74,    75,    76,
      -1,    78,    -1,    -1,    -1,    -1,    83,    84,    85,    -1,
      -1,    -1,    89,    -1,    -1,    -1,    93,    94,    95,    96,
      97,    98,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    -1,    -1,    -1,    15,    16,    -1,    18,    19,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    43,    44,    45,    46,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    72,    73,    74,    75,    76,    -1,    78,    -1,    -1,
      -1,    -1,    83,    -1,    85,    -1,    -1,    -1,    89,    90,
      -1,    -1,    93,    94,    95,    96,    97,    98,     3,     4,
       5,     6,    -1,    -1,     9,    10,    11,    -1,    -1,    -1,
      15,    16,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,
      45,    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    72,    73,    74,
      75,    76,    -1,    78,    -1,    -1,    -1,    -1,    83,    -1,
      85,    -1,    -1,    -1,    89,    90,    -1,    -1,    93,    94,
      95,    96,    97,    98,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    -1,    -1,    -1,    15,    16,    -1,    18,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    43,    44,    45,    46,    -1,     3,
       4,     5,     6,    -1,    -1,     9,    10,    11,    -1,    -1,
      -1,    15,    16,    -1,    18,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    72,    73,    74,    75,    76,    -1,    78,
      -1,    -1,    -1,    -1,    83,    -1,    85,    -1,    -1,    43,
      89,    45,    -1,    -1,    93,    94,    95,    96,    97,    98,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    72,    73,
      74,    75,    76,    -1,    78,    -1,    -1,    -1,    -1,    83,
      -1,    85,    -1,    -1,    -1,    89,    90,    -1,    -1,    93,
      94,    95,    96,    97,    98,     3,     4,     5,     6,    -1,
      -1,     9,    10,    11,    -1,    -1,    -1,    15,    16,    -1,
      18,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    43,    -1,    45,     3,     4,
       5,     6,    -1,    -1,     9,    10,    11,    -1,    -1,    -1,
      15,    16,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    72,    73,    74,    75,    76,    -1,
      78,    -1,    -1,    -1,    -1,    83,    -1,    85,    43,    -1,
      45,    89,    90,    -1,    -1,    93,    94,    95,    96,    97,
      98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    72,    73,    74,
      75,    76,    -1,    78,    -1,    -1,    -1,    -1,    83,    -1,
      85,    -1,    -1,    -1,    89,    90,    -1,    -1,    93,    94,
      95,    96,    97,    98,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    -1,    -1,    -1,    15,    16,    -1,    18,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    43,    -1,    45,     3,     4,     5,
       6,    -1,    -1,     9,    10,    11,    -1,    -1,    -1,    15,
      16,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    72,    73,    74,    75,    76,    -1,    78,
      -1,    -1,    -1,    -1,    83,    -1,    85,    43,    -1,    45,
      89,    90,    -1,    -1,    93,    94,    95,    96,    97,    98,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    72,    73,    74,    75,
      76,    -1,    78,    -1,    -1,    -1,    -1,    83,    -1,    85,
      -1,    -1,    -1,    89,    90,    -1,    -1,    93,    94,    95,
      96,    97,    98,     3,     4,     5,     6,    -1,    -1,     9,
      10,    11,    -1,    -1,    -1,    15,    16,    -1,    18,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    -1,    45,     3,     4,     5,     6,
      -1,    -1,     9,    10,    11,    -1,    -1,    -1,    15,    16,
      -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    72,    73,    74,    75,    76,    -1,    78,    -1,
      -1,    -1,    -1,    83,    -1,    85,    43,    -1,    45,    89,
      90,    -1,    -1,    93,    94,    95,    96,    97,    98,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    72,    73,    74,    75,    76,
      -1,    78,    -1,    -1,    -1,    -1,    83,    -1,    85,    -1,
      -1,    -1,    89,    -1,    -1,    -1,    93,    94,    95,    96,
      97,    98,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    -1,    -1,    -1,    15,    16,    -1,    18,    19,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    43,    -1,    45,     3,     4,     5,     6,    -1,
      -1,     9,    10,    11,    -1,    -1,    -1,    15,    16,    -1,
      18,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    72,    73,    74,    75,    76,    -1,    78,    -1,    -1,
      -1,    -1,    83,    -1,    -1,    43,    -1,    45,    89,    -1,
      -1,    -1,    93,    94,    95,    96,    97,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    72,    73,    74,    75,    76,    -1,
      78,    -1,    -1,    -1,    -1,    83,    -1,    -1,    -1,    -1,
      -1,    89,    -1,    -1,    -1,    93,    94,    95,    96,    97,
      98,     3,     4,     5,     6,    -1,    -1,     9,    10,    11,
      -1,    -1,    -1,    15,    16,    -1,    18,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    -1,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      72,    73,    74,    75,    76,    -1,    78,    -1,    -1,    -1,
      -1,    83,    -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,
      -1,    93,    94,    95,    96,    97,    98
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     1,     3,     4,     5,     6,     9,    10,    11,    13,
      14,    15,    16,    18,    19,    40,    41,    43,    44,    45,
      46,    61,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    78,    81,    83,    85,
      89,    93,    94,    95,    96,    97,    98,   106,   109,   111,
     112,   113,   114,   115,   116,   117,   120,   127,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   149,   150,   151,   152,   198,   199,   200,
     203,   205,   206,   207,   209,   210,   211,   212,   213,   214,
     215,   106,    76,    83,   109,   129,    83,    83,   129,   109,
      83,    83,   129,   129,   109,   109,   145,    85,   152,    85,
     203,    83,    83,    83,    83,   198,    83,   106,   106,   106,
     152,   152,    88,   109,    83,   152,   198,   152,   198,    83,
      85,   109,   151,     5,     6,    78,    83,    89,   109,   121,
       7,     8,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    82,   109,   152,   158,   159,
     163,   167,   168,   184,   185,   186,    42,    86,   152,   198,
     201,   202,   204,    88,    90,   118,   119,   128,   151,    82,
      88,    83,    17,    18,    19,    83,    89,    92,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    91,   144,
     129,   131,    94,    99,   100,    95,    96,    20,    21,    22,
      23,   101,   102,    24,    25,    93,   103,   104,    26,    27,
     105,    87,   106,   109,     0,   215,   184,   109,   184,   184,
      85,   152,   106,    87,   106,   152,    85,   151,   152,   152,
     152,    64,   152,   208,   106,    88,   198,    83,   109,    86,
      86,    86,    86,   152,   109,    87,   121,   121,    88,    90,
     121,   123,   125,   126,    83,    88,   109,    82,    84,   158,
     109,   168,   186,    84,    83,    89,    94,   180,   194,   195,
     168,   145,    86,    86,   201,   204,    86,   198,    90,    87,
      90,    60,    87,    90,    88,   109,   198,    84,   145,   146,
     109,    84,   128,   151,   152,   109,   151,   131,   131,   131,
     132,   132,   133,   133,   134,   134,   134,   134,   135,   135,
     136,   137,   138,   139,   140,   152,   151,    83,    84,    82,
      84,    84,    86,   145,    87,   109,     1,    12,    57,    58,
      59,    86,   107,   109,   147,   148,   154,   156,   159,   160,
     161,   162,   168,   170,   173,    84,    87,    84,    84,    84,
      83,   106,   198,   128,    84,    84,    86,   151,    87,    84,
      90,    88,    60,    87,    90,    87,    90,    84,   109,   121,
     122,   123,   124,   198,   109,   159,    82,   131,    84,   194,
      90,   142,   153,   168,   180,   195,    83,    89,   106,   131,
      86,    90,   118,   151,    90,    90,   151,   151,   203,    60,
      84,    84,    87,    90,    88,    84,   146,   109,    86,    87,
     184,   106,   170,   109,   110,   153,    86,   148,   168,   109,
     110,   160,    83,   106,   109,   157,   176,   177,   180,   128,
     198,   203,   198,   152,   208,    84,    79,   121,   121,    90,
      90,   121,    90,   121,   126,    91,    84,    87,    87,   109,
     168,   169,    84,    90,   180,    59,    84,   109,   159,   162,
     168,   187,   188,   189,   190,   191,   192,   193,    90,   153,
     203,   142,   203,    84,    86,    87,   157,    85,   170,    85,
     176,    87,   106,    83,    89,   177,    84,    62,    84,   106,
      79,   121,   109,   109,   169,   109,   191,    84,    87,    83,
     109,   180,   194,   196,   197,   168,    90,   203,   109,   106,
      86,    87,   109,   174,   175,   157,     1,    86,   107,   109,
     164,   165,   166,   170,    84,   176,    84,   107,   155,   181,
     182,   183,    90,   153,   198,   106,   208,    91,   169,    60,
     189,   196,   197,    83,    89,    84,    86,    91,    86,    87,
     106,   106,   107,   153,    88,   165,   166,    86,   171,   172,
     176,   169,   153,   170,    84,    87,    90,    84,   121,    84,
      84,   187,    90,   153,   153,    86,   175,   153,   106,   170,
      86,    87,   106,    83,    89,   176,   178,   179,   180,   169,
      60,   183,   198,    84,    90,   170,   106,   171,   172,    84,
     178,    90,   153,    83,    89,   179,   172,   106,   169,    84,
      90,    84,   181,    90,   153,    88,   169,    84,    90,   153,
     169,   106
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
#line 101 "c.y"
    { ((*yyvalp).expr) = 0 ;}
    break;

  case 5:

/* Line 936 of glr.c  */
#line 106 "c.y"
    { ((*yyvalp).expr) = dotickesrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 7:

/* Line 936 of glr.c  */
#line 115 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elambda, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 8:

/* Line 936 of glr.c  */
#line 117 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elambda, nullelist(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 9:

/* Line 936 of glr.c  */
#line 122 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefrec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 10:

/* Line 936 of glr.c  */
#line 124 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefrec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 11:

/* Line 936 of glr.c  */
#line 126 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefrec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 12:

/* Line 936 of glr.c  */
#line 131 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 13:

/* Line 936 of glr.c  */
#line 136 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxquote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 14:

/* Line 936 of glr.c  */
#line 138 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxquote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 15:

/* Line 936 of glr.c  */
#line 140 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxquasi, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 16:

/* Line 936 of glr.c  */
#line 142 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxquasi, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 17:

/* Line 936 of glr.c  */
#line 144 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxunquote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 18:

/* Line 936 of glr.c  */
#line 146 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxunquote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 19:

/* Line 936 of glr.c  */
#line 151 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estx, doid("id"), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 20:

/* Line 936 of glr.c  */
#line 153 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 21:

/* Line 936 of glr.c  */
#line 158 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, Eelist,
				     (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0),
			  0, 0);
	;}
    break;

  case 22:

/* Line 936 of glr.c  */
#line 168 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 23:

/* Line 936 of glr.c  */
#line 170 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 26:

/* Line 936 of glr.c  */
#line 177 "c.y"
    { ((*yyvalp).expr) = dosym((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 27:

/* Line 936 of glr.c  */
#line 179 "c.y"
    { ((*yyvalp).expr) = doconst(ctx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 28:

/* Line 936 of glr.c  */
#line 181 "c.y"
    { ((*yyvalp).expr) = dostr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 29:

/* Line 936 of glr.c  */
#line 183 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 30:

/* Line 936 of glr.c  */
#line 185 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epair, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 31:

/* Line 936 of glr.c  */
#line 187 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, nullelist(), 0, 0, 0); ;}
    break;

  case 32:

/* Line 936 of glr.c  */
#line 189 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 33:

/* Line 936 of glr.c  */
#line 191 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 34:

/* Line 936 of glr.c  */
#line 193 "c.y"
    { Expr *ell = newexprsrc(&ctx->inp->src, Eellipsis, 0, 0, 0, 0);
          ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist,
                          invert(newexprsrc(&ctx->inp->src, Eelist,
                                            ell, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), 0, 0)), 0, 0, 0); ;}
    break;

  case 35:

/* Line 936 of glr.c  */
#line 198 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, nullelist(), 0, 0, 0); ;}
    break;

  case 36:

/* Line 936 of glr.c  */
#line 200 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 37:

/* Line 936 of glr.c  */
#line 202 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 44:

/* Line 936 of glr.c  */
#line 213 "c.y"
    { ((*yyvalp).expr) = doconst(ctx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 45:

/* Line 936 of glr.c  */
#line 215 "c.y"
    { ((*yyvalp).expr) = dostr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 46:

/* Line 936 of glr.c  */
#line 217 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 47:

/* Line 936 of glr.c  */
#line 219 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epair, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 48:

/* Line 936 of glr.c  */
#line 221 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, nullelist(), 0, 0, 0); ;}
    break;

  case 49:

/* Line 936 of glr.c  */
#line 223 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 50:

/* Line 936 of glr.c  */
#line 225 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 51:

/* Line 936 of glr.c  */
#line 227 "c.y"
    { Expr *ell = newexprsrc(&ctx->inp->src, Eellipsis, 0, 0, 0, 0);
          ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist,
                          invert(newexprsrc(&ctx->inp->src, Eelist,
                                            ell, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), 0, 0)), 0, 0, 0); ;}
    break;

  case 52:

/* Line 936 of glr.c  */
#line 232 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, nullelist(), 0, 0, 0); ;}
    break;

  case 53:

/* Line 936 of glr.c  */
#line 234 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 54:

/* Line 936 of glr.c  */
#line 236 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 55:

/* Line 936 of glr.c  */
#line 238 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 56:

/* Line 936 of glr.c  */
#line 240 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 59:

/* Line 936 of glr.c  */
#line 250 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 60:

/* Line 936 of glr.c  */
#line 252 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 61:

/* Line 936 of glr.c  */
#line 257 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, 
                          newbinopsrc(&ctx->inp->src, Eeq, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)),
                          nullelist(), 
                          0, 0); ;}
    break;

  case 62:

/* Line 936 of glr.c  */
#line 262 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, 
                          newbinopsrc(&ctx->inp->src, Eeq, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr)),
                          (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), 
                          0, 0); ;}
    break;

  case 63:

/* Line 936 of glr.c  */
#line 270 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 64:

/* Line 936 of glr.c  */
#line 272 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 65:

/* Line 936 of glr.c  */
#line 277 "c.y"
    { /* key may not actually bind variables, but this will get
             caught during compilation. */
          ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, Eelist,
				     (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0),
			  0, 0);
	;}
    break;

  case 67:

/* Line 936 of glr.c  */
#line 290 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 68:

/* Line 936 of glr.c  */
#line 292 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 69:

/* Line 936 of glr.c  */
#line 294 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 70:

/* Line 936 of glr.c  */
#line 296 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edot, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 71:

/* Line 936 of glr.c  */
#line 298 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earrow, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 72:

/* Line 936 of glr.c  */
#line 300 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epostinc, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 73:

/* Line 936 of glr.c  */
#line 302 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epostdec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 74:

/* Line 936 of glr.c  */
#line 304 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Econtainer, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 75:

/* Line 936 of glr.c  */
#line 309 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 76:

/* Line 936 of glr.c  */
#line 311 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 78:

/* Line 936 of glr.c  */
#line 317 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epreinc, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 79:

/* Line 936 of glr.c  */
#line 319 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epredec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 80:

/* Line 936 of glr.c  */
#line 321 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 81:

/* Line 936 of glr.c  */
#line 323 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Esizeofe, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 82:

/* Line 936 of glr.c  */
#line 325 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Esizeoft, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 83:

/* Line 936 of glr.c  */
#line 327 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypeofe, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 84:

/* Line 936 of glr.c  */
#line 329 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypeoft, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 85:

/* Line 936 of glr.c  */
#line 331 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Emkctype, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 86:

/* Line 936 of glr.c  */
#line 336 "c.y"
    { ((*yyvalp).kind) = Eref; ;}
    break;

  case 87:

/* Line 936 of glr.c  */
#line 338 "c.y"
    { ((*yyvalp).kind) = Ederef; ;}
    break;

  case 88:

/* Line 936 of glr.c  */
#line 340 "c.y"
    { ((*yyvalp).kind) = Euplus; ;}
    break;

  case 89:

/* Line 936 of glr.c  */
#line 342 "c.y"
    { ((*yyvalp).kind) = Euminus; ;}
    break;

  case 90:

/* Line 936 of glr.c  */
#line 344 "c.y"
    { ((*yyvalp).kind) = Eutwiddle; ;}
    break;

  case 91:

/* Line 936 of glr.c  */
#line 346 "c.y"
    { ((*yyvalp).kind) = Eunot; ;}
    break;

  case 93:

/* Line 936 of glr.c  */
#line 352 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecast, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 94:

/* Line 936 of glr.c  */
#line 354 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Excast, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 96:

/* Line 936 of glr.c  */
#line 360 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Emul, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 97:

/* Line 936 of glr.c  */
#line 362 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ediv, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 98:

/* Line 936 of glr.c  */
#line 364 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Emod, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 100:

/* Line 936 of glr.c  */
#line 370 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eadd, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 101:

/* Line 936 of glr.c  */
#line 372 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Esub, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 103:

/* Line 936 of glr.c  */
#line 378 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eshl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 104:

/* Line 936 of glr.c  */
#line 380 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eshr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 106:

/* Line 936 of glr.c  */
#line 386 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Elt, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 107:

/* Line 936 of glr.c  */
#line 388 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Egt, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 108:

/* Line 936 of glr.c  */
#line 390 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ele, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 109:

/* Line 936 of glr.c  */
#line 392 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ege, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 111:

/* Line 936 of glr.c  */
#line 398 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eeq, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 112:

/* Line 936 of glr.c  */
#line 400 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eneq, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 114:

/* Line 936 of glr.c  */
#line 406 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eband, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 116:

/* Line 936 of glr.c  */
#line 412 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ebxor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 118:

/* Line 936 of glr.c  */
#line 418 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ebor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 120:

/* Line 936 of glr.c  */
#line 424 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eland, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 122:

/* Line 936 of glr.c  */
#line 430 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 124:

/* Line 936 of glr.c  */
#line 436 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Econd, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 126:

/* Line 936 of glr.c  */
#line 442 "c.y"
    { if((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.kind) == Eg)
	  	((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0);
	  else
	  	((*yyvalp).expr) = newgopsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr));
	;}
    break;

  case 127:

/* Line 936 of glr.c  */
#line 451 "c.y"
    { ((*yyvalp).kind) = Eg; ;}
    break;

  case 128:

/* Line 936 of glr.c  */
#line 453 "c.y"
    { ((*yyvalp).kind) = Egadd; ;}
    break;

  case 129:

/* Line 936 of glr.c  */
#line 455 "c.y"
    { ((*yyvalp).kind) = Egband; ;}
    break;

  case 130:

/* Line 936 of glr.c  */
#line 457 "c.y"
    { ((*yyvalp).kind) = Egbor; ;}
    break;

  case 131:

/* Line 936 of glr.c  */
#line 459 "c.y"
    { ((*yyvalp).kind) = Egbxor; ;}
    break;

  case 132:

/* Line 936 of glr.c  */
#line 461 "c.y"
    { ((*yyvalp).kind) = Egdiv; ;}
    break;

  case 133:

/* Line 936 of glr.c  */
#line 463 "c.y"
    { ((*yyvalp).kind) = Egmod; ;}
    break;

  case 134:

/* Line 936 of glr.c  */
#line 465 "c.y"
    { ((*yyvalp).kind) = Egmul; ;}
    break;

  case 135:

/* Line 936 of glr.c  */
#line 467 "c.y"
    { ((*yyvalp).kind) = Egshl; ;}
    break;

  case 136:

/* Line 936 of glr.c  */
#line 469 "c.y"
    { ((*yyvalp).kind) = Egshr; ;}
    break;

  case 137:

/* Line 936 of glr.c  */
#line 471 "c.y"
    { ((*yyvalp).kind) = Egsub; ;}
    break;

  case 138:

/* Line 936 of glr.c  */
#line 476 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 139:

/* Line 936 of glr.c  */
#line 478 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 140:

/* Line 936 of glr.c  */
#line 483 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 141:

/* Line 936 of glr.c  */
#line 485 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
			  newexprsrc(&ctx->inp->src, Eellipsis, 0, 0, 0, 0),
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0);
	;}
    break;

  case 142:

/* Line 936 of glr.c  */
#line 493 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 143:

/* Line 936 of glr.c  */
#line 495 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 146:

/* Line 936 of glr.c  */
#line 502 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 148:

/* Line 936 of glr.c  */
#line 508 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Enames, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 149:

/* Line 936 of glr.c  */
#line 510 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Enames, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 150:

/* Line 936 of glr.c  */
#line 515 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elapply, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 151:

/* Line 936 of glr.c  */
#line 517 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elapply, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 155:

/* Line 936 of glr.c  */
#line 528 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecomma, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 157:

/* Line 936 of glr.c  */
#line 537 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 158:

/* Line 936 of glr.c  */
#line 542 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 159:

/* Line 936 of glr.c  */
#line 544 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 160:

/* Line 936 of glr.c  */
#line 549 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecls, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 161:

/* Line 936 of glr.c  */
#line 551 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecls, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 162:

/* Line 936 of glr.c  */
#line 553 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecls, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 163:

/* Line 936 of glr.c  */
#line 558 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 164:

/* Line 936 of glr.c  */
#line 560 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 165:

/* Line 936 of glr.c  */
#line 565 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Evoid, 0, 0, 0, 0); ;}
    break;

  case 166:

/* Line 936 of glr.c  */
#line 567 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebool, 0, 0, 0, 0); ;}
    break;

  case 167:

/* Line 936 of glr.c  */
#line 569 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Echar, 0, 0, 0, 0); ;}
    break;

  case 168:

/* Line 936 of glr.c  */
#line 571 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eshort, 0, 0, 0, 0); ;}
    break;

  case 169:

/* Line 936 of glr.c  */
#line 573 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eint, 0, 0, 0, 0); ;}
    break;

  case 170:

/* Line 936 of glr.c  */
#line 575 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elong, 0, 0, 0, 0); ;}
    break;

  case 171:

/* Line 936 of glr.c  */
#line 577 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efloat, 0, 0, 0, 0); ;}
    break;

  case 172:

/* Line 936 of glr.c  */
#line 579 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edouble, 0, 0, 0, 0); ;}
    break;

  case 173:

/* Line 936 of glr.c  */
#line 581 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Esigned, 0, 0, 0, 0); ;}
    break;

  case 174:

/* Line 936 of glr.c  */
#line 583 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eunsigned, 0, 0, 0, 0); ;}
    break;

  case 175:

/* Line 936 of glr.c  */
#line 587 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 176:

/* Line 936 of glr.c  */
#line 589 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 177:

/* Line 936 of glr.c  */
#line 594 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 178:

/* Line 936 of glr.c  */
#line 596 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 181:

/* Line 936 of glr.c  */
#line 603 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 182:

/* Line 936 of glr.c  */
#line 605 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), Znull(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 183:

/* Line 936 of glr.c  */
#line 607 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 184:

/* Line 936 of glr.c  */
#line 609 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 185:

/* Line 936 of glr.c  */
#line 614 "c.y"
    { ((*yyvalp).kind) = Estruct; ;}
    break;

  case 186:

/* Line 936 of glr.c  */
#line 616 "c.y"
    { ((*yyvalp).kind) = Eunion; ;}
    break;

  case 187:

/* Line 936 of glr.c  */
#line 621 "c.y"
    { ((*yyvalp).kind) = Estruct; ;}
    break;

  case 188:

/* Line 936 of glr.c  */
#line 623 "c.y"
    { ((*yyvalp).kind) = Eunion; ;}
    break;

  case 189:

/* Line 936 of glr.c  */
#line 625 "c.y"
    { ((*yyvalp).kind) = Eenum; ;}
    break;

  case 190:

/* Line 936 of glr.c  */
#line 630 "c.y"
    {
		/* labels yield null struct_declarations */
		if((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr))
			((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
					(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0);
		else
			((*yyvalp).expr) = Znull();
	;}
    break;

  case 191:

/* Line 936 of glr.c  */
#line 639 "c.y"
    {
		/* labels yield null struct_declarations */
	 	if((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr))
			((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0);
		else
			((*yyvalp).expr) = Znull();
	;}
    break;

  case 192:

/* Line 936 of glr.c  */
#line 650 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efields, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 193:

/* Line 936 of glr.c  */
#line 652 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efields, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), Znull(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 194:

/* Line 936 of glr.c  */
#line 654 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efields, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 195:

/* Line 936 of glr.c  */
#line 656 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebitfield, (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 196:

/* Line 936 of glr.c  */
#line 659 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 197:

/* Line 936 of glr.c  */
#line 661 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 198:

/* Line 936 of glr.c  */
#line 666 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 206:

/* Line 936 of glr.c  */
#line 688 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 207:

/* Line 936 of glr.c  */
#line 690 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 208:

/* Line 936 of glr.c  */
#line 695 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 209:

/* Line 936 of glr.c  */
#line 697 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 211:

/* Line 936 of glr.c  */
#line 706 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 212:

/* Line 936 of glr.c  */
#line 708 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 213:

/* Line 936 of glr.c  */
#line 710 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 214:

/* Line 936 of glr.c  */
#line 712 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 215:

/* Line 936 of glr.c  */
#line 714 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 216:

/* Line 936 of glr.c  */
#line 719 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 217:

/* Line 936 of glr.c  */
#line 721 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 218:

/* Line 936 of glr.c  */
#line 728 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenumel, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 219:

/* Line 936 of glr.c  */
#line 730 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenumel, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 220:

/* Line 936 of glr.c  */
#line 735 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 222:

/* Line 936 of glr.c  */
#line 741 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 223:

/* Line 936 of glr.c  */
#line 743 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 224:

/* Line 936 of glr.c  */
#line 745 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 225:

/* Line 936 of glr.c  */
#line 747 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 226:

/* Line 936 of glr.c  */
#line 749 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 227:

/* Line 936 of glr.c  */
#line 751 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 230:

/* Line 936 of glr.c  */
#line 758 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 231:

/* Line 936 of glr.c  */
#line 763 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 232:

/* Line 936 of glr.c  */
#line 765 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, 0, 0, 0); ;}
    break;

  case 233:

/* Line 936 of glr.c  */
#line 767 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 234:

/* Line 936 of glr.c  */
#line 769 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 235:

/* Line 936 of glr.c  */
#line 771 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 236:

/* Line 936 of glr.c  */
#line 773 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, 0, Znull(), 0, 0); ;}
    break;

  case 237:

/* Line 936 of glr.c  */
#line 775 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 238:

/* Line 936 of glr.c  */
#line 777 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 239:

/* Line 936 of glr.c  */
#line 782 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, 0, 0, 0, 0); ;}
    break;

  case 240:

/* Line 936 of glr.c  */
#line 784 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, 0, 0, 0, 0); ;}
    break;

  case 241:

/* Line 936 of glr.c  */
#line 786 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 242:

/* Line 936 of glr.c  */
#line 788 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 243:

/* Line 936 of glr.c  */
#line 793 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 244:

/* Line 936 of glr.c  */
#line 795 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 245:

/* Line 936 of glr.c  */
#line 800 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 246:

/* Line 936 of glr.c  */
#line 802 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 247:

/* Line 936 of glr.c  */
#line 807 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 248:

/* Line 936 of glr.c  */
#line 809 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 249:

/* Line 936 of glr.c  */
#line 811 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 250:

/* Line 936 of glr.c  */
#line 816 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypename, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 251:

/* Line 936 of glr.c  */
#line 818 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypename, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 253:

/* Line 936 of glr.c  */
#line 824 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 254:

/* Line 936 of glr.c  */
#line 826 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 255:

/* Line 936 of glr.c  */
#line 831 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 256:

/* Line 936 of glr.c  */
#line 833 "c.y"
    { ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0)); ;}
    break;

  case 257:

/* Line 936 of glr.c  */
#line 836 "c.y"
    { ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0)); ;}
    break;

  case 258:

/* Line 936 of glr.c  */
#line 839 "c.y"
    { ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, 0,
			  newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0)); ;}
    break;

  case 259:

/* Line 936 of glr.c  */
#line 842 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 260:

/* Line 936 of glr.c  */
#line 844 "c.y"
    { ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0)); ;}
    break;

  case 261:

/* Line 936 of glr.c  */
#line 849 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 262:

/* Line 936 of glr.c  */
#line 851 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 263:

/* Line 936 of glr.c  */
#line 856 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 264:

/* Line 936 of glr.c  */
#line 858 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 265:

/* Line 936 of glr.c  */
#line 863 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 266:

/* Line 936 of glr.c  */
#line 865 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 267:

/* Line 936 of glr.c  */
#line 867 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 269:

/* Line 936 of glr.c  */
#line 873 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 270:

/* Line 936 of glr.c  */
#line 875 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 271:

/* Line 936 of glr.c  */
#line 880 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 272:

/* Line 936 of glr.c  */
#line 882 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 275:

/* Line 936 of glr.c  */
#line 889 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 276:

/* Line 936 of glr.c  */
#line 894 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 279:

/* Line 936 of glr.c  */
#line 901 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 280:

/* Line 936 of glr.c  */
#line 906 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 281:

/* Line 936 of glr.c  */
#line 908 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, 0, 0, 0); ;}
    break;

  case 282:

/* Line 936 of glr.c  */
#line 910 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 283:

/* Line 936 of glr.c  */
#line 912 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 284:

/* Line 936 of glr.c  */
#line 914 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 285:

/* Line 936 of glr.c  */
#line 916 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, 0, nullelist(), 0, 0); ;}
    break;

  case 286:

/* Line 936 of glr.c  */
#line 918 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 287:

/* Line 936 of glr.c  */
#line 920 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 288:

/* Line 936 of glr.c  */
#line 925 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 290:

/* Line 936 of glr.c  */
#line 931 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 291:

/* Line 936 of glr.c  */
#line 933 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 292:

/* Line 936 of glr.c  */
#line 935 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 293:

/* Line 936 of glr.c  */
#line 937 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 294:

/* Line 936 of glr.c  */
#line 939 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 295:

/* Line 936 of glr.c  */
#line 941 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 305:

/* Line 936 of glr.c  */
#line 958 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxunquote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 306:

/* Line 936 of glr.c  */
#line 963 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eglobal, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 307:

/* Line 936 of glr.c  */
#line 968 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 308:

/* Line 936 of glr.c  */
#line 973 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 309:

/* Line 936 of glr.c  */
#line 975 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 310:

/* Line 936 of glr.c  */
#line 980 "c.y"
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

  case 311:

/* Line 936 of glr.c  */
#line 990 "c.y"
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

  case 312:

/* Line 936 of glr.c  */
#line 1000 "c.y"
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

  case 313:

/* Line 936 of glr.c  */
#line 1013 "c.y"
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

  case 314:

/* Line 936 of glr.c  */
#line 1029 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 315:

/* Line 936 of glr.c  */
#line 1031 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 316:

/* Line 936 of glr.c  */
#line 1036 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Enil, 0, 0, 0, 0); ;}
    break;

  case 317:

/* Line 936 of glr.c  */
#line 1038 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 318:

/* Line 936 of glr.c  */
#line 1040 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 319:

/* Line 936 of glr.c  */
#line 1045 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eif, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 320:

/* Line 936 of glr.c  */
#line 1047 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eif, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (7))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (7))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (7))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 321:

/* Line 936 of glr.c  */
#line 1050 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eswitch, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 322:

/* Line 936 of glr.c  */
#line 1055 "c.y"
    { ((*yyvalp).expr) = Zlabelsrc(&(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr)->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 323:

/* Line 936 of glr.c  */
#line 1057 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), NULL, NULL); ;}
    break;

  case 324:

/* Line 936 of glr.c  */
#line 1059 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ematch, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), NULL, NULL); ;}
    break;

  case 325:

/* Line 936 of glr.c  */
#line 1061 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefault, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), NULL, NULL, NULL); ;}
    break;

  case 327:

/* Line 936 of glr.c  */
#line 1067 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 328:

/* Line 936 of glr.c  */
#line 1072 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ewhile, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 329:

/* Line 936 of glr.c  */
#line 1074 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edo, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (7))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 330:

/* Line 936 of glr.c  */
#line 1076 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (9))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (9))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (9))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((9) - (9))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 331:

/* Line 936 of glr.c  */
#line 1081 "c.y"
    { ((*yyvalp).expr) = Zgotosrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 332:

/* Line 936 of glr.c  */
#line 1083 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Econtinue, 0, 0, 0, 0); ;}
    break;

  case 333:

/* Line 936 of glr.c  */
#line 1085 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebreak, 0, 0, 0, 0); ;}
    break;

  case 334:

/* Line 936 of glr.c  */
#line 1087 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eret, 0, 0, 0, 0); ;}
    break;

  case 335:

/* Line 936 of glr.c  */
#line 1089 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eret, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 336:

/* Line 936 of glr.c  */
#line 1094 "c.y"
    { ((*yyvalp).kind) = Edefine; ;}
    break;

  case 337:

/* Line 936 of glr.c  */
#line 1096 "c.y"
    { ((*yyvalp).kind) = Edefloc; ;}
    break;

  case 338:

/* Line 936 of glr.c  */
#line 1101 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr)->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (6))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 339:

/* Line 936 of glr.c  */
#line 1103 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&(((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr)->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), nullelist(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 344:

/* Line 936 of glr.c  */
#line 1117 "c.y"
    { dotop(ctx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr)); ;}
    break;



/* Line 936 of glr.c  */
#line 4326 "c.tab.c"
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
#line 1120 "c.y"

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

