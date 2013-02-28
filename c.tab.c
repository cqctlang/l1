/* A Bison parser, made by GNU Bison 2.5.  */

/* Skeleton implementation for Bison GLR parsers in C
   
      Copyright (C) 2002-2011 Free Software Foundation, Inc.
   
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
#define YYBISON_VERSION "2.5"

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
#line 69 "c.tab.c"



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
#line 96 "c.y"

	static void yyerror(U *ctx, const char *s);
	static Expr* castmerge(YYSTYPE e1, YYSTYPE e2);
	static Expr* ofmerge(YYSTYPE e1, YYSTYPE e2);


/* Line 243 of glr.c  */
#line 111 "c.tab.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
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
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5) \
      || (defined __STRICT_ANSI__ && __STRICT_ANSI__))
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
#define YYFINAL  255
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   4446

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  112
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  120
/* YYNRULES -- Number of rules.  */
#define YYNRULES  389
/* YYNRULES -- Number of states.  */
#define YYNSTATES  702
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule.  */
#define YYMAXRHS 9
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule.  */
#define YYMAXLEFT 0

/* YYTRANSLATE(X) -- Bison symbol number corresponding to X.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   340

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   102,     2,     2,     2,   104,    97,     2,
      86,    87,    98,    99,    90,   100,    96,   103,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    91,   110,
     105,    95,   106,   109,   111,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    94,     2,    92,   107,     2,    93,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    88,   108,    89,   101,     2,     2,     2,
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
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     5,     8,    13,    16,    21,    23,    25,
      27,    29,    31,    32,    34,    40,    45,    51,    58,    63,
      66,    70,    74,    78,    82,    87,    91,    93,    97,   102,
     105,   107,   110,   112,   116,   118,   122,   125,   129,   134,
     136,   140,   142,   144,   146,   150,   156,   159,   163,   168,
     173,   177,   181,   186,   188,   190,   192,   194,   196,   198,
     200,   202,   204,   206,   208,   212,   218,   221,   225,   230,
     235,   239,   243,   248,   253,   257,   262,   266,   268,   270,
     272,   274,   276,   280,   284,   290,   292,   296,   300,   302,
     307,   311,   316,   320,   324,   327,   330,   339,   341,   343,
     345,   349,   351,   353,   356,   359,   362,   365,   370,   373,
     378,   383,   385,   387,   389,   391,   393,   395,   397,   402,
     407,   409,   413,   417,   421,   423,   427,   431,   433,   437,
     441,   443,   447,   451,   455,   459,   461,   465,   469,   471,
     475,   477,   481,   483,   487,   489,   493,   495,   499,   501,
     507,   509,   513,   515,   517,   519,   521,   523,   525,   527,
     529,   531,   533,   535,   537,   539,   541,   545,   547,   550,
     552,   555,   557,   559,   561,   564,   566,   572,   577,   584,
     589,   591,   593,   595,   599,   601,   606,   609,   610,   613,
     619,   623,   625,   629,   631,   633,   635,   637,   639,   641,
     643,   645,   647,   649,   651,   654,   656,   658,   660,   662,
     669,   675,   681,   686,   689,   691,   693,   695,   697,   699,
     701,   704,   710,   715,   718,   722,   731,   737,   740,   743,
     747,   749,   751,   753,   756,   758,   759,   761,   764,   767,
     769,   773,   775,   781,   788,   793,   799,   802,   804,   808,
     810,   814,   817,   819,   821,   825,   830,   834,   840,   845,
     847,   849,   852,   856,   859,   863,   867,   872,   876,   881,
     887,   889,   892,   895,   899,   901,   905,   907,   911,   915,
     919,   922,   924,   927,   929,   932,   935,   937,   941,   945,
     947,   950,   955,   957,   959,   963,   965,   969,   972,   975,
     977,   979,   982,   985,   987,   989,   991,   993,   996,   999,
    1001,  1003,  1006,  1010,  1013,  1017,  1021,  1026,  1030,  1035,
    1041,  1044,  1046,  1048,  1052,  1057,  1061,  1066,  1070,  1072,
    1074,  1076,  1078,  1080,  1082,  1084,  1086,  1088,  1090,  1092,
    1098,  1103,  1106,  1111,  1115,  1119,  1121,  1124,  1127,  1131,
    1135,  1140,  1142,  1144,  1146,  1149,  1151,  1154,  1157,  1163,
    1171,  1177,  1181,  1186,  1191,  1198,  1202,  1204,  1205,  1211,
    1219,  1229,  1233,  1236,  1239,  1242,  1246,  1254,  1261,  1268,
    1274,  1279,  1281,  1283,  1290,  1296,  1298,  1299,  1301,  1304
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const short int yyrhs[] =
{
     229,     0,    -1,     3,    -1,    77,   113,    -1,    77,    86,
     165,    87,    -1,    78,   113,    -1,    78,    86,   165,    87,
      -1,   113,    -1,   114,    -1,     9,    -1,    10,    -1,   116,
      -1,    -1,    46,    -1,   120,    86,   159,    87,   217,    -1,
     120,    86,    87,   217,    -1,    17,   116,    88,   158,    89,
      -1,    17,   116,    88,   158,    90,    89,    -1,    17,   116,
      88,    89,    -1,    48,   217,    -1,    75,   219,    89,    -1,
      75,   165,    89,    -1,    76,   219,    89,    -1,    76,   165,
      89,    -1,   118,    86,   140,    87,    -1,   118,    86,    87,
      -1,   118,    -1,   117,    86,    87,    -1,   117,    86,   140,
      87,    -1,    85,     5,    -1,    85,    -1,    84,     6,    -1,
      84,    -1,   164,    91,   164,    -1,   128,    -1,   129,    90,
     128,    -1,    79,    92,    -1,    79,   140,    92,    -1,    79,
     140,    90,    92,    -1,   116,    -1,   116,    93,   116,    -1,
       4,    -1,     5,    -1,     6,    -1,    86,   165,    87,    -1,
      80,   164,    90,   164,    81,    -1,    94,    92,    -1,    94,
     140,    92,    -1,    94,   140,    90,    92,    -1,    94,   140,
      63,    92,    -1,    94,    91,    92,    -1,    94,   129,    92,
      -1,    94,   129,    90,    92,    -1,   130,    -1,   121,    -1,
     122,    -1,   123,    -1,   124,    -1,   126,    -1,   127,    -1,
     116,    -1,     5,    -1,     4,    -1,     6,    -1,    86,   132,
      87,    -1,    80,   132,    90,   132,    81,    -1,    94,    92,
      -1,    94,   134,    92,    -1,    94,   134,    90,    92,    -1,
      94,   134,    63,    92,    -1,    94,    91,    92,    -1,    94,
     136,    92,    -1,    94,   136,    90,    92,    -1,   116,    86,
     133,    87,    -1,   116,    86,    87,    -1,   118,    86,   134,
      87,    -1,   118,    86,    87,    -1,   118,    -1,   130,    -1,
     134,    -1,   135,    -1,   132,    -1,   134,    90,   132,    -1,
     116,    95,   132,    -1,   135,    90,   116,    95,   132,    -1,
     137,    -1,   136,    90,   137,    -1,   132,    91,   132,    -1,
     131,    -1,   138,    94,   165,    92,    -1,   138,    86,    87,
      -1,   138,    86,   140,    87,    -1,   138,    96,   116,    -1,
     138,    20,   116,    -1,   138,    21,    -1,   138,    22,    -1,
      19,    86,   165,    90,   197,    90,   116,    87,    -1,   164,
      -1,   115,    -1,   139,    -1,   140,    90,   139,    -1,   138,
      -1,   125,    -1,    21,   141,    -1,    22,   141,    -1,   142,
     143,    -1,    11,   141,    -1,    11,    86,   197,    87,    -1,
      13,   141,    -1,    13,    86,   197,    87,    -1,    12,    86,
     197,    87,    -1,    97,    -1,    98,    -1,    99,    -1,   100,
      -1,   101,    -1,   102,    -1,   141,    -1,    86,   197,    87,
     143,    -1,    88,   165,    89,   143,    -1,   143,    -1,   144,
      98,   143,    -1,   144,   103,   143,    -1,   144,   104,   143,
      -1,   144,    -1,   145,    99,   144,    -1,   145,   100,   144,
      -1,   145,    -1,   146,    23,   145,    -1,   146,    24,   145,
      -1,   146,    -1,   147,   105,   146,    -1,   147,   106,   146,
      -1,   147,    25,   146,    -1,   147,    26,   146,    -1,   147,
      -1,   148,    27,   147,    -1,   148,    28,   147,    -1,   148,
      -1,   149,    97,   148,    -1,   149,    -1,   150,   107,   149,
      -1,   150,    -1,   151,   108,   150,    -1,   151,    -1,   152,
      29,   151,    -1,   152,    -1,   153,    30,   152,    -1,   153,
      -1,   153,   109,   165,    91,   154,    -1,   154,    -1,   141,
     156,   164,    -1,    95,    -1,    34,    -1,    38,    -1,    40,
      -1,    39,    -1,    32,    -1,    33,    -1,    31,    -1,    36,
      -1,    37,    -1,    35,    -1,   116,    -1,   115,    -1,   157,
      -1,   158,    90,   157,    -1,   158,    -1,   158,    63,    -1,
     161,    -1,   160,   161,    -1,   169,    -1,   167,    -1,   115,
      -1,     1,   110,    -1,   155,    -1,    47,   165,    88,   160,
      89,    -1,    47,   165,    88,    89,    -1,    49,    86,   164,
      90,   140,    87,    -1,    49,    86,   164,    87,    -1,   162,
      -1,   163,    -1,   164,    -1,   165,    90,   164,    -1,   154,
      -1,    14,   183,   170,   110,    -1,   111,   166,    -1,    -1,
     183,   110,    -1,   111,   166,   183,   170,   110,    -1,   183,
     170,   110,    -1,   189,    -1,   170,    90,   189,    -1,    59,
      -1,    50,    -1,    51,    -1,    52,    -1,    53,    -1,    54,
      -1,    57,    -1,    58,    -1,    55,    -1,    56,    -1,   171,
      -1,   172,   171,    -1,   172,    -1,   116,    -1,   174,    -1,
     186,    -1,   175,   119,    88,   177,   179,    89,    -1,   175,
     119,    88,   177,    89,    -1,   175,   119,    88,   179,    89,
      -1,   175,   119,    88,    89,    -1,   175,   116,    -1,    60,
      -1,    61,    -1,    60,    -1,    61,    -1,    62,    -1,   178,
      -1,   177,   178,    -1,   111,   166,   183,   184,   110,    -1,
     111,   166,   183,   110,    -1,   183,   110,    -1,   183,   184,
     110,    -1,   111,   111,   166,   183,   185,    91,   166,   110,
      -1,   183,   185,    91,   166,   110,    -1,   116,    91,    -1,
       1,   110,    -1,   111,   166,   110,    -1,     7,    -1,     8,
      -1,   180,    -1,   180,   181,    -1,   181,    -1,    -1,   173,
      -1,   181,   173,    -1,   173,   181,    -1,   185,    -1,   184,
      90,   185,    -1,   189,    -1,    62,   119,    88,   187,    89,
      -1,    62,   119,    88,   187,    90,    89,    -1,    62,   119,
      88,    89,    -1,    62,   119,    88,    90,    89,    -1,    62,
     116,    -1,   188,    -1,   187,    90,   188,    -1,   116,    -1,
     116,    95,   166,    -1,   193,   190,    -1,   190,    -1,   116,
      -1,    86,   189,    87,    -1,   190,    94,   166,    92,    -1,
     190,    94,    92,    -1,   190,    86,   194,    87,   182,    -1,
     190,    86,    87,   182,    -1,   193,    -1,   192,    -1,   193,
     192,    -1,    86,   191,    87,    -1,    94,    92,    -1,    94,
     166,    92,    -1,   192,    94,    92,    -1,   192,    94,   166,
      92,    -1,    86,    87,   182,    -1,   192,    86,    87,   182,
      -1,   192,    86,   194,    87,   182,    -1,    98,    -1,    98,
     181,    -1,    98,   193,    -1,    98,   181,   193,    -1,   195,
      -1,   195,    90,    63,    -1,   196,    -1,   195,    90,   196,
      -1,   168,   183,   189,    -1,   168,   183,   191,    -1,   168,
     183,    -1,   198,    -1,   198,   207,    -1,   199,    -1,   181,
     199,    -1,   199,   181,    -1,   172,    -1,   116,    93,   172,
      -1,   116,    93,   116,    -1,   113,    -1,   176,   116,    -1,
     176,   116,    93,   116,    -1,   114,    -1,   201,    -1,   201,
      90,    63,    -1,   202,    -1,   201,    90,   202,    -1,   203,
     209,    -1,   203,   207,    -1,   203,    -1,   204,    -1,   204,
     181,    -1,   181,   204,    -1,   172,    -1,   116,    -1,   205,
      -1,   206,    -1,   175,   116,    -1,    62,   116,    -1,   193,
      -1,   208,    -1,   193,   208,    -1,    86,   207,    87,    -1,
      94,    92,    -1,    94,   166,    92,    -1,   208,    94,    92,
      -1,   208,    94,   166,    92,    -1,    86,    87,   182,    -1,
     208,    86,    87,   182,    -1,   208,    86,   200,    87,   182,
      -1,   193,   210,    -1,   210,    -1,   116,    -1,    86,   209,
      87,    -1,   210,    94,   166,    92,    -1,   210,    94,    92,
      -1,   210,    86,   200,    87,    -1,   210,    86,    87,    -1,
     217,    -1,   220,    -1,   221,    -1,   224,    -1,   225,    -1,
     228,    -1,   226,    -1,   222,    -1,   214,    -1,   213,    -1,
     212,    -1,   117,    86,   140,    87,   217,    -1,   117,    86,
      87,   217,    -1,   117,   217,    -1,    77,    88,   219,    89,
      -1,    44,   158,   110,    -1,    45,   158,   110,    -1,   215,
      -1,   216,   215,    -1,    88,    89,    -1,    88,   216,    89,
      -1,    88,   219,    89,    -1,    88,   216,   219,    89,    -1,
     211,    -1,   115,    -1,   218,    -1,   219,   218,    -1,   110,
      -1,   165,   110,    -1,     1,   110,    -1,    64,    86,   165,
      87,   211,    -1,    64,    86,   165,    87,   211,    65,   211,
      -1,    66,    86,   165,    87,   217,    -1,   116,    91,   211,
      -1,    73,   165,    91,   211,    -1,    83,   132,    91,   211,
      -1,    83,   132,    29,   165,    91,   211,    -1,    74,    91,
     211,    -1,   165,    -1,    -1,    67,    86,   165,    87,   211,
      -1,    68,   211,    67,    86,   165,    87,   110,    -1,    69,
      86,   223,   110,   223,   110,   223,    87,   211,    -1,    43,
     116,   110,    -1,    70,   110,    -1,    71,   110,    -1,    72,
     110,    -1,    72,   165,   110,    -1,    18,   117,    86,   159,
      87,   116,   217,    -1,    18,   117,    86,   159,    87,   217,
      -1,    18,   117,    86,    87,   116,   217,    -1,    18,   117,
      86,    87,   217,    -1,    18,   117,   116,   217,    -1,    15,
      -1,    16,    -1,   227,   116,    86,   159,    87,   217,    -1,
     227,   116,    86,    87,   217,    -1,   230,    -1,    -1,   231,
      -1,   230,   231,    -1,   211,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   105,   105,   110,   112,   117,   119,   124,   125,   129,
     134,   139,   141,   145,   149,   151,   156,   158,   160,   165,
     170,   172,   174,   176,   181,   183,   185,   190,   192,   197,
     207,   211,   218,   225,   235,   237,   242,   244,   246,   251,
     252,   254,   256,   258,   260,   262,   264,   266,   268,   270,
     275,   277,   279,   281,   282,   283,   284,   285,   286,   287,
     291,   292,   294,   296,   298,   300,   302,   304,   306,   308,
     313,   315,   317,   319,   321,   323,   325,   327,   329,   333,
     334,   338,   340,   345,   350,   358,   360,   365,   377,   378,
     380,   382,   384,   386,   388,   390,   392,   397,   398,   402,
     404,   409,   410,   411,   413,   415,   417,   419,   421,   423,
     425,   430,   432,   434,   436,   438,   440,   445,   446,   448,
     453,   454,   456,   458,   463,   464,   466,   471,   472,   474,
     479,   480,   482,   484,   486,   491,   492,   494,   499,   500,
     505,   506,   511,   512,   517,   518,   523,   524,   529,   530,
     535,   536,   545,   547,   549,   551,   553,   555,   557,   559,
     561,   563,   565,   571,   572,   576,   578,   583,   585,   593,
     595,   600,   601,   602,   603,   608,   609,   611,   616,   618,
     623,   624,   628,   629,   634,   638,   643,   646,   650,   652,
     654,   659,   661,   666,   668,   670,   672,   674,   676,   678,
     680,   682,   684,   688,   690,   695,   697,   699,   700,   704,
     706,   708,   710,   712,   717,   719,   724,   726,   728,   733,
     742,   753,   755,   757,   759,   761,   763,   766,   768,   773,
     779,   780,   784,   785,   789,   790,   794,   795,   797,   802,
     804,   809,   813,   815,   817,   819,   821,   826,   828,   835,
     837,   842,   844,   848,   850,   852,   854,   856,   858,   863,
     864,   865,   870,   872,   874,   876,   878,   880,   882,   884,
     889,   891,   893,   895,   900,   902,   907,   909,   914,   916,
     918,   923,   925,   930,   931,   933,   938,   940,   943,   946,
     949,   951,   954,   958,   960,   965,   967,   972,   974,   976,
     981,   982,   984,   989,   991,   993,   994,   998,  1003,  1008,
    1009,  1010,  1015,  1017,  1019,  1021,  1023,  1025,  1027,  1029,
    1034,  1036,  1040,  1042,  1044,  1046,  1048,  1050,  1055,  1056,
    1057,  1058,  1059,  1060,  1061,  1062,  1063,  1064,  1065,  1069,
    1071,  1073,  1078,  1083,  1088,  1093,  1095,  1100,  1110,  1120,
    1132,  1147,  1148,  1152,  1154,  1159,  1161,  1163,  1168,  1170,
    1172,  1178,  1180,  1182,  1184,  1186,  1191,  1193,  1197,  1199,
    1201,  1206,  1208,  1210,  1212,  1214,  1219,  1221,  1223,  1225,
    1227,  1232,  1234,  1239,  1241,  1246,  1247,  1251,  1252,  1256
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENTIFIER", "SYMBOL", "CONSTANT",
  "STRING_LITERAL", "CONST", "VOLATILE", "ATIDENTIFIER", "SYNTAXID",
  "SIZEOF", "TYPENAME", "TYPEOF", "TYPEDEF", "DEFINE", "DEFLOC", "DEFREC",
  "DEFSTX", "CONTAINEROF", "PTR_OP", "INC_OP", "DEC_OP", "LEFT_OP",
  "RIGHT_OP", "LE_OP", "GE_OP", "EQ_OP", "NE_OP", "AND_OP", "OR_OP",
  "MUL_ASSIGN", "DIV_ASSIGN", "MOD_ASSIGN", "ADD_ASSIGN", "SUB_ASSIGN",
  "LEFT_ASSIGN", "RIGHT_ASSIGN", "AND_ASSIGN", "XOR_ASSIGN", "OR_ASSIGN",
  "CAST_ASSIGN", "XCAST_ASSIGN", "GOTO", "GLOBAL", "LOCAL", "LAMBDA",
  "NAMES", "LET", "LAPPLY", "BOOL", "CHAR", "SHORT", "INT", "LONG",
  "SIGNED", "UNSIGNED", "FLOAT", "DOUBLE", "VOID", "STRUCT", "UNION",
  "ENUM", "ELLIPSIS", "IF", "ELSE", "SWITCH", "WHILE", "DO", "FOR",
  "CONTINUE", "BREAK", "RETURN", "CASE", "DEFAULT", "SYNTAXQUOTE",
  "SYNTAXQUASI", "SYNTAXUNQUOTE", "SYNTAXSPLICE", "SYNTAXLIST", "LPAIR",
  "RPAIR", "NOBIND_PRE", "MATCH", "ATFILE", "ATLINE", "'('", "')'", "'{'",
  "'}'", "','", "':'", "']'", "'`'", "'['", "'='", "'.'", "'&'", "'*'",
  "'+'", "'-'", "'~'", "'!'", "'/'", "'%'", "'<'", "'>'", "'^'", "'|'",
  "'?'", "';'", "'@'", "$accept", "_id", "unquote_expr", "splice_expr",
  "id", "atid", "syntaxid", "maybeid", "lambda", "lambda_expression",
  "defrec_expression", "let_expression", "quote_expression",
  "syntax_expression", "mcall_expression", "src_expression", "table_init",
  "table_init_list", "syntax_list", "primary_expression", "pattern",
  "rec_pat_list", "pattern_list", "var_pat_list",
  "table_init_pattern_list", "table_init_pattern", "postfix_expression",
  "argument_expression_list_element", "argument_expression_list",
  "unary_expression", "unary_operator", "cast_expression",
  "multiplicative_expression", "additive_expression", "shift_expression",
  "relational_expression", "equality_expression", "and_expression",
  "exclusive_or_expression", "inclusive_or_expression",
  "logical_and_expression", "logical_or_expression",
  "conditional_expression", "assignment_expression", "assignment_operator",
  "id_list_element", "id_list", "arg_id_list", "names_declaration_list",
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
  "statement", "mcall_statement", "unquote_statement", "global_statement",
  "local", "local_list", "compound_statement", "statement_list_element",
  "statement_list", "expression_statement", "selection_statement",
  "labeled_statement", "maybe_expression", "iteration_statement",
  "jump_statement", "defstx_statement", "define", "define_statement",
  "translation_unit_seq", "translation_unit", "external_declaration", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   112,   113,   114,   114,   115,   115,   116,   116,   117,
     118,   119,   119,   120,   121,   121,   122,   122,   122,   123,
     124,   124,   124,   124,   125,   125,   125,   126,   126,   127,
     127,   127,   127,   128,   129,   129,   130,   130,   130,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     132,   132,   132,   132,   132,   132,   132,   132,   132,   132,
     132,   132,   132,   132,   132,   132,   132,   132,   132,   133,
     133,   134,   134,   135,   135,   136,   136,   137,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   139,   139,   140,
     140,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   142,   142,   142,   142,   142,   142,   143,   143,   143,
     144,   144,   144,   144,   145,   145,   145,   146,   146,   146,
     147,   147,   147,   147,   147,   148,   148,   148,   149,   149,
     150,   150,   151,   151,   152,   152,   153,   153,   154,   154,
     155,   155,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   157,   157,   158,   158,   159,   159,   160,
     160,   161,   161,   161,   161,   162,   162,   162,   163,   163,
     164,   164,   165,   165,   166,   167,   168,   168,   169,   169,
     169,   170,   170,   171,   171,   171,   171,   171,   171,   171,
     171,   171,   171,   172,   172,   173,   173,   173,   173,   174,
     174,   174,   174,   174,   175,   175,   176,   176,   176,   177,
     177,   178,   178,   178,   178,   178,   178,   178,   178,   179,
     180,   180,   181,   181,   182,   182,   183,   183,   183,   184,
     184,   185,   186,   186,   186,   186,   186,   187,   187,   188,
     188,   189,   189,   190,   190,   190,   190,   190,   190,   191,
     191,   191,   192,   192,   192,   192,   192,   192,   192,   192,
     193,   193,   193,   193,   194,   194,   195,   195,   196,   196,
     196,   197,   197,   198,   198,   198,   199,   199,   199,   199,
     199,   199,   199,   200,   200,   201,   201,   202,   202,   202,
     203,   203,   203,   204,   204,   204,   204,   205,   206,   207,
     207,   207,   208,   208,   208,   208,   208,   208,   208,   208,
     209,   209,   210,   210,   210,   210,   210,   210,   211,   211,
     211,   211,   211,   211,   211,   211,   211,   211,   211,   212,
     212,   212,   213,   214,   215,   216,   216,   217,   217,   217,
     217,   218,   218,   219,   219,   220,   220,   220,   221,   221,
     221,   222,   222,   222,   222,   222,   223,   223,   224,   224,
     224,   225,   225,   225,   225,   225,   226,   226,   226,   226,
     226,   227,   227,   228,   228,   229,   229,   230,   230,   231
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     2,     4,     2,     4,     1,     1,     1,
       1,     1,     0,     1,     5,     4,     5,     6,     4,     2,
       3,     3,     3,     3,     4,     3,     1,     3,     4,     2,
       1,     2,     1,     3,     1,     3,     2,     3,     4,     1,
       3,     1,     1,     1,     3,     5,     2,     3,     4,     4,
       3,     3,     4,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     5,     2,     3,     4,     4,
       3,     3,     4,     4,     3,     4,     3,     1,     1,     1,
       1,     1,     3,     3,     5,     1,     3,     3,     1,     4,
       3,     4,     3,     3,     2,     2,     8,     1,     1,     1,
       3,     1,     1,     2,     2,     2,     2,     4,     2,     4,
       4,     1,     1,     1,     1,     1,     1,     1,     4,     4,
       1,     3,     3,     3,     1,     3,     3,     1,     3,     3,
       1,     3,     3,     3,     3,     1,     3,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     5,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     1,     2,     1,
       2,     1,     1,     1,     2,     1,     5,     4,     6,     4,
       1,     1,     1,     3,     1,     4,     2,     0,     2,     5,
       3,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     1,     6,
       5,     5,     4,     2,     1,     1,     1,     1,     1,     1,
       2,     5,     4,     2,     3,     8,     5,     2,     2,     3,
       1,     1,     1,     2,     1,     0,     1,     2,     2,     1,
       3,     1,     5,     6,     4,     5,     2,     1,     3,     1,
       3,     2,     1,     1,     3,     4,     3,     5,     4,     1,
       1,     2,     3,     2,     3,     3,     4,     3,     4,     5,
       1,     2,     2,     3,     1,     3,     1,     3,     3,     3,
       2,     1,     2,     1,     2,     2,     1,     3,     3,     1,
       2,     4,     1,     1,     3,     1,     3,     2,     2,     1,
       1,     2,     2,     1,     1,     1,     1,     2,     2,     1,
       1,     2,     3,     2,     3,     3,     4,     3,     4,     5,
       2,     1,     1,     3,     4,     3,     4,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     5,
       4,     2,     4,     3,     3,     1,     2,     2,     3,     3,
       4,     1,     1,     1,     2,     1,     2,     2,     5,     7,
       5,     3,     4,     4,     6,     3,     1,     0,     5,     7,
       9,     3,     2,     2,     2,     3,     7,     6,     6,     5,
       4,     1,     1,     6,     5,     1,     0,     1,     2,     1
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     2,     1,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     1,     1,     1,     1,
       0,     0,     0,     0,     0,     0,     0,     2,     2,     0,
       2,     2,     0,     0,     2,     2,     2,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     2,     2,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
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

/* YYDEFACT[S] -- default reduction number in state S.  Performed when
   YYTABLE doesn't specify something else to do.  Zero means the default
   is an error.  */
static const unsigned short int yydefact[] =
{
       0,     0,     2,    41,    42,    43,     9,    10,     0,     0,
       0,   381,   382,     0,     0,     0,     0,     0,     0,     0,
      13,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      32,    30,     0,     0,     0,   111,   112,   113,   114,   115,
     116,   355,     7,     8,    39,     0,    26,     0,    54,    55,
      56,    57,   102,    58,    59,    53,    88,   101,   117,     0,
     120,   124,   127,   130,   135,   138,   140,   142,   144,   146,
     148,   150,   175,   180,   181,   182,     0,   389,   338,   337,
     336,   328,   329,   330,   335,   331,   332,   334,     0,   333,
       0,     0,   387,   357,     0,     0,    39,     0,   106,     0,
       0,   108,     0,     0,     0,     0,   103,   104,     0,     0,
     164,   163,   165,     0,     0,     0,     0,    19,     0,     0,
       0,     0,     0,   367,   372,   373,   374,     0,     0,     0,
     352,     0,   351,   353,     0,     0,     0,     0,     0,     3,
      36,    98,    99,     0,    97,     0,    62,    61,    63,     0,
       0,     0,    60,    77,    78,     0,    31,    29,   230,   231,
     194,   195,   196,   197,   198,   201,   202,   199,   200,   193,
     216,   217,   218,     7,     8,    39,     0,   203,   286,     0,
     232,     0,     0,   281,   283,     0,   347,     0,   345,     0,
       0,     0,    46,    34,     0,     0,    97,     0,     0,     0,
     341,     0,     0,     0,    94,    95,     0,     0,     0,   159,
     157,   158,   153,   162,   160,   161,   154,   156,   155,   152,
       0,   117,   105,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   356,     0,     1,   388,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   371,     0,     5,     0,
     343,     0,     0,     0,     0,     0,     0,     0,   366,     0,
     375,     0,   365,    21,    20,   354,    23,    22,     0,     0,
       0,    37,     0,     0,     0,     0,    66,    81,     0,     0,
      85,     0,     0,     0,     0,     0,    44,   204,   290,   233,
     284,     0,     0,     0,   270,   309,   282,   310,   285,     0,
       0,   348,   346,     0,   349,    50,     0,    51,     0,     0,
      47,     0,   361,    40,    27,     0,    25,     0,     0,   167,
       0,    93,    90,     0,     0,    92,   151,   121,   122,   123,
     125,   126,   128,   129,   133,   134,   131,   132,   136,   137,
     139,   141,   143,   145,   147,     0,   183,     0,   107,    27,
       0,     0,   110,   109,    18,     0,     0,     0,   380,     0,
       0,   166,     0,     0,   214,   215,    12,   177,     0,   173,
     206,     0,   169,   172,   171,   205,   236,   207,    12,     0,
       0,   208,   179,     0,     0,     0,     0,     0,   367,   362,
       4,   342,    38,   100,     0,     0,    64,    70,     0,     0,
       0,    67,     0,    71,    74,    60,    81,     0,    79,    80,
      76,     0,     0,   363,    40,   287,     0,   118,   235,     0,
     313,   184,     0,   271,   272,   311,     0,     0,   344,   119,
     350,    52,    35,     0,    49,    48,    33,   340,    28,     0,
      24,    15,   168,     0,    91,    89,     0,     0,     0,    28,
     288,    16,     0,     0,   379,     0,     0,     6,   174,     0,
     246,     0,     0,   176,   170,   238,   213,     0,   237,     0,
     188,   253,     0,   191,   252,     0,     0,   358,   360,   368,
       0,     0,    45,     0,    87,    69,    68,    82,    72,     0,
      86,     0,    73,     0,     0,    75,     0,   291,   234,   317,
     312,   314,   273,     0,   235,   304,   303,     0,     0,     0,
     293,   295,   299,   300,   305,   306,   315,     0,   339,    14,
     149,   384,     0,    17,   378,     0,   377,     0,     0,     0,
       0,     0,     0,     0,   190,   187,     0,   251,   178,     0,
       0,   367,    65,    83,     0,   364,   308,   318,   307,   302,
     235,     0,     0,   322,   309,   298,   297,   321,   301,   316,
     383,   376,     0,   185,   244,     0,   249,     0,   247,     0,
       0,   212,     0,   206,     0,   219,     0,     0,   254,   192,
     235,     0,     0,     0,   274,   276,   256,     0,   359,   369,
       0,     0,   319,   294,   296,     0,   320,     0,     0,    96,
     245,     0,   242,     0,   189,   228,     0,     0,   227,   210,
     220,     0,   211,   223,     0,   239,   241,   258,   186,   280,
     235,   187,   255,     0,    84,   323,   327,     0,   325,     0,
     250,   243,   248,     0,   229,     0,   209,     0,   224,     0,
       0,     0,   278,   279,   260,   259,   257,   275,   277,   370,
     326,   324,     0,   222,     0,   239,   240,     0,   235,     0,
     263,     0,   187,     0,   261,     0,   221,   226,   267,   262,
     264,   235,     0,   265,     0,     0,   268,   235,   266,     0,
     269,   225
};

/* YYPDEFGOTO[NTERM-NUM].  */
static const short int yydefgoto[] =
{
      -1,    52,    53,   140,   106,   107,    56,   481,    57,    58,
      59,    60,    61,    62,    63,    64,   203,   204,    65,    66,
     426,   427,   298,   429,   299,   300,    67,   152,   153,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,   230,   122,   339,   340,   391,   392,
      83,    84,    85,    86,   442,   393,   602,   394,   492,   187,
     395,   396,   397,   398,   189,   594,   595,   596,   190,   399,
     519,   400,   634,   635,   401,   587,   588,   636,   494,   663,
     664,   495,   603,   604,   605,   192,   193,   194,   529,   530,
     531,   532,   533,   534,   535,   439,   317,   576,   577,   142,
      88,    89,    90,   198,   199,    91,   143,   200,    92,    93,
      94,   279,    95,    96,    97,    98,    99,   100,   101,   102
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -479
static const short int yypact[] =
{
     979,   -65,  -479,  -479,  -479,  -479,  -479,  -479,  4197,     8,
    4244,  -479,  -479,    94,    50,    37,  4344,  4344,    94,   259,
    -479,  3609,    -7,    62,    66,   134,   138,  2196,   172,   188,
     192,  2401,  3609,   238,  2094,  2094,    34,  2709,  3609,   685,
     327,   339,  2509,  1380,  2609,  -479,  -479,  -479,  -479,  -479,
    -479,  -479,  -479,  -479,    16,   269,   262,   303,  -479,  -479,
    -479,  -479,  -479,  -479,  -479,  -479,  -479,   477,  1272,  4097,
    -479,   409,   165,   420,    57,   343,   319,   349,   341,   438,
      32,  -479,  -479,  -479,  -479,  -479,   -19,  -479,  -479,  -479,
    -479,  -479,  -479,  -479,  -479,  -479,  -479,  -479,    94,  -479,
     473,  1087,  -479,  -479,    33,  2509,   388,   429,  -479,  2382,
    2509,  -479,   407,    83,  3609,  3609,  -479,  -479,   436,    54,
    -479,  -479,  -479,    85,  3609,   347,  1380,  -479,  3609,  3609,
    3609,  3609,   494,  3609,  -479,  -479,  -479,   109,   439,  2196,
    -479,   -25,  -479,  -479,  1584,   112,  1686,  3609,  2094,  -479,
    -479,  -479,  -479,   370,  -479,   450,  -479,  -479,  -479,   685,
     685,   927,   488,   492,  -479,     4,  -479,  -479,  -479,  -479,
    -479,  -479,  -479,  -479,  -479,  -479,  -479,  -479,  -479,  -479,
    -479,  -479,  -479,   760,   780,   490,     9,  -479,  1486,    94,
     503,  1458,   499,   -40,   503,   259,  -479,   276,  -479,  1482,
    1788,   489,  -479,  -479,   412,   271,   501,  2196,    94,  2809,
    -479,  2909,   248,    94,  -479,  -479,  3009,  3609,    94,  -479,
    -479,  -479,  -479,  -479,  -479,  -479,  -479,  -479,  -479,  -479,
    3609,  -479,  -479,  4097,  4097,  4097,  4097,  4097,  4097,  4097,
    4097,  4097,  4097,  4097,  4097,  4097,  4097,  4097,  4097,  4097,
    4097,  3609,  3609,  -479,   508,  -479,  -479,   522,  3109,   504,
     528,   530,    24,   282,    -7,   529,  -479,  3609,  -479,   259,
    -479,   500,  1191,   121,   227,   264,   367,   540,   541,   527,
    -479,  2196,  -479,  -479,  -479,  -479,  -479,  -479,   392,  1890,
    3209,  -479,  3609,   550,   558,   565,  -479,   559,   293,   433,
    -479,  1034,  1137,  3609,  2196,  1297,  -479,  -479,   568,  -479,
    -479,  4097,   -31,  3656,    13,   169,  -479,   222,  -479,   205,
    4097,  -479,  -479,  1992,  -479,  -479,  3409,  -479,   572,  3309,
    -479,  3609,  -479,  -479,    -7,   396,  -479,   402,    -7,   -22,
     581,  -479,  -479,   430,   470,  -479,  -479,  -479,  -479,  -479,
     409,   409,   165,   165,   420,   420,   420,   420,    57,    57,
     343,   319,   349,   341,   438,   510,  -479,   387,  -479,  -479,
     437,  1297,  -479,  -479,  -479,   514,    39,   582,  -479,  2509,
     444,  -479,   562,  2584,  -479,  -479,    94,  -479,  4097,  -479,
    -479,  1205,  -479,  -479,  -479,  1486,   503,  -479,    94,  1560,
      35,  -479,  -479,  3509,  2196,    -7,  2196,  3609,  3609,  -479,
    -479,  -479,  -479,  -479,   590,   685,  -479,  -479,   685,   584,
     465,  -479,   726,  -479,  -479,    81,  -479,   586,   588,   589,
    -479,   448,   521,  -479,   782,  1486,    94,  -479,   503,   587,
    -479,  -479,   591,   595,  -479,   222,   916,  3756,  -479,  -479,
    -479,  -479,  -479,   501,  -479,  -479,  -479,  -479,    -7,  3509,
    -479,  -479,  -479,    -7,  -479,  -479,  4097,    -7,   594,  -479,
    -479,  -479,   243,    -7,  -479,    39,   592,  -479,  -479,   195,
     596,   597,  2584,  -479,  -479,  -479,   596,   598,  -479,   195,
    -479,  -479,   214,  -479,   307,   245,   463,   631,  -479,  -479,
     485,   600,  -479,   616,  -479,  -479,  -479,  -479,  -479,   559,
    -479,   685,  -479,   685,    94,  -479,  2196,  -479,  -479,  -479,
    -479,  -479,  -479,    94,   503,  -479,  1486,    94,  1662,   611,
     609,  -479,   342,   503,  -479,  -479,  -479,   608,  -479,  -479,
    -479,  -479,    -7,  -479,  -479,    -7,  -479,    94,   292,    40,
     195,  1269,   614,   195,  -479,   -27,  3803,   307,  -479,  2196,
     602,  3609,  -479,  -479,   610,  -479,  -479,  -479,  -479,  -479,
     503,  2280,   281,  -479,   189,  -479,  -479,   314,  -479,  -479,
    -479,  -479,   619,  -479,  -479,   618,   620,   534,  -479,   297,
     603,  -479,  2298,   625,  1283,  -479,   628,    48,  -479,  -479,
     503,  4097,  2584,   633,   632,  -479,  -479,   629,  -479,  -479,
     637,   685,  -479,  -479,  -479,   638,   314,  1354,  3903,  -479,
    -479,  4097,  -479,   132,  -479,  -479,  4097,   841,  -479,  -479,
    -479,   649,  -479,  -479,   308,   635,  -479,  -479,  -479,   428,
     503,   -13,  -479,  2196,  -479,  -479,  -479,   640,  -479,   647,
    -479,  -479,  -479,  2584,  -479,   213,  -479,   195,  -479,  4097,
     335,  3950,  -479,  -479,   344,   194,  -479,  -479,  -479,  -479,
    -479,  -479,   195,  -479,   316,  -479,  -479,   634,   503,   655,
    -479,   651,   -21,  4050,   344,   654,  -479,  -479,  -479,  -479,
    -479,   503,   659,  -479,   656,  4097,  -479,   503,  -479,   642,
    -479,  -479
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -479,   -30,   -17,   561,     5,   143,   -23,   352,  -479,  -479,
    -479,  -479,  -479,  -479,  -479,  -479,   421,  -479,    -5,  -479,
      30,  -479,   328,  -479,  -479,   331,  -479,  -277,    43,    -8,
    -479,   -50,   406,   416,   326,   422,   509,   511,   513,   526,
     507,  -479,   148,  -479,  -479,  -258,   -15,  -228,  -479,   368,
    -479,  -479,   583,   604,   261,  -479,  -479,  -479,  -450,  -185,
     -32,   364,  -479,  -424,  -479,  -479,   184,   186,  -479,    63,
    -206,  -275,   127,  -179,  -479,  -479,   160,  -347,  -478,   124,
     130,   -69,   114,  -479,   156,   -79,  -479,   607,   183,  -479,
     230,  -479,   274,  -479,  -479,  -186,  -314,   232,   234,    49,
    -479,  -479,  -479,   612,  -479,   318,   -74,   -20,  -479,  -479,
    -479,  -380,  -479,  -479,  -479,  -479,  -479,  -479,  -479,   709
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -387
static const short int yytable[] =
{
     108,   445,   111,   307,   123,    54,   149,   316,   116,   117,
     188,   381,   183,   413,   144,   146,   163,   557,   112,   232,
     168,   169,   527,   118,   121,   184,   257,     2,   501,   548,
     260,   261,    54,   303,   164,   377,     2,     2,     2,    54,
      54,   462,     2,     2,   162,   103,   312,   185,    54,    87,
     667,     2,   413,   493,   313,   312,   438,     2,   314,     6,
     600,   231,   250,   313,   283,   252,   691,   314,   269,   165,
     285,   252,   285,   188,   149,   183,   132,   188,   188,   183,
     183,   126,   240,   241,   601,   253,     2,   205,   184,   268,
     601,   253,   184,   184,   109,   304,   306,     2,   601,   252,
     589,   104,   119,   254,   527,   191,    54,   207,   479,   208,
     185,   314,   104,   374,   259,   185,   104,   104,   264,   147,
     147,   489,   148,   114,   315,   104,   285,   126,   289,   584,
     585,    54,   493,   314,   489,     2,   163,   163,   163,   468,
     267,   251,   552,    55,    54,   490,   314,   527,   128,    54,
      87,    54,   129,    54,   164,   164,   164,   113,   633,   188,
     104,   183,   242,   243,   162,   162,   162,   301,   191,   263,
      55,   104,   191,   191,   184,   269,   511,    55,    55,   323,
     319,   610,   413,   347,   348,   349,    55,   557,   282,   293,
     294,   297,     2,   527,   308,   270,   259,     2,     2,   252,
     121,   286,   252,   493,    54,    54,   599,   550,   402,   104,
     307,   403,    54,   333,   381,   285,     2,   121,   341,   280,
     130,   651,   253,   345,   131,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   315,    55,   444,     2,   375,     2,   285,
     307,     2,   335,   309,   337,   312,   332,   318,   133,   343,
     445,   437,     2,   313,   236,   237,   104,   121,   121,    55,
     449,   104,   104,   435,   121,   572,   597,   390,   163,   163,
     660,   489,    55,   313,     2,     2,    54,    55,   661,    55,
     104,    55,   662,   314,    54,   269,   164,   164,   134,   489,
     476,   370,   135,   231,   553,   231,   425,   162,   446,    54,
     434,   314,   231,   552,   404,   448,   447,   252,   567,   597,
     104,   119,   104,   673,   554,   104,   119,   639,    54,   139,
     409,   489,   543,   166,   328,   338,   104,   119,     2,   435,
     127,   307,    55,    55,   167,     2,   575,   188,   211,   183,
      55,   405,   655,   433,   252,   209,   419,   126,   104,   104,
     119,   329,   184,   330,   612,   320,   252,   572,   438,   376,
     244,   245,   121,   210,   522,   313,   470,   443,   672,   314,
     231,   473,   553,   420,   185,   421,   253,   553,   390,   212,
       2,   480,   163,   555,   637,   163,   390,   163,   657,   163,
     617,   556,   583,   486,   390,   491,   657,   624,   618,    54,
     164,    54,   104,   164,   526,   164,   246,   164,   658,   104,
     162,   660,   678,   162,    55,   162,   686,   162,   572,   661,
     682,     2,    55,   314,   666,   272,   313,   252,   683,   231,
     314,   517,   191,   238,   239,   503,   496,    55,   504,   248,
     507,   525,   509,   497,   406,   499,   247,   252,   231,   485,
     290,   441,   291,   574,   104,   119,    55,   249,     2,   156,
     157,   158,   688,   255,   467,     7,   675,   121,   676,   410,
     545,   208,   252,   458,   491,   696,   459,   390,   163,   460,
     163,   700,   459,   685,   491,   262,   526,   213,   214,   215,
     491,   518,   326,   574,   327,   104,   164,   233,   164,   528,
     168,   169,   234,   235,   660,   258,   162,   464,   162,   564,
     459,    54,   661,   422,   469,   423,   314,   459,   566,   252,
     281,   477,   568,   525,   252,   515,   441,   573,   513,   526,
     292,   563,   104,   507,    37,   159,   266,    55,   231,    55,
     558,   160,   582,   459,   586,   491,   593,   506,   491,   161,
     252,   277,   465,   216,    54,   565,   354,   355,   356,   357,
     665,   217,   560,   218,   301,   252,   525,   573,   302,   573,
     120,   325,   378,   305,   231,   526,   311,   518,   163,   320,
     252,   665,   331,   231,   367,   441,   578,   371,   151,   593,
     252,   466,   491,   471,   472,   151,   164,   390,   608,   368,
     231,   252,   516,   231,   540,   372,   162,   373,   231,   379,
     154,   155,   525,   622,   623,   125,   407,   206,   586,   428,
     431,   252,   390,   518,   528,   137,   138,   408,   141,   145,
     415,   644,   350,   351,   491,   416,   186,   197,    54,   482,
     418,   231,   457,   231,   352,   353,   461,   417,   390,    55,
     491,   436,   491,   518,   454,   491,   358,   359,   463,   475,
     491,   502,   478,   512,   520,   231,   505,   491,   513,   514,
     528,   542,   547,   521,   -11,   549,   551,   231,     2,   156,
     157,   158,   669,   314,   474,     7,   559,   562,   570,   571,
     579,   598,    55,   518,   441,   611,   619,   620,   537,   186,
     561,   273,   609,   625,   186,   621,   628,   632,   265,   186,
     640,   642,   641,   498,   643,   645,   659,   670,   271,     2,
     156,   157,   158,   274,   275,   276,     7,   278,   656,   671,
     441,   518,   689,   690,   687,   695,   697,   452,   698,   441,
     487,   288,   701,   510,   518,   360,   120,   364,   361,   484,
     518,   362,   104,   488,    37,   159,   441,  -289,  -289,   441,
     151,   160,   151,   120,   441,   363,   538,   151,   630,   161,
     631,   539,   674,   652,   679,   541,    55,  -292,  -292,  -288,
    -288,   544,   154,   546,   154,   684,   692,   668,   310,   154,
     647,   614,   569,   104,   615,    37,   159,   441,   616,   441,
     256,   322,   160,   346,     0,     0,     0,   607,   508,   151,
     161,   344,     0,   120,   120,     0,     0,     0,     0,     0,
     120,   441,     0,   389,     0,   366,     0,     0,     0,     0,
       0,   154,     0,   441,     2,     0,    -7,    -7,   168,   169,
      -7,   151,     0,   627,    -7,   365,     0,     0,    -7,     0,
     580,     0,   638,   581,     0,     0,    -8,    -8,   -40,   -40,
      -8,   380,   -40,   154,    -8,   414,   -40,     0,    -8,   649,
     -40,     0,   650,     0,     0,     0,     0,   653,     0,     0,
     151,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   384,   385,   386,     0,     0,     0,   432,     0,   453,
       0,     0,   154,     0,   456,     0,     0,     0,   104,     2,
     677,     0,   681,   168,   169,     0,     0,     0,   120,     0,
       2,   156,   157,   158,     0,     0,     0,     7,     0,     0,
       0,     0,     0,     0,   694,     0,     0,     0,     0,     0,
       0,   654,   389,     0,     0,     0,   699,     0,     0,     0,
       0,     0,   366,     0,   151,     0,   170,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   384,   385,   523,  -386,
       1,     0,     2,     3,     4,     5,   154,     0,     6,     7,
       8,     9,    10,   104,    11,    12,    13,    14,    15,     0,
      16,    17,     0,   524,   104,     0,    37,   159,     0,     0,
       0,   500,   278,   160,     0,     0,     0,     0,   295,   296,
     151,   161,    18,    19,     0,    20,    21,    22,    23,     0,
       0,     0,     0,   120,     0,     0,     0,     2,   156,   157,
     158,     0,   154,    24,     7,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,     0,    37,    38,
       0,     0,    39,    40,    41,    42,     0,    43,     0,     0,
       0,     0,     0,    44,     0,     0,    45,    46,    47,    48,
      49,    50,     0,     0,     0,     0,     0,  -385,     1,    51,
       2,     3,     4,     5,     0,     0,     6,     7,     8,     9,
      10,     0,    11,    12,    13,    14,    15,     0,    16,    17,
       0,   104,     0,    37,   159,     0,     0,     0,     0,     0,
     160,   424,     0,     0,     0,     0,     0,     0,   161,     0,
      18,    19,     0,    20,    21,    22,    23,     0,     0,     0,
       2,   156,   157,   158,     0,     0,     0,     7,     0,     0,
       0,    24,     0,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,   278,    37,    38,     0,     0,
      39,    40,    41,    42,     0,    43,     0,     0,     0,     0,
       0,    44,     0,     0,    45,    46,    47,    48,    49,    50,
       0,     0,   382,     0,     2,     0,     0,    51,   168,   169,
       0,     0,     0,     0,     0,   383,   382,     0,     2,     0,
       0,     0,   168,   169,   104,     0,    37,   159,     0,   383,
       0,     0,     0,   160,   430,     0,     0,     0,     0,     0,
       0,   161,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   384,   385,   386,     0,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   384,   385,   386,   104,   119,
     590,     0,     2,     0,     0,     0,   168,   169,     0,     0,
     387,     0,   104,   119,   590,     0,     2,     0,     0,     0,
     168,   169,     0,     0,   483,     0,     0,     0,     0,     0,
       2,     0,   388,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,     0,     0,     0,   388,     0,     0,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   384,
     385,   386,     0,   170,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   384,   385,   386,   104,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,     2,   591,     0,
     104,   168,   169,     0,     0,     0,     0,   229,     0,     0,
       0,     0,   629,     0,   104,     0,     0,     0,     0,     0,
     592,     1,     0,     2,     3,     4,     5,     0,     0,     6,
       7,     8,     9,    10,   592,    11,    12,    13,    14,    15,
       0,    16,    17,     0,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   384,   385,   523,     0,     0,     0,
       0,     0,     0,    18,    19,   195,    20,    21,    22,    23,
       0,   104,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   646,     0,     0,    24,     0,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,   119,    37,
      38,     2,     0,    39,    40,    41,    42,     0,    43,   196,
       0,     0,     0,     0,    44,     0,     0,    45,    46,    47,
      48,    49,    50,     1,     0,     2,     3,     4,     5,     0,
      51,     6,     7,     8,     9,    10,     0,    11,    12,    13,
      14,    15,     0,    16,    17,     0,     0,     0,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,     0,     0,     0,     0,    18,    19,   195,    20,    21,
      22,    23,     0,     0,     0,   104,   170,   171,   172,   173,
     174,   175,   176,   177,   178,   179,    24,     0,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
     119,    37,    38,     2,     0,    39,    40,    41,    42,     0,
      43,   321,     0,     0,     0,     0,    44,     0,     0,    45,
      46,    47,    48,    49,    50,     1,     0,     2,     3,     4,
       5,     0,    51,     6,     7,     8,     9,    10,     0,    11,
      12,    13,    14,    15,     0,    16,    17,     0,     0,     0,
     170,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     384,   385,   386,     0,     0,     0,     0,    18,    19,     0,
      20,    21,    22,    23,     0,     0,     0,   104,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    24,     0,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,   119,    37,    38,     2,     0,    39,    40,    41,
      42,     0,    43,   284,     0,     0,     0,     0,    44,     0,
       0,    45,    46,    47,    48,    49,    50,     1,     0,     2,
       3,     4,     5,     0,    51,     6,     7,     8,     9,    10,
       0,    11,    12,    13,    14,    15,     0,    16,    17,     0,
       0,     0,   170,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   384,   385,   523,     0,     0,     0,     0,    18,
      19,     0,    20,    21,    22,    23,     0,     0,     0,   104,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      24,     0,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,   119,    37,    38,     0,     0,    39,
      40,    41,    42,     0,    43,   287,     0,     0,     0,     0,
      44,     0,     0,    45,    46,    47,    48,    49,    50,     1,
       0,     2,     3,     4,     5,     0,    51,     6,     7,     8,
       9,    10,     0,    11,    12,    13,    14,    15,     0,    16,
      17,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    18,    19,     0,    20,    21,    22,    23,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    24,     0,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,   119,    37,    38,     0,
       0,    39,    40,    41,    42,     0,    43,   324,     0,     0,
       0,     0,    44,     0,     0,    45,    46,    47,    48,    49,
      50,     1,     0,     2,     3,     4,     5,     0,    51,     6,
       7,     8,     9,    10,     0,    11,    12,    13,    14,    15,
       0,    16,    17,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    18,    19,     0,    20,    21,    22,    23,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    24,     0,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,   119,    37,
      38,     0,     0,    39,    40,    41,    42,     0,    43,   411,
       0,     0,     0,     0,    44,     0,     0,    45,    46,    47,
      48,    49,    50,     1,     0,     2,     3,     4,     5,     0,
      51,     6,     7,     8,     9,    10,     0,    11,    12,    13,
      14,    15,     0,    16,    17,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    18,    19,     0,    20,    21,
      22,    23,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    24,     0,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
     119,    37,    38,     0,     0,    39,    40,    41,    42,     0,
      43,   450,     0,     0,     0,     0,    44,     0,     0,    45,
      46,    47,    48,    49,    50,     1,     0,     2,     3,     4,
       5,     0,    51,     6,     7,     8,     9,    10,     0,    11,
      12,    13,    14,    15,     0,    16,    17,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    18,    19,     0,
      20,    21,    22,    23,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    24,     0,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,   119,    37,    38,     0,     0,    39,    40,    41,
      42,     0,    43,     0,     0,     0,     0,     0,    44,     0,
       0,    45,    46,    47,    48,    49,    50,     1,     0,     2,
       3,     4,     5,     0,    51,     6,     7,     8,     9,    10,
       0,    11,    12,    13,    14,    15,     0,    16,    17,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    18,
      19,     0,    20,    21,    22,    23,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      24,     0,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,     0,    37,    38,     0,     0,    39,
      40,    41,    42,     2,    43,     0,     0,   168,   169,     0,
      44,     0,     0,    45,    46,    47,    48,    49,    50,     0,
       0,     2,     3,     4,     5,     0,    51,     6,     7,     8,
       9,    10,     0,     0,     0,    13,     0,    15,     0,    16,
      17,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     170,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     384,   385,   523,   613,    20,     0,    22,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   104,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    34,    35,   104,     0,    37,    38,     0,
       0,     0,    40,    41,    42,     2,   124,     0,     0,   168,
     169,     0,    44,     0,     0,    45,    46,    47,    48,    49,
      50,     0,     0,     0,     2,     3,     4,     5,     0,   626,
       6,     7,     8,     9,    10,     0,     0,     0,    13,     0,
      15,     0,    16,    17,     0,     0,     0,     0,     0,     0,
       0,     0,   170,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,     0,     0,    20,    21,    22,
      23,     0,     0,     0,     0,     0,     0,     0,     0,   104,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    34,    35,   104,     0,
      37,    38,     0,     0,     0,    40,    41,    42,     0,   124,
       0,     0,     0,     0,     0,    44,     0,     0,    45,    46,
      47,    48,    49,    50,     0,     0,     0,     0,     0,     0,
       0,   136,     2,     3,     4,     5,   168,   169,     6,     7,
       8,     9,    10,     0,     0,     0,    13,     0,    15,     0,
      16,    17,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    20,    21,    22,    23,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    35,   104,     2,    37,    38,
       0,   168,   169,    40,    41,    42,     0,   124,     0,     0,
       0,     0,     0,    44,     0,     0,    45,    46,    47,    48,
      49,    50,     2,     3,     4,     5,     0,     0,     6,     7,
       8,     9,    10,     0,     0,     0,    13,     0,    15,     0,
      16,    17,     0,     0,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   384,   385,   386,     0,     0,     0,
       0,     0,     0,     0,     0,    20,    21,    22,    23,     0,
       0,   104,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    35,   104,   119,    37,    38,
       0,     0,     0,    40,    41,    42,     0,   124,     0,     0,
     201,   202,     0,    44,     0,     0,    45,    46,    47,    48,
      49,    50,     2,     3,     4,     5,     0,     0,     6,     7,
       8,     9,    10,     0,     0,     0,    13,     0,    15,     0,
      16,    17,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    20,    21,    22,    23,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    35,   104,   119,    37,    38,
       0,     0,     0,    40,    41,    42,     0,   124,     0,     0,
       0,   150,     0,    44,     0,     0,    45,    46,    47,    48,
      49,    50,     2,     3,     4,     5,     0,     0,     6,     7,
       8,     9,    10,     0,     0,     0,    13,     0,    15,     0,
      16,    17,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    20,    21,    22,    23,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    35,   104,   119,    37,    38,
       0,     0,     0,    40,    41,    42,   334,   124,     0,     0,
       0,     0,     0,    44,     0,     0,    45,    46,    47,    48,
      49,    50,     2,     3,     4,     5,     0,     0,     6,     7,
       8,     9,    10,     0,     0,     0,    13,     0,    15,     0,
      16,    17,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    20,    21,    22,    23,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    35,   104,   119,    37,    38,
       0,     0,     0,    40,    41,    42,   336,   124,     0,     0,
       0,     0,     0,    44,     0,     0,    45,    46,    47,    48,
      49,    50,     2,     3,     4,     5,     0,     0,     6,     7,
       8,     9,    10,     0,     0,     0,    13,     0,    15,     0,
      16,    17,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    20,    21,    22,    23,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    35,   104,   119,    37,    38,
       0,     0,     0,    40,    41,    42,   342,   124,     0,     0,
       0,     0,     0,    44,     0,     0,    45,    46,    47,    48,
      49,    50,     2,     3,     4,     5,     0,     0,     6,     7,
       8,     9,    10,     0,     0,     0,    13,     0,    15,     0,
      16,    17,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    20,    21,    22,    23,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    35,   104,   119,    37,    38,
       0,     0,     0,    40,    41,    42,   369,   124,     0,     0,
       0,     0,     0,    44,     0,     0,    45,    46,    47,    48,
      49,    50,     2,     3,     4,     5,     0,     0,     6,     7,
       8,     9,    10,     0,     0,     0,    13,     0,    15,     0,
      16,    17,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    20,    21,    22,    23,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    35,   104,   119,    37,    38,
       0,     0,     0,    40,    41,    42,     0,   124,     0,     0,
       0,   412,     0,    44,     0,     0,    45,    46,    47,    48,
      49,    50,     2,     3,     4,     5,     0,     0,     6,     7,
       8,     9,    10,     0,     0,     0,    13,     0,    15,     0,
      16,    17,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    20,    21,    22,    23,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    35,   104,   119,    37,    38,
       0,     0,     0,    40,    41,    42,     0,   124,     0,     0,
       0,   455,     0,    44,     0,     0,    45,    46,    47,    48,
      49,    50,     2,     3,     4,     5,     0,     0,     6,     7,
       8,     9,    10,     0,     0,     0,    13,     0,    15,     0,
      16,    17,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    20,    21,    22,    23,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    35,   104,     0,    37,    38,
       0,     0,     0,    40,    41,    42,     0,   124,     0,     0,
       0,   451,     0,    44,     0,     0,    45,    46,    47,    48,
      49,    50,     2,     3,     4,     5,     0,     0,     6,     7,
       8,     9,    10,     0,     0,     0,    13,     0,    15,     0,
      16,    17,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    20,    21,    22,    23,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    35,   104,   119,    37,    38,
       0,     0,     0,    40,    41,    42,     0,   124,     0,     0,
       0,     0,     0,    44,     0,     0,    45,    46,    47,    48,
      49,    50,     2,     3,     4,     5,     0,     0,     6,     7,
       8,     9,    10,     0,     0,     0,    13,     0,    15,     0,
      16,    17,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    20,    21,    22,    23,     2,
       3,     4,     5,     0,     0,     6,     7,     8,     9,    10,
       0,     0,     0,    13,     0,    15,     0,    16,    17,     0,
       0,     0,     0,     0,    34,    35,   104,     0,    37,    38,
       0,     0,     0,    40,    41,    42,     0,   124,     0,     0,
       0,     0,    20,    44,    22,     0,    45,    46,    47,    48,
      49,    50,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    34,    35,   104,     0,    37,    38,     0,     0,     0,
      40,    41,    42,     0,   124,     0,     0,     0,   440,     0,
      44,     0,     0,    45,    46,    47,    48,    49,    50,     2,
       3,     4,     5,     0,     0,     6,     7,     8,     9,    10,
       0,     0,     0,    13,     0,    15,     0,    16,    17,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    20,     0,    22,     0,     2,     3,     4,     5,
       0,     0,     6,     7,     8,     9,    10,     0,     0,     0,
      13,     0,    15,     0,    16,    17,     0,     0,     0,     0,
       0,    34,    35,   104,     0,    37,    38,     0,     0,     0,
      40,    41,    42,     0,   124,     0,     0,     0,   536,    20,
      44,    22,     0,    45,    46,    47,    48,    49,    50,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    35,
     104,     0,    37,    38,     0,     0,     0,    40,    41,    42,
       0,   124,     0,     0,     0,   606,     0,    44,     0,     0,
      45,    46,    47,    48,    49,    50,     2,     3,     4,     5,
       0,     0,     6,     7,     8,     9,    10,     0,     0,     0,
      13,     0,    15,     0,    16,    17,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    20,
       0,    22,     0,     2,     3,     4,     5,     0,     0,     6,
       7,     8,     9,    10,     0,     0,     0,    13,     0,    15,
       0,    16,    17,     0,     0,     0,     0,     0,    34,    35,
     104,     0,    37,    38,     0,     0,     0,    40,    41,    42,
       0,   124,     0,     0,     0,   648,    20,    44,    22,     0,
      45,    46,    47,    48,    49,    50,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    34,    35,   104,     0,    37,
      38,     0,     0,     0,    40,    41,    42,     0,   124,     0,
       0,     0,   680,     0,    44,     0,     0,    45,    46,    47,
      48,    49,    50,     2,     3,     4,     5,     0,     0,     6,
       7,     8,     9,    10,     0,     0,     0,    13,     0,    15,
       0,    16,    17,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    20,     0,    22,     0,
       2,     3,     4,     5,     0,     0,     6,     7,     8,     9,
      10,     0,     0,     0,    13,     0,    15,     0,    16,    17,
       0,     0,     0,     0,     0,    34,    35,   104,     0,    37,
      38,     0,     0,     0,    40,    41,    42,     0,   124,     0,
       0,     0,   693,    20,    44,    22,     0,    45,    46,    47,
      48,    49,    50,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    34,    35,   104,     0,    37,    38,     0,     0,
       0,    40,    41,    42,     0,   124,     0,     0,     0,     0,
       0,    44,     0,     0,    45,    46,    47,    48,    49,    50,
       2,     3,     4,     5,     0,     0,     6,     7,     8,     9,
      10,     0,     0,     0,    13,     0,    15,     0,    16,    17,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    20,     0,    22,     0,     2,     3,     4,
       5,     0,     0,     6,     7,     8,     9,    10,     0,     0,
       0,    13,     0,    15,     0,    16,    17,     0,     0,     0,
       0,     0,    34,    35,   104,     0,    37,    38,     0,     0,
       0,    40,    41,   105,     0,     0,     0,     0,     0,     0,
      20,    44,    22,     0,    45,    46,    47,    48,    49,    50,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    34,
      35,   104,     0,    37,    38,     0,     0,     0,    40,    41,
     110,     0,     0,     0,     0,     0,     0,     0,    44,     0,
       0,    45,    46,    47,    48,    49,    50,     2,     3,     4,
       5,     0,     0,     6,     7,     8,     9,    10,     0,     0,
       0,    13,     0,    15,     0,    16,    17,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      20,     0,    22,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    34,
      35,   104,     0,    37,    38,     0,     0,     0,    40,    41,
     115,     0,     0,     0,     0,     0,     0,     0,    44,     0,
       0,    45,    46,    47,    48,    49,    50
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    31,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     1,     3,     0,     0,
       5,     0,     0,     0,     7,     0,     0,     0,     9,     0,
       0,     0,     0,     0,     0,     0,    11,    13,    21,    23,
      15,     0,    25,     0,    17,     0,    27,     0,    19,     0,
      29,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
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
       0,   289,     0,   289,     0,   289,     0,   289,     0,   289,
       0,   292,     0,   292,     0,   292,     0,   292,     0,   292,
       0,   288,     0,   288,     0,   288,     0,   288,     0,   288,
       0,   358,     0
};

static const short int yycheck[] =
{
       8,   315,    10,   188,    19,     0,    36,   193,    16,    17,
      42,   269,    42,   290,    34,    35,    39,   495,    13,    69,
       7,     8,   446,    18,    19,    42,   105,     3,   408,   479,
     109,   110,    27,    29,    39,   263,     3,     3,     3,    34,
      35,    63,     3,     3,    39,   110,    86,    42,    43,     0,
      63,     3,   329,   400,    94,    86,    87,     3,    98,     9,
      87,    69,    30,    94,    89,    90,    87,    98,    90,    39,
     144,    90,   146,   105,   104,   105,    27,   109,   110,   109,
     110,    88,    25,    26,   111,   110,     3,    44,   105,   119,
     111,   110,   109,   110,    86,    91,    87,     3,   111,    90,
     550,    77,    78,    98,   528,    42,   101,    91,   383,    93,
     105,    98,    77,    89,   109,   110,    77,    77,   113,    86,
      86,    86,    88,    86,   193,    77,   200,    88,   148,    89,
      90,   126,   479,    98,    86,     3,   159,   160,   161,   367,
      86,   109,   489,     0,   139,   110,    98,   571,    86,   144,
     101,   146,    86,   148,   159,   160,   161,    14,   110,   191,
      77,   191,   105,   106,   159,   160,   161,    86,   105,    86,
      27,    77,   109,   110,   191,    90,    95,    34,    35,   199,
     195,   561,   459,   233,   234,   235,    43,   665,   139,   159,
     160,   161,     3,   617,   189,   110,   191,     3,     3,    90,
     195,    89,    90,   550,   199,   200,   553,   482,    87,    77,
     395,    90,   207,   208,   472,   289,     3,   212,   213,   110,
      86,    89,   110,   218,    86,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   244,   245,   246,   247,
     248,   249,   250,   312,   101,   314,     3,   262,     3,   323,
     435,     3,   209,   190,   211,    86,   207,   194,    86,   216,
     574,   311,     3,    94,    99,   100,    77,   262,   263,   126,
     320,    77,    77,   305,   269,    86,   551,   272,   301,   302,
      86,    86,   139,    94,     3,     3,   281,   144,    94,   146,
      77,   148,   639,    98,   289,    90,   301,   302,   110,    86,
     379,   258,   110,   311,    90,   313,   301,   302,    86,   304,
     305,    98,   320,   660,    87,   110,    94,    90,   524,   594,
      77,    78,    77,   110,   110,    77,    78,   602,   323,    91,
     281,    86,    89,     6,    63,    87,    77,    78,     3,   371,
      22,   526,   199,   200,     5,     3,   532,   379,    86,   379,
     207,    87,   627,   304,    90,    86,    63,    88,    77,    77,
      78,    90,   379,    92,   570,    89,    90,    86,    87,    87,
      27,    28,   367,    55,   443,    94,   371,   314,   653,    98,
     388,   376,    90,    90,   379,    92,   110,    90,   383,    86,
       3,   386,   415,    86,   600,   418,   391,   420,    90,   422,
      86,    94,   110,   398,   399,   400,    90,   110,    94,   404,
     415,   406,    77,   418,   446,   420,    97,   422,   110,    77,
     415,    86,    87,   418,   281,   420,   110,   422,    86,    94,
      86,     3,   289,    98,   640,    88,    94,    90,    94,   447,
      98,   436,   379,    23,    24,   415,   403,   304,   418,   108,
     420,   446,   422,   404,    87,   406,   107,    90,   466,   396,
      90,   313,    92,   532,    77,    78,   323,    29,     3,     4,
       5,     6,   678,     0,    87,    10,   655,   472,   657,    87,
     475,    93,    90,    87,   479,   691,    90,   482,   511,    87,
     513,   697,    90,   672,   489,    88,   528,    20,    21,    22,
     495,   438,    90,   572,    92,    77,   511,    98,   513,   446,
       7,     8,   103,   104,    86,    86,   511,    87,   513,   514,
      90,   516,    94,    90,    87,    92,    98,    90,   523,    90,
      91,    87,   527,   528,    90,    87,   388,   532,    90,   571,
      90,   511,    77,   513,    79,    80,   110,   404,   556,   406,
      87,    86,   547,    90,   549,   550,   551,    92,   553,    94,
      90,    67,    92,    86,   559,   516,   240,   241,   242,   243,
     639,    94,    87,    96,    86,    90,   571,   572,    86,   574,
      19,    92,   264,    93,   592,   617,    87,   524,   611,    89,
      90,   660,    91,   601,    86,   447,   533,    93,    37,   594,
      90,    91,   597,    89,    90,    44,   611,   602,   559,    87,
     618,    90,    91,   621,   466,    87,   611,    87,   626,    90,
      37,    38,   617,    89,    90,    21,    86,    44,   623,   301,
     302,    90,   627,   570,   571,    31,    32,   110,    34,    35,
      90,   611,   236,   237,   639,    87,    42,    43,   643,   388,
      91,   659,   334,   661,   238,   239,   338,    92,   653,   516,
     655,    93,   657,   600,    92,   660,   244,   245,    87,    87,
     665,    81,   110,    87,    87,   683,    92,   672,    90,    90,
     617,    87,    90,    92,    88,    88,    88,   695,     3,     4,
       5,     6,   643,    98,   376,    10,    65,    81,    87,    90,
      92,    87,   559,   640,   556,    95,    87,    89,   447,   105,
     110,   128,   110,   110,   110,    95,    91,    89,   114,   115,
      87,    92,    90,   405,    87,    87,    91,    87,   124,     3,
       4,     5,     6,   129,   130,   131,    10,   133,    89,    92,
     592,   678,    87,    92,   110,    91,    87,   326,    92,   601,
     398,   147,   110,   422,   691,   246,   195,   250,   247,   391,
     697,   248,    77,   399,    79,    80,   618,     7,     8,   621,
     209,    86,   211,   212,   626,   249,   458,   216,   594,    94,
     594,   463,   655,   623,   660,   467,   643,     7,     8,     7,
       8,   473,   209,   475,   211,   665,   682,   641,   191,   216,
     617,   571,   528,    77,   572,    79,    80,   659,   574,   661,
     101,   199,    86,   230,    -1,    -1,    -1,   556,    92,   258,
      94,   217,    -1,   262,   263,    -1,    -1,    -1,    -1,    -1,
     269,   683,    -1,   272,    -1,   252,    -1,    -1,    -1,    -1,
      -1,   258,    -1,   695,     3,    -1,    86,    87,     7,     8,
      90,   290,    -1,   592,    94,   251,    -1,    -1,    98,    -1,
     542,    -1,   601,   545,    -1,    -1,    86,    87,    86,    87,
      90,   267,    90,   290,    94,   292,    94,    -1,    98,   618,
      98,    -1,   621,    -1,    -1,    -1,    -1,   626,    -1,    -1,
     329,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    -1,    -1,    -1,   303,    -1,   326,
      -1,    -1,   329,    -1,   331,    -1,    -1,    -1,    77,     3,
     659,    -1,   661,     7,     8,    -1,    -1,    -1,   367,    -1,
       3,     4,     5,     6,    -1,    -1,    -1,    10,    -1,    -1,
      -1,    -1,    -1,    -1,   683,    -1,    -1,    -1,    -1,    -1,
      -1,   110,   391,    -1,    -1,    -1,   695,    -1,    -1,    -1,
      -1,    -1,   379,    -1,   403,    -1,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,     0,
       1,    -1,     3,     4,     5,     6,   403,    -1,     9,    10,
      11,    12,    13,    77,    15,    16,    17,    18,    19,    -1,
      21,    22,    -1,    87,    77,    -1,    79,    80,    -1,    -1,
      -1,   407,   408,    86,    -1,    -1,    -1,    -1,    91,    92,
     459,    94,    43,    44,    -1,    46,    47,    48,    49,    -1,
      -1,    -1,    -1,   472,    -1,    -1,    -1,     3,     4,     5,
       6,    -1,   459,    64,    10,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    -1,    79,    80,
      -1,    -1,    83,    84,    85,    86,    -1,    88,    -1,    -1,
      -1,    -1,    -1,    94,    -1,    -1,    97,    98,    99,   100,
     101,   102,    -1,    -1,    -1,    -1,    -1,     0,     1,   110,
       3,     4,     5,     6,    -1,    -1,     9,    10,    11,    12,
      13,    -1,    15,    16,    17,    18,    19,    -1,    21,    22,
      -1,    77,    -1,    79,    80,    -1,    -1,    -1,    -1,    -1,
      86,    87,    -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,
      43,    44,    -1,    46,    47,    48,    49,    -1,    -1,    -1,
       3,     4,     5,     6,    -1,    -1,    -1,    10,    -1,    -1,
      -1,    64,    -1,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,   561,    79,    80,    -1,    -1,
      83,    84,    85,    86,    -1,    88,    -1,    -1,    -1,    -1,
      -1,    94,    -1,    -1,    97,    98,    99,   100,   101,   102,
      -1,    -1,     1,    -1,     3,    -1,    -1,   110,     7,     8,
      -1,    -1,    -1,    -1,    -1,    14,     1,    -1,     3,    -1,
      -1,    -1,     7,     8,    77,    -1,    79,    80,    -1,    14,
      -1,    -1,    -1,    86,    87,    -1,    -1,    -1,    -1,    -1,
      -1,    94,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    -1,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    77,    78,
       1,    -1,     3,    -1,    -1,    -1,     7,     8,    -1,    -1,
      89,    -1,    77,    78,     1,    -1,     3,    -1,    -1,    -1,
       7,     8,    -1,    -1,    89,    -1,    -1,    -1,    -1,    -1,
       3,    -1,   111,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    -1,    -1,    -1,   111,    -1,    -1,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    -1,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    77,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,     3,    89,    -1,
      77,     7,     8,    -1,    -1,    -1,    -1,    95,    -1,    -1,
      -1,    -1,    89,    -1,    77,    -1,    -1,    -1,    -1,    -1,
     111,     1,    -1,     3,     4,     5,     6,    -1,    -1,     9,
      10,    11,    12,    13,   111,    15,    16,    17,    18,    19,
      -1,    21,    22,    -1,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    44,    45,    46,    47,    48,    49,
      -1,    77,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    87,    -1,    -1,    64,    -1,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,     3,    -1,    83,    84,    85,    86,    -1,    88,    89,
      -1,    -1,    -1,    -1,    94,    -1,    -1,    97,    98,    99,
     100,   101,   102,     1,    -1,     3,     4,     5,     6,    -1,
     110,     9,    10,    11,    12,    13,    -1,    15,    16,    17,
      18,    19,    -1,    21,    22,    -1,    -1,    -1,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    -1,    -1,    -1,    -1,    43,    44,    45,    46,    47,
      48,    49,    -1,    -1,    -1,    77,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    64,    -1,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,     3,    -1,    83,    84,    85,    86,    -1,
      88,    89,    -1,    -1,    -1,    -1,    94,    -1,    -1,    97,
      98,    99,   100,   101,   102,     1,    -1,     3,     4,     5,
       6,    -1,   110,     9,    10,    11,    12,    13,    -1,    15,
      16,    17,    18,    19,    -1,    21,    22,    -1,    -1,    -1,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    -1,    -1,    -1,    -1,    43,    44,    -1,
      46,    47,    48,    49,    -1,    -1,    -1,    77,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,    -1,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,     3,    -1,    83,    84,    85,
      86,    -1,    88,    89,    -1,    -1,    -1,    -1,    94,    -1,
      -1,    97,    98,    99,   100,   101,   102,     1,    -1,     3,
       4,     5,     6,    -1,   110,     9,    10,    11,    12,    13,
      -1,    15,    16,    17,    18,    19,    -1,    21,    22,    -1,
      -1,    -1,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    -1,    -1,    -1,    -1,    43,
      44,    -1,    46,    47,    48,    49,    -1,    -1,    -1,    77,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      64,    -1,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    -1,    -1,    83,
      84,    85,    86,    -1,    88,    89,    -1,    -1,    -1,    -1,
      94,    -1,    -1,    97,    98,    99,   100,   101,   102,     1,
      -1,     3,     4,     5,     6,    -1,   110,     9,    10,    11,
      12,    13,    -1,    15,    16,    17,    18,    19,    -1,    21,
      22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    44,    -1,    46,    47,    48,    49,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    64,    -1,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    -1,
      -1,    83,    84,    85,    86,    -1,    88,    89,    -1,    -1,
      -1,    -1,    94,    -1,    -1,    97,    98,    99,   100,   101,
     102,     1,    -1,     3,     4,     5,     6,    -1,   110,     9,
      10,    11,    12,    13,    -1,    15,    16,    17,    18,    19,
      -1,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    44,    -1,    46,    47,    48,    49,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    64,    -1,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    -1,    -1,    83,    84,    85,    86,    -1,    88,    89,
      -1,    -1,    -1,    -1,    94,    -1,    -1,    97,    98,    99,
     100,   101,   102,     1,    -1,     3,     4,     5,     6,    -1,
     110,     9,    10,    11,    12,    13,    -1,    15,    16,    17,
      18,    19,    -1,    21,    22,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    43,    44,    -1,    46,    47,
      48,    49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    64,    -1,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    83,    84,    85,    86,    -1,
      88,    89,    -1,    -1,    -1,    -1,    94,    -1,    -1,    97,
      98,    99,   100,   101,   102,     1,    -1,     3,     4,     5,
       6,    -1,   110,     9,    10,    11,    12,    13,    -1,    15,
      16,    17,    18,    19,    -1,    21,    22,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,    -1,
      46,    47,    48,    49,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,    -1,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    -1,    -1,    83,    84,    85,
      86,    -1,    88,    -1,    -1,    -1,    -1,    -1,    94,    -1,
      -1,    97,    98,    99,   100,   101,   102,     1,    -1,     3,
       4,     5,     6,    -1,   110,     9,    10,    11,    12,    13,
      -1,    15,    16,    17,    18,    19,    -1,    21,    22,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,
      44,    -1,    46,    47,    48,    49,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      64,    -1,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    -1,    79,    80,    -1,    -1,    83,
      84,    85,    86,     3,    88,    -1,    -1,     7,     8,    -1,
      94,    -1,    -1,    97,    98,    99,   100,   101,   102,    -1,
      -1,     3,     4,     5,     6,    -1,   110,     9,    10,    11,
      12,    13,    -1,    -1,    -1,    17,    -1,    19,    -1,    21,
      22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    46,    -1,    48,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    77,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    75,    76,    77,    -1,    79,    80,    -1,
      -1,    -1,    84,    85,    86,     3,    88,    -1,    -1,     7,
       8,    -1,    94,    -1,    -1,    97,    98,    99,   100,   101,
     102,    -1,    -1,    -1,     3,     4,     5,     6,    -1,   111,
       9,    10,    11,    12,    13,    -1,    -1,    -1,    17,    -1,
      19,    -1,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    -1,    -1,    46,    47,    48,
      49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    77,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    75,    76,    77,    -1,
      79,    80,    -1,    -1,    -1,    84,    85,    86,    -1,    88,
      -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,    97,    98,
      99,   100,   101,   102,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   110,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    17,    -1,    19,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    76,    77,     3,    79,    80,
      -1,     7,     8,    84,    85,    86,    -1,    88,    -1,    -1,
      -1,    -1,    -1,    94,    -1,    -1,    97,    98,    99,   100,
     101,   102,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    17,    -1,    19,    -1,
      21,    22,    -1,    -1,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,    -1,
      -1,    77,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    76,    77,    78,    79,    80,
      -1,    -1,    -1,    84,    85,    86,    -1,    88,    -1,    -1,
      91,    92,    -1,    94,    -1,    -1,    97,    98,    99,   100,
     101,   102,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    17,    -1,    19,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    76,    77,    78,    79,    80,
      -1,    -1,    -1,    84,    85,    86,    -1,    88,    -1,    -1,
      -1,    92,    -1,    94,    -1,    -1,    97,    98,    99,   100,
     101,   102,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    17,    -1,    19,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    76,    77,    78,    79,    80,
      -1,    -1,    -1,    84,    85,    86,    87,    88,    -1,    -1,
      -1,    -1,    -1,    94,    -1,    -1,    97,    98,    99,   100,
     101,   102,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    17,    -1,    19,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    76,    77,    78,    79,    80,
      -1,    -1,    -1,    84,    85,    86,    87,    88,    -1,    -1,
      -1,    -1,    -1,    94,    -1,    -1,    97,    98,    99,   100,
     101,   102,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    17,    -1,    19,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    76,    77,    78,    79,    80,
      -1,    -1,    -1,    84,    85,    86,    87,    88,    -1,    -1,
      -1,    -1,    -1,    94,    -1,    -1,    97,    98,    99,   100,
     101,   102,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    17,    -1,    19,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    76,    77,    78,    79,    80,
      -1,    -1,    -1,    84,    85,    86,    87,    88,    -1,    -1,
      -1,    -1,    -1,    94,    -1,    -1,    97,    98,    99,   100,
     101,   102,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    17,    -1,    19,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    76,    77,    78,    79,    80,
      -1,    -1,    -1,    84,    85,    86,    -1,    88,    -1,    -1,
      -1,    92,    -1,    94,    -1,    -1,    97,    98,    99,   100,
     101,   102,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    17,    -1,    19,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    76,    77,    78,    79,    80,
      -1,    -1,    -1,    84,    85,    86,    -1,    88,    -1,    -1,
      -1,    92,    -1,    94,    -1,    -1,    97,    98,    99,   100,
     101,   102,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    17,    -1,    19,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    76,    77,    -1,    79,    80,
      -1,    -1,    -1,    84,    85,    86,    -1,    88,    -1,    -1,
      -1,    92,    -1,    94,    -1,    -1,    97,    98,    99,   100,
     101,   102,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    17,    -1,    19,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    76,    77,    78,    79,    80,
      -1,    -1,    -1,    84,    85,    86,    -1,    88,    -1,    -1,
      -1,    -1,    -1,    94,    -1,    -1,    97,    98,    99,   100,
     101,   102,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    17,    -1,    19,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,     3,
       4,     5,     6,    -1,    -1,     9,    10,    11,    12,    13,
      -1,    -1,    -1,    17,    -1,    19,    -1,    21,    22,    -1,
      -1,    -1,    -1,    -1,    75,    76,    77,    -1,    79,    80,
      -1,    -1,    -1,    84,    85,    86,    -1,    88,    -1,    -1,
      -1,    -1,    46,    94,    48,    -1,    97,    98,    99,   100,
     101,   102,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    75,    76,    77,    -1,    79,    80,    -1,    -1,    -1,
      84,    85,    86,    -1,    88,    -1,    -1,    -1,    92,    -1,
      94,    -1,    -1,    97,    98,    99,   100,   101,   102,     3,
       4,     5,     6,    -1,    -1,     9,    10,    11,    12,    13,
      -1,    -1,    -1,    17,    -1,    19,    -1,    21,    22,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    -1,    48,    -1,     3,     4,     5,     6,
      -1,    -1,     9,    10,    11,    12,    13,    -1,    -1,    -1,
      17,    -1,    19,    -1,    21,    22,    -1,    -1,    -1,    -1,
      -1,    75,    76,    77,    -1,    79,    80,    -1,    -1,    -1,
      84,    85,    86,    -1,    88,    -1,    -1,    -1,    92,    46,
      94,    48,    -1,    97,    98,    99,   100,   101,   102,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    75,    76,
      77,    -1,    79,    80,    -1,    -1,    -1,    84,    85,    86,
      -1,    88,    -1,    -1,    -1,    92,    -1,    94,    -1,    -1,
      97,    98,    99,   100,   101,   102,     3,     4,     5,     6,
      -1,    -1,     9,    10,    11,    12,    13,    -1,    -1,    -1,
      17,    -1,    19,    -1,    21,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    48,    -1,     3,     4,     5,     6,    -1,    -1,     9,
      10,    11,    12,    13,    -1,    -1,    -1,    17,    -1,    19,
      -1,    21,    22,    -1,    -1,    -1,    -1,    -1,    75,    76,
      77,    -1,    79,    80,    -1,    -1,    -1,    84,    85,    86,
      -1,    88,    -1,    -1,    -1,    92,    46,    94,    48,    -1,
      97,    98,    99,   100,   101,   102,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    75,    76,    77,    -1,    79,
      80,    -1,    -1,    -1,    84,    85,    86,    -1,    88,    -1,
      -1,    -1,    92,    -1,    94,    -1,    -1,    97,    98,    99,
     100,   101,   102,     3,     4,     5,     6,    -1,    -1,     9,
      10,    11,    12,    13,    -1,    -1,    -1,    17,    -1,    19,
      -1,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    48,    -1,
       3,     4,     5,     6,    -1,    -1,     9,    10,    11,    12,
      13,    -1,    -1,    -1,    17,    -1,    19,    -1,    21,    22,
      -1,    -1,    -1,    -1,    -1,    75,    76,    77,    -1,    79,
      80,    -1,    -1,    -1,    84,    85,    86,    -1,    88,    -1,
      -1,    -1,    92,    46,    94,    48,    -1,    97,    98,    99,
     100,   101,   102,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    75,    76,    77,    -1,    79,    80,    -1,    -1,
      -1,    84,    85,    86,    -1,    88,    -1,    -1,    -1,    -1,
      -1,    94,    -1,    -1,    97,    98,    99,   100,   101,   102,
       3,     4,     5,     6,    -1,    -1,     9,    10,    11,    12,
      13,    -1,    -1,    -1,    17,    -1,    19,    -1,    21,    22,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    -1,    48,    -1,     3,     4,     5,
       6,    -1,    -1,     9,    10,    11,    12,    13,    -1,    -1,
      -1,    17,    -1,    19,    -1,    21,    22,    -1,    -1,    -1,
      -1,    -1,    75,    76,    77,    -1,    79,    80,    -1,    -1,
      -1,    84,    85,    86,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    94,    48,    -1,    97,    98,    99,   100,   101,   102,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    75,
      76,    77,    -1,    79,    80,    -1,    -1,    -1,    84,    85,
      86,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,
      -1,    97,    98,    99,   100,   101,   102,     3,     4,     5,
       6,    -1,    -1,     9,    10,    11,    12,    13,    -1,    -1,
      -1,    17,    -1,    19,    -1,    21,    22,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    -1,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    75,
      76,    77,    -1,    79,    80,    -1,    -1,    -1,    84,    85,
      86,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,
      -1,    97,    98,    99,   100,   101,   102
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     1,     3,     4,     5,     6,     9,    10,    11,    12,
      13,    15,    16,    17,    18,    19,    21,    22,    43,    44,
      46,    47,    48,    49,    64,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    79,    80,    83,
      84,    85,    86,    88,    94,    97,    98,    99,   100,   101,
     102,   110,   113,   114,   116,   117,   118,   120,   121,   122,
     123,   124,   125,   126,   127,   130,   131,   138,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   162,   163,   164,   165,   211,   212,   213,
     214,   217,   220,   221,   222,   224,   225,   226,   227,   228,
     229,   230,   231,   110,    77,    86,   116,   117,   141,    86,
      86,   141,   116,   117,    86,    86,   141,   141,   116,    78,
     115,   116,   157,   158,    88,   165,    88,   217,    86,    86,
      86,    86,   211,    86,   110,   110,   110,   165,   165,    91,
     115,   165,   211,   218,   219,   165,   219,    86,    88,   113,
      92,   115,   139,   140,   164,   164,     4,     5,     6,    80,
      86,    94,   116,   118,   130,   132,     6,     5,     7,     8,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,   113,   114,   116,   165,   171,   172,   176,
     180,   181,   197,   198,   199,    45,    89,   165,   215,   216,
     219,    91,    92,   128,   129,   140,   164,    91,    93,    86,
     217,    86,    86,    20,    21,    22,    86,    94,    96,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    95,
     156,   141,   143,    98,   103,   104,    99,   100,    23,    24,
      25,    26,   105,   106,    27,    28,    97,   107,   108,    29,
      30,   109,    90,   110,   116,     0,   231,   197,    86,   116,
     197,   197,    88,    86,   116,   165,   110,    86,   113,    90,
     110,   165,    88,   164,   165,   165,   165,    67,   165,   223,
     110,    91,   211,    89,    89,   218,    89,    89,   165,   219,
      90,    92,    90,   132,   132,    91,    92,   132,   134,   136,
     137,    86,    86,    29,    91,    93,    87,   171,   116,   181,
     199,    87,    86,    94,    98,   193,   207,   208,   181,   158,
      89,    89,   215,   219,    89,    92,    90,    92,    63,    90,
      92,    91,   211,   116,    87,   140,    87,   140,    87,   158,
     159,   116,    87,   140,   165,   116,   164,   143,   143,   143,
     144,   144,   145,   145,   146,   146,   146,   146,   147,   147,
     148,   149,   150,   151,   152,   165,   164,    86,    87,    87,
     140,    93,    87,    87,    89,   158,    87,   159,   217,    90,
     165,   157,     1,    14,    60,    61,    62,    89,   111,   115,
     116,   160,   161,   167,   169,   172,   173,   174,   175,   181,
     183,   186,    87,    90,    87,    87,    87,    86,   110,   211,
      87,    89,    92,   139,   164,    90,    87,    92,    91,    63,
      90,    92,    90,    92,    87,   116,   132,   133,   134,   135,
      87,   134,   165,   211,   116,   172,    93,   143,    87,   207,
      92,   154,   166,   181,   193,   208,    86,    94,   110,   143,
      89,    92,   128,   164,    92,    92,   164,   217,    87,    90,
      87,   217,    63,    87,    87,    92,    91,    87,   159,    87,
     116,    89,    90,   116,   217,    87,   197,    87,   110,   183,
     116,   119,   166,    89,   161,   181,   116,   119,   173,    86,
     110,   116,   170,   189,   190,   193,   140,   211,   217,   211,
     165,   223,    81,   132,   132,    92,    92,   132,    92,   132,
     137,    95,    87,    90,    90,    87,    91,   116,   181,   182,
      87,    92,   193,    62,    87,   116,   172,   175,   181,   200,
     201,   202,   203,   204,   205,   206,    92,   166,   217,   217,
     154,   217,    87,    89,   217,   116,   217,    90,   170,    88,
     183,    88,   189,    90,   110,    86,    94,   190,    87,    65,
      87,   110,    81,   132,   116,   211,   116,   182,   116,   204,
      87,    90,    86,   116,   193,   207,   209,   210,   181,    92,
     217,   217,   116,   110,    89,    90,   116,   187,   188,   170,
       1,    89,   111,   116,   177,   178,   179,   183,    87,   189,
      87,   111,   168,   194,   195,   196,    92,   166,   211,   110,
     223,    95,   182,    63,   202,   209,   210,    86,    94,    87,
      89,    95,    89,    90,   110,   110,   111,   166,    91,    89,
     178,   179,    89,   110,   184,   185,   189,   182,   166,   183,
      87,    90,    92,    87,   132,    87,    87,   200,    92,   166,
     166,    89,   188,   166,   110,   183,    89,    90,   110,    91,
      86,    94,   189,   191,   192,   193,   182,    63,   196,   211,
      87,    92,   183,   110,   184,   185,   185,   166,    87,   191,
      92,   166,    86,    94,   192,   185,   110,   110,   182,    87,
      92,    87,   194,    92,   166,    91,   182,    87,    92,   166,
     182,   110
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
#line 106 "c.y"
    { ((*yyvalp).expr) = doidnsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); }
    break;

  case 3:

/* Line 936 of glr.c  */
#line 111 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxunquote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 4:

/* Line 936 of glr.c  */
#line 113 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxunquote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 5:

/* Line 936 of glr.c  */
#line 118 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxsplice, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 6:

/* Line 936 of glr.c  */
#line 120 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxsplice, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 9:

/* Line 936 of glr.c  */
#line 130 "c.y"
    { ((*yyvalp).expr) = doidnsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p+1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len-1); }
    break;

  case 10:

/* Line 936 of glr.c  */
#line 135 "c.y"
    { ((*yyvalp).expr) = doidnsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p+1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len-1); }
    break;

  case 12:

/* Line 936 of glr.c  */
#line 141 "c.y"
    { ((*yyvalp).expr) = 0; }
    break;

  case 14:

/* Line 936 of glr.c  */
#line 150 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elambda, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 15:

/* Line 936 of glr.c  */
#line 152 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elambda, nullelist(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 16:

/* Line 936 of glr.c  */
#line 157 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefrec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); }
    break;

  case 17:

/* Line 936 of glr.c  */
#line 159 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefrec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), 0, 0); }
    break;

  case 18:

/* Line 936 of glr.c  */
#line 161 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefrec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); }
    break;

  case 19:

/* Line 936 of glr.c  */
#line 166 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); }
    break;

  case 20:

/* Line 936 of glr.c  */
#line 171 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxquote, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); }
    break;

  case 21:

/* Line 936 of glr.c  */
#line 173 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxquote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 22:

/* Line 936 of glr.c  */
#line 175 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxquasi, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); }
    break;

  case 23:

/* Line 936 of glr.c  */
#line 177 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxquasi, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 24:

/* Line 936 of glr.c  */
#line 182 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); }
    break;

  case 25:

/* Line 936 of glr.c  */
#line 184 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); }
    break;

  case 26:

/* Line 936 of glr.c  */
#line 186 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); }
    break;

  case 27:

/* Line 936 of glr.c  */
#line 191 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Emcall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); }
    break;

  case 28:

/* Line 936 of glr.c  */
#line 193 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Emcall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); }
    break;

  case 29:

/* Line 936 of glr.c  */
#line 198 "c.y"
    {
		Lit lit;
		char *err;
		if(0 != parselit((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.chars).len, &lit, 0, &err))
			parseerror(ctx, err);
		/* FIXME: check type of literal */
		ctx->inp->src.line = (unsigned)lit.v.u;
		((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Enil, 0, 0, 0, 0);
	}
    break;

  case 30:

/* Line 936 of glr.c  */
#line 208 "c.y"
    {
		((*yyvalp).expr) = Zuint(ctx->inp->src.line);
	}
    break;

  case 31:

/* Line 936 of glr.c  */
#line 212 "c.y"
    {
		unsigned long nlen;
		expandstr((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.chars).len, &nlen);
		ctx->inp->src.filename = internfilename((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.chars).p, nlen);
		((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Enil, 0, 0, 0, 0);
	}
    break;

  case 32:

/* Line 936 of glr.c  */
#line 219 "c.y"
    {
		((*yyvalp).expr) = Zstr(ctx->inp->src.filename);
	}
    break;

  case 33:

/* Line 936 of glr.c  */
#line 226 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, Eelist,
				     (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0),
			  0, 0);
	}
    break;

  case 34:

/* Line 936 of glr.c  */
#line 236 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); }
    break;

  case 35:

/* Line 936 of glr.c  */
#line 238 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 36:

/* Line 936 of glr.c  */
#line 243 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxlist, nullelist(), 0, 0, 0); }
    break;

  case 37:

/* Line 936 of glr.c  */
#line 245 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxlist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); }
    break;

  case 38:

/* Line 936 of glr.c  */
#line 247 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxlist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); }
    break;

  case 40:

/* Line 936 of glr.c  */
#line 253 "c.y"
    { ((*yyvalp).expr) = dotickesrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 41:

/* Line 936 of glr.c  */
#line 255 "c.y"
    { ((*yyvalp).expr) = dosym((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); }
    break;

  case 42:

/* Line 936 of glr.c  */
#line 257 "c.y"
    { ((*yyvalp).expr) = doconst(ctx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); }
    break;

  case 43:

/* Line 936 of glr.c  */
#line 259 "c.y"
    { ((*yyvalp).expr) = dostr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); }
    break;

  case 44:

/* Line 936 of glr.c  */
#line 261 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); }
    break;

  case 45:

/* Line 936 of glr.c  */
#line 263 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epair, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 46:

/* Line 936 of glr.c  */
#line 265 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, nullelist(), 0, 0, 0); }
    break;

  case 47:

/* Line 936 of glr.c  */
#line 267 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); }
    break;

  case 48:

/* Line 936 of glr.c  */
#line 269 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); }
    break;

  case 49:

/* Line 936 of glr.c  */
#line 271 "c.y"
    { Expr *ell = newexprsrc(&ctx->inp->src, Eellipsis, 0, 0, 0, 0);
          ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist,
                          invert(newexprsrc(&ctx->inp->src, Eelist,
                                            ell, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), 0, 0)), 0, 0, 0); }
    break;

  case 50:

/* Line 936 of glr.c  */
#line 276 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, nullelist(), 0, 0, 0); }
    break;

  case 51:

/* Line 936 of glr.c  */
#line 278 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); }
    break;

  case 52:

/* Line 936 of glr.c  */
#line 280 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); }
    break;

  case 61:

/* Line 936 of glr.c  */
#line 293 "c.y"
    { ((*yyvalp).expr) = doconst(ctx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); }
    break;

  case 62:

/* Line 936 of glr.c  */
#line 295 "c.y"
    { ((*yyvalp).expr) = dosym((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); }
    break;

  case 63:

/* Line 936 of glr.c  */
#line 297 "c.y"
    { ((*yyvalp).expr) = dostr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); }
    break;

  case 64:

/* Line 936 of glr.c  */
#line 299 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); }
    break;

  case 65:

/* Line 936 of glr.c  */
#line 301 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epair, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 66:

/* Line 936 of glr.c  */
#line 303 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, nullelist(), 0, 0, 0); }
    break;

  case 67:

/* Line 936 of glr.c  */
#line 305 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); }
    break;

  case 68:

/* Line 936 of glr.c  */
#line 307 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); }
    break;

  case 69:

/* Line 936 of glr.c  */
#line 309 "c.y"
    { Expr *ell = newexprsrc(&ctx->inp->src, Eellipsis, 0, 0, 0, 0);
          ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist,
                          invert(newexprsrc(&ctx->inp->src, Eelist,
                                            ell, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), 0, 0)), 0, 0, 0); }
    break;

  case 70:

/* Line 936 of glr.c  */
#line 314 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, nullelist(), 0, 0, 0); }
    break;

  case 71:

/* Line 936 of glr.c  */
#line 316 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); }
    break;

  case 72:

/* Line 936 of glr.c  */
#line 318 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); }
    break;

  case 73:

/* Line 936 of glr.c  */
#line 320 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); }
    break;

  case 74:

/* Line 936 of glr.c  */
#line 322 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); }
    break;

  case 75:

/* Line 936 of glr.c  */
#line 324 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); }
    break;

  case 76:

/* Line 936 of glr.c  */
#line 326 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); }
    break;

  case 77:

/* Line 936 of glr.c  */
#line 328 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); }
    break;

  case 81:

/* Line 936 of glr.c  */
#line 339 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); }
    break;

  case 82:

/* Line 936 of glr.c  */
#line 341 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 83:

/* Line 936 of glr.c  */
#line 346 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
                          newbinopsrc(&ctx->inp->src, Eeq, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)),
                          nullelist(),
                          0, 0); }
    break;

  case 84:

/* Line 936 of glr.c  */
#line 351 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
                          newbinopsrc(&ctx->inp->src, Eeq, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr)),
                          (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr),
                          0, 0); }
    break;

  case 85:

/* Line 936 of glr.c  */
#line 359 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); }
    break;

  case 86:

/* Line 936 of glr.c  */
#line 361 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 87:

/* Line 936 of glr.c  */
#line 366 "c.y"
    { /* key may not actually bind variables, but this will get
             caught during compilation. */
          ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, Eelist,
				     (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0),
			  0, 0);
	}
    break;

  case 89:

/* Line 936 of glr.c  */
#line 379 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 90:

/* Line 936 of glr.c  */
#line 381 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); }
    break;

  case 91:

/* Line 936 of glr.c  */
#line 383 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); }
    break;

  case 92:

/* Line 936 of glr.c  */
#line 385 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edot, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 93:

/* Line 936 of glr.c  */
#line 387 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earrow, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 94:

/* Line 936 of glr.c  */
#line 389 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epostinc, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 95:

/* Line 936 of glr.c  */
#line 391 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epostdec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 96:

/* Line 936 of glr.c  */
#line 393 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Econtainer, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.expr), 0); }
    break;

  case 99:

/* Line 936 of glr.c  */
#line 403 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); }
    break;

  case 100:

/* Line 936 of glr.c  */
#line 405 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 103:

/* Line 936 of glr.c  */
#line 412 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epreinc, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 104:

/* Line 936 of glr.c  */
#line 414 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epredec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 105:

/* Line 936 of glr.c  */
#line 416 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 106:

/* Line 936 of glr.c  */
#line 418 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Esizeofe, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 107:

/* Line 936 of glr.c  */
#line 420 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Esizeoft, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 108:

/* Line 936 of glr.c  */
#line 422 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypeofe, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 109:

/* Line 936 of glr.c  */
#line 424 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypeoft, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 110:

/* Line 936 of glr.c  */
#line 426 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Emkctype, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 111:

/* Line 936 of glr.c  */
#line 431 "c.y"
    { ((*yyvalp).kind) = Eref; }
    break;

  case 112:

/* Line 936 of glr.c  */
#line 433 "c.y"
    { ((*yyvalp).kind) = Ederef; }
    break;

  case 113:

/* Line 936 of glr.c  */
#line 435 "c.y"
    { ((*yyvalp).kind) = Euplus; }
    break;

  case 114:

/* Line 936 of glr.c  */
#line 437 "c.y"
    { ((*yyvalp).kind) = Euminus; }
    break;

  case 115:

/* Line 936 of glr.c  */
#line 439 "c.y"
    { ((*yyvalp).kind) = Eutwiddle; }
    break;

  case 116:

/* Line 936 of glr.c  */
#line 441 "c.y"
    { ((*yyvalp).kind) = Eunot; }
    break;

  case 118:

/* Line 936 of glr.c  */
#line 447 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecast, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 119:

/* Line 936 of glr.c  */
#line 449 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Excast, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr)); }
    break;

  case 121:

/* Line 936 of glr.c  */
#line 455 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Emul, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 122:

/* Line 936 of glr.c  */
#line 457 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ediv, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 123:

/* Line 936 of glr.c  */
#line 459 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Emod, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 125:

/* Line 936 of glr.c  */
#line 465 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eadd, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 126:

/* Line 936 of glr.c  */
#line 467 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Esub, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 128:

/* Line 936 of glr.c  */
#line 473 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eshl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 129:

/* Line 936 of glr.c  */
#line 475 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eshr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 131:

/* Line 936 of glr.c  */
#line 481 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Elt, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 132:

/* Line 936 of glr.c  */
#line 483 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Egt, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 133:

/* Line 936 of glr.c  */
#line 485 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ele, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 134:

/* Line 936 of glr.c  */
#line 487 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ege, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 136:

/* Line 936 of glr.c  */
#line 493 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eeq, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 137:

/* Line 936 of glr.c  */
#line 495 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eneq, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 139:

/* Line 936 of glr.c  */
#line 501 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eband, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 141:

/* Line 936 of glr.c  */
#line 507 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ebxor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 143:

/* Line 936 of glr.c  */
#line 513 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ebor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 145:

/* Line 936 of glr.c  */
#line 519 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eland, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 147:

/* Line 936 of glr.c  */
#line 525 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 149:

/* Line 936 of glr.c  */
#line 531 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Econd, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0); }
    break;

  case 151:

/* Line 936 of glr.c  */
#line 537 "c.y"
    { if((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.kind) == Eg)
	  	((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0);
	  else
	  	((*yyvalp).expr) = newgopsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr));
	}
    break;

  case 152:

/* Line 936 of glr.c  */
#line 546 "c.y"
    { ((*yyvalp).kind) = Eg; }
    break;

  case 153:

/* Line 936 of glr.c  */
#line 548 "c.y"
    { ((*yyvalp).kind) = Egadd; }
    break;

  case 154:

/* Line 936 of glr.c  */
#line 550 "c.y"
    { ((*yyvalp).kind) = Egband; }
    break;

  case 155:

/* Line 936 of glr.c  */
#line 552 "c.y"
    { ((*yyvalp).kind) = Egbor; }
    break;

  case 156:

/* Line 936 of glr.c  */
#line 554 "c.y"
    { ((*yyvalp).kind) = Egbxor; }
    break;

  case 157:

/* Line 936 of glr.c  */
#line 556 "c.y"
    { ((*yyvalp).kind) = Egdiv; }
    break;

  case 158:

/* Line 936 of glr.c  */
#line 558 "c.y"
    { ((*yyvalp).kind) = Egmod; }
    break;

  case 159:

/* Line 936 of glr.c  */
#line 560 "c.y"
    { ((*yyvalp).kind) = Egmul; }
    break;

  case 160:

/* Line 936 of glr.c  */
#line 562 "c.y"
    { ((*yyvalp).kind) = Egshl; }
    break;

  case 161:

/* Line 936 of glr.c  */
#line 564 "c.y"
    { ((*yyvalp).kind) = Egshr; }
    break;

  case 162:

/* Line 936 of glr.c  */
#line 566 "c.y"
    { ((*yyvalp).kind) = Egsub; }
    break;

  case 165:

/* Line 936 of glr.c  */
#line 577 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); }
    break;

  case 166:

/* Line 936 of glr.c  */
#line 579 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 167:

/* Line 936 of glr.c  */
#line 584 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr); }
    break;

  case 168:

/* Line 936 of glr.c  */
#line 586 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
			  newexprsrc(&ctx->inp->src, Eellipsis, 0, 0, 0, 0),
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0);
	}
    break;

  case 169:

/* Line 936 of glr.c  */
#line 594 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); }
    break;

  case 170:

/* Line 936 of glr.c  */
#line 596 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 174:

/* Line 936 of glr.c  */
#line 604 "c.y"
    { ((*yyvalp).expr) = 0; }
    break;

  case 176:

/* Line 936 of glr.c  */
#line 610 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Enames, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); }
    break;

  case 177:

/* Line 936 of glr.c  */
#line 612 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Enames, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); }
    break;

  case 178:

/* Line 936 of glr.c  */
#line 617 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elapply, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.expr)), 0, 0); }
    break;

  case 179:

/* Line 936 of glr.c  */
#line 619 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elapply, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); }
    break;

  case 183:

/* Line 936 of glr.c  */
#line 630 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecomma, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 185:

/* Line 936 of glr.c  */
#line 639 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); }
    break;

  case 186:

/* Line 936 of glr.c  */
#line 644 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); }
    break;

  case 187:

/* Line 936 of glr.c  */
#line 646 "c.y"
    { ((*yyvalp).expr) = 0; }
    break;

  case 188:

/* Line 936 of glr.c  */
#line 651 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecls, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); }
    break;

  case 189:

/* Line 936 of glr.c  */
#line 653 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecls, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), 0); }
    break;

  case 190:

/* Line 936 of glr.c  */
#line 655 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecls, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0); }
    break;

  case 191:

/* Line 936 of glr.c  */
#line 660 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); }
    break;

  case 192:

/* Line 936 of glr.c  */
#line 662 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 193:

/* Line 936 of glr.c  */
#line 667 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Evoid, 0, 0, 0, 0); }
    break;

  case 194:

/* Line 936 of glr.c  */
#line 669 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebool, 0, 0, 0, 0); }
    break;

  case 195:

/* Line 936 of glr.c  */
#line 671 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Echar, 0, 0, 0, 0); }
    break;

  case 196:

/* Line 936 of glr.c  */
#line 673 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eshort, 0, 0, 0, 0); }
    break;

  case 197:

/* Line 936 of glr.c  */
#line 675 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eint, 0, 0, 0, 0); }
    break;

  case 198:

/* Line 936 of glr.c  */
#line 677 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elong, 0, 0, 0, 0); }
    break;

  case 199:

/* Line 936 of glr.c  */
#line 679 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efloat, 0, 0, 0, 0); }
    break;

  case 200:

/* Line 936 of glr.c  */
#line 681 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edouble, 0, 0, 0, 0); }
    break;

  case 201:

/* Line 936 of glr.c  */
#line 683 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Esigned, 0, 0, 0, 0); }
    break;

  case 202:

/* Line 936 of glr.c  */
#line 685 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eunsigned, 0, 0, 0, 0); }
    break;

  case 203:

/* Line 936 of glr.c  */
#line 689 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); }
    break;

  case 204:

/* Line 936 of glr.c  */
#line 691 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 205:

/* Line 936 of glr.c  */
#line 696 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 206:

/* Line 936 of glr.c  */
#line 698 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 209:

/* Line 936 of glr.c  */
#line 705 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.expr), 0); }
    break;

  case 210:

/* Line 936 of glr.c  */
#line 707 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); }
    break;

  case 211:

/* Line 936 of glr.c  */
#line 709 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), Znull(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr), 0); }
    break;

  case 212:

/* Line 936 of glr.c  */
#line 711 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); }
    break;

  case 213:

/* Line 936 of glr.c  */
#line 713 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 214:

/* Line 936 of glr.c  */
#line 718 "c.y"
    { ((*yyvalp).kind) = Estruct; }
    break;

  case 215:

/* Line 936 of glr.c  */
#line 720 "c.y"
    { ((*yyvalp).kind) = Eunion; }
    break;

  case 216:

/* Line 936 of glr.c  */
#line 725 "c.y"
    { ((*yyvalp).kind) = Estruct; }
    break;

  case 217:

/* Line 936 of glr.c  */
#line 727 "c.y"
    { ((*yyvalp).kind) = Eunion; }
    break;

  case 218:

/* Line 936 of glr.c  */
#line 729 "c.y"
    { ((*yyvalp).kind) = Eenum; }
    break;

  case 219:

/* Line 936 of glr.c  */
#line 734 "c.y"
    {
		/* labels yield null struct_declarations */
		if((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr))
			((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
					(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0);
		else
			((*yyvalp).expr) = Znull();
	}
    break;

  case 220:

/* Line 936 of glr.c  */
#line 743 "c.y"
    {
		/* labels yield null struct_declarations */
	 	if((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr))
			((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0);
		else
			((*yyvalp).expr) = Znull();
	}
    break;

  case 221:

/* Line 936 of glr.c  */
#line 754 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efields, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), 0); }
    break;

  case 222:

/* Line 936 of glr.c  */
#line 756 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efields, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), Znull(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), 0); }
    break;

  case 223:

/* Line 936 of glr.c  */
#line 758 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efields, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); }
    break;

  case 224:

/* Line 936 of glr.c  */
#line 760 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efields, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0); }
    break;

  case 225:

/* Line 936 of glr.c  */
#line 762 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebitfield, (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.expr)); }
    break;

  case 226:

/* Line 936 of glr.c  */
#line 764 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebitfield, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)); }
    break;

  case 227:

/* Line 936 of glr.c  */
#line 767 "c.y"
    { ((*yyvalp).expr) = 0; }
    break;

  case 228:

/* Line 936 of glr.c  */
#line 769 "c.y"
    { ((*yyvalp).expr) = 0; }
    break;

  case 229:

/* Line 936 of glr.c  */
#line 774 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); }
    break;

  case 237:

/* Line 936 of glr.c  */
#line 796 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); }
    break;

  case 238:

/* Line 936 of glr.c  */
#line 798 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); }
    break;

  case 239:

/* Line 936 of glr.c  */
#line 803 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); }
    break;

  case 240:

/* Line 936 of glr.c  */
#line 805 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 242:

/* Line 936 of glr.c  */
#line 814 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); }
    break;

  case 243:

/* Line 936 of glr.c  */
#line 816 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), 0, 0); }
    break;

  case 244:

/* Line 936 of glr.c  */
#line 818 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); }
    break;

  case 245:

/* Line 936 of glr.c  */
#line 820 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); }
    break;

  case 246:

/* Line 936 of glr.c  */
#line 822 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 247:

/* Line 936 of glr.c  */
#line 827 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); }
    break;

  case 248:

/* Line 936 of glr.c  */
#line 829 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 249:

/* Line 936 of glr.c  */
#line 836 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenumel, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 250:

/* Line 936 of glr.c  */
#line 838 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenumel, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 251:

/* Line 936 of glr.c  */
#line 843 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); }
    break;

  case 253:

/* Line 936 of glr.c  */
#line 849 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr); }
    break;

  case 254:

/* Line 936 of glr.c  */
#line 851 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); }
    break;

  case 255:

/* Line 936 of glr.c  */
#line 853 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 256:

/* Line 936 of glr.c  */
#line 855 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 257:

/* Line 936 of glr.c  */
#line 857 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 258:

/* Line 936 of glr.c  */
#line 859 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); }
    break;

  case 261:

/* Line 936 of glr.c  */
#line 866 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); }
    break;

  case 262:

/* Line 936 of glr.c  */
#line 871 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); }
    break;

  case 263:

/* Line 936 of glr.c  */
#line 873 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, 0, 0, 0); }
    break;

  case 264:

/* Line 936 of glr.c  */
#line 875 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 265:

/* Line 936 of glr.c  */
#line 877 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 266:

/* Line 936 of glr.c  */
#line 879 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 267:

/* Line 936 of glr.c  */
#line 881 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, 0, Znull(), 0, 0); }
    break;

  case 268:

/* Line 936 of glr.c  */
#line 883 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); }
    break;

  case 269:

/* Line 936 of glr.c  */
#line 885 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 270:

/* Line 936 of glr.c  */
#line 890 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, 0, 0, 0, 0); }
    break;

  case 271:

/* Line 936 of glr.c  */
#line 892 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, 0, 0, 0, 0); }
    break;

  case 272:

/* Line 936 of glr.c  */
#line 894 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 273:

/* Line 936 of glr.c  */
#line 896 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 274:

/* Line 936 of glr.c  */
#line 901 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr)); }
    break;

  case 275:

/* Line 936 of glr.c  */
#line 903 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 276:

/* Line 936 of glr.c  */
#line 908 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); }
    break;

  case 277:

/* Line 936 of glr.c  */
#line 910 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 278:

/* Line 936 of glr.c  */
#line 915 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0); }
    break;

  case 279:

/* Line 936 of glr.c  */
#line 917 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0); }
    break;

  case 280:

/* Line 936 of glr.c  */
#line 919 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0); }
    break;

  case 281:

/* Line 936 of glr.c  */
#line 924 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypename, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 282:

/* Line 936 of glr.c  */
#line 926 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypename, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 284:

/* Line 936 of glr.c  */
#line 932 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); }
    break;

  case 285:

/* Line 936 of glr.c  */
#line 934 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); }
    break;

  case 286:

/* Line 936 of glr.c  */
#line 939 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 287:

/* Line 936 of glr.c  */
#line 941 "c.y"
    { ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0)); }
    break;

  case 288:

/* Line 936 of glr.c  */
#line 944 "c.y"
    { ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0)); }
    break;

  case 289:

/* Line 936 of glr.c  */
#line 947 "c.y"
    { ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, 0,
			  newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0)); }
    break;

  case 290:

/* Line 936 of glr.c  */
#line 950 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 291:

/* Line 936 of glr.c  */
#line 952 "c.y"
    { ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0)); }
    break;

  case 293:

/* Line 936 of glr.c  */
#line 959 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr)); }
    break;

  case 294:

/* Line 936 of glr.c  */
#line 961 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 295:

/* Line 936 of glr.c  */
#line 966 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); }
    break;

  case 296:

/* Line 936 of glr.c  */
#line 968 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 297:

/* Line 936 of glr.c  */
#line 973 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 298:

/* Line 936 of glr.c  */
#line 975 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 299:

/* Line 936 of glr.c  */
#line 977 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 301:

/* Line 936 of glr.c  */
#line 983 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); }
    break;

  case 302:

/* Line 936 of glr.c  */
#line 985 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); }
    break;

  case 303:

/* Line 936 of glr.c  */
#line 990 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 304:

/* Line 936 of glr.c  */
#line 992 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 307:

/* Line 936 of glr.c  */
#line 999 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 308:

/* Line 936 of glr.c  */
#line 1004 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 311:

/* Line 936 of glr.c  */
#line 1011 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); }
    break;

  case 312:

/* Line 936 of glr.c  */
#line 1016 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); }
    break;

  case 313:

/* Line 936 of glr.c  */
#line 1018 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, 0, 0, 0); }
    break;

  case 314:

/* Line 936 of glr.c  */
#line 1020 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 315:

/* Line 936 of glr.c  */
#line 1022 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 316:

/* Line 936 of glr.c  */
#line 1024 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 317:

/* Line 936 of glr.c  */
#line 1026 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, 0, nullelist(), 0, 0); }
    break;

  case 318:

/* Line 936 of glr.c  */
#line 1028 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); }
    break;

  case 319:

/* Line 936 of glr.c  */
#line 1030 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 320:

/* Line 936 of glr.c  */
#line 1035 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); }
    break;

  case 322:

/* Line 936 of glr.c  */
#line 1041 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr); }
    break;

  case 323:

/* Line 936 of glr.c  */
#line 1043 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); }
    break;

  case 324:

/* Line 936 of glr.c  */
#line 1045 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 325:

/* Line 936 of glr.c  */
#line 1047 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 326:

/* Line 936 of glr.c  */
#line 1049 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 327:

/* Line 936 of glr.c  */
#line 1051 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); }
    break;

  case 339:

/* Line 936 of glr.c  */
#line 1070 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Emcall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0); }
    break;

  case 340:

/* Line 936 of glr.c  */
#line 1072 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Emcall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), nullelist(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), 0); }
    break;

  case 341:

/* Line 936 of glr.c  */
#line 1074 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Emcall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0); }
    break;

  case 342:

/* Line 936 of glr.c  */
#line 1079 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxunquote, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); }
    break;

  case 343:

/* Line 936 of glr.c  */
#line 1084 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eglobal, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); }
    break;

  case 344:

/* Line 936 of glr.c  */
#line 1089 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 345:

/* Line 936 of glr.c  */
#line 1094 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); }
    break;

  case 346:

/* Line 936 of glr.c  */
#line 1096 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 347:

/* Line 936 of glr.c  */
#line 1101 "c.y"
    {
		((*yyvalp).expr) = newexprsrc(&ctx->inp->src,
				Escope,
				newexprsrc(&ctx->inp->src,
					   Eblock,
					   nullelist(),
					   nullelist(), 0, 0),
				0, 0, 0);
	}
    break;

  case 348:

/* Line 936 of glr.c  */
#line 1111 "c.y"
    {
		((*yyvalp).expr) = newexprsrc(&ctx->inp->src,
				Escope,
				newexprsrc(&ctx->inp->src,
					   Eblock,
					   flatten((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)),
					   nullelist(), 0, 0),
				0, 0, 0);
	}
    break;

  case 349:

/* Line 936 of glr.c  */
#line 1121 "c.y"
    {
		/* use src of first statement */
		Expr *sl;
		sl = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr));
		((*yyvalp).expr) = putsrc(newexpr(Escope,
				    newexpr(Eblock,
					    nullelist(),
					    sl, 0, 0),
				    0, 0, 0),
			    sl->src);
	}
    break;

  case 350:

/* Line 936 of glr.c  */
#line 1133 "c.y"
    {
		/* use src of first statement */
		Expr *sl;
		sl = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr));
		((*yyvalp).expr) = putsrc(newexpr(Escope,
				    newexpr(Eblock,
					    flatten((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)),
					    sl, 0, 0),
				    0, 0, 0),
			    sl->src);
	}
    break;

  case 353:

/* Line 936 of glr.c  */
#line 1153 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); }
    break;

  case 354:

/* Line 936 of glr.c  */
#line 1155 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 355:

/* Line 936 of glr.c  */
#line 1160 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Enil, 0, 0, 0, 0); }
    break;

  case 356:

/* Line 936 of glr.c  */
#line 1162 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); }
    break;

  case 357:

/* Line 936 of glr.c  */
#line 1164 "c.y"
    { ((*yyvalp).expr) = 0; }
    break;

  case 358:

/* Line 936 of glr.c  */
#line 1169 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eif, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 359:

/* Line 936 of glr.c  */
#line 1171 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eif, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (7))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (7))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (7))].yystate.yysemantics.yysval.expr), 0); }
    break;

  case 360:

/* Line 936 of glr.c  */
#line 1174 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eswitch, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 361:

/* Line 936 of glr.c  */
#line 1179 "c.y"
    { ((*yyvalp).expr) = Zlabelsrc((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr)->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 362:

/* Line 936 of glr.c  */
#line 1181 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), NULL, NULL); }
    break;

  case 363:

/* Line 936 of glr.c  */
#line 1183 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ematch, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), NULL, NULL); }
    break;

  case 364:

/* Line 936 of glr.c  */
#line 1185 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ematch, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (6))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr), NULL); }
    break;

  case 365:

/* Line 936 of glr.c  */
#line 1187 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefault, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), NULL, NULL, NULL); }
    break;

  case 367:

/* Line 936 of glr.c  */
#line 1193 "c.y"
    { ((*yyvalp).expr) = 0; }
    break;

  case 368:

/* Line 936 of glr.c  */
#line 1198 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ewhile, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 369:

/* Line 936 of glr.c  */
#line 1200 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edo, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (7))].yystate.yysemantics.yysval.expr), 0, 0); }
    break;

  case 370:

/* Line 936 of glr.c  */
#line 1202 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (9))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (9))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (9))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((9) - (9))].yystate.yysemantics.yysval.expr)); }
    break;

  case 371:

/* Line 936 of glr.c  */
#line 1207 "c.y"
    { ((*yyvalp).expr) = Zgotosrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)); }
    break;

  case 372:

/* Line 936 of glr.c  */
#line 1209 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Econtinue, 0, 0, 0, 0); }
    break;

  case 373:

/* Line 936 of glr.c  */
#line 1211 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebreak, 0, 0, 0, 0); }
    break;

  case 374:

/* Line 936 of glr.c  */
#line 1213 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eret, 0, 0, 0, 0); }
    break;

  case 375:

/* Line 936 of glr.c  */
#line 1215 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eret, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); }
    break;

  case 376:

/* Line 936 of glr.c  */
#line 1220 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefstx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (7))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (7))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (7))].yystate.yysemantics.yysval.expr)); }
    break;

  case 377:

/* Line 936 of glr.c  */
#line 1222 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefstx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (6))].yystate.yysemantics.yysval.expr)); }
    break;

  case 378:

/* Line 936 of glr.c  */
#line 1224 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefstx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), nullelist(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (6))].yystate.yysemantics.yysval.expr)); }
    break;

  case 379:

/* Line 936 of glr.c  */
#line 1226 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefstx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), nullelist(), 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr)); }
    break;

  case 380:

/* Line 936 of glr.c  */
#line 1228 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefstx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr)); }
    break;

  case 381:

/* Line 936 of glr.c  */
#line 1233 "c.y"
    { ((*yyvalp).kind) = Edefine; }
    break;

  case 382:

/* Line 936 of glr.c  */
#line 1235 "c.y"
    { ((*yyvalp).kind) = Edefloc; }
    break;

  case 383:

/* Line 936 of glr.c  */
#line 1240 "c.y"
    { ((*yyvalp).expr) = putsrc(newexpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (6))].yystate.yysemantics.yysval.expr), 0), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr)->src); }
    break;

  case 384:

/* Line 936 of glr.c  */
#line 1242 "c.y"
    { ((*yyvalp).expr) = putsrc(newexpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), nullelist(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr)->src); }
    break;

  case 389:

/* Line 936 of glr.c  */
#line 1256 "c.y"
    { dotop(ctx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr)); }
    break;



/* Line 936 of glr.c  */
#line 5054 "c.tab.c"
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

#define yypact_value_is_default(yystate) \
  ((yystate) == (-479))

/** True iff LR state STATE has only a default reduction (regardless
 *  of token).  */
static inline yybool
yyisDefaultedState (yyStateNum yystate)
{
  return yypact_value_is_default (yypact[yystate]);
}

/** The default reduction for STATE, assuming it has one.  */
static inline yyRuleNum
yydefaultAction (yyStateNum yystate)
{
  return yydefact[yystate];
}

#define yytable_value_is_error(yytable_value) \
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
  if (yypact_value_is_default (yypact[yystate])
      || yyindex < 0 || YYLAST < yyindex || yycheck[yyindex] != yytoken)
    {
      *yyaction = -yydefact[yystate];
      *yyconflicts = yyconfl;
    }
  else if (! yytable_value_is_error (yytable[yyindex]))
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
  if (yystackp->yyerrState != 0)
    return;
#if ! YYERROR_VERBOSE
  yyerror (ctx, YY_("syntax error"));
#else
  yySymbol yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);
  size_t yysize0 = yytnamerr (NULL, yytokenName (yytoken));
  size_t yysize = yysize0;
  size_t yysize1;
  yybool yysize_overflow = yyfalse;
  char* yymsg = NULL;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected").  */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[yystackp->yytops.yystates[0]->yylrState];
      yyarg[yycount++] = yytokenName (yytoken);
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for this
             state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;
          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytokenName (yyx);
                yysize1 = yysize + yytnamerr (NULL, yytokenName (yyx));
                yysize_overflow |= yysize1 < yysize;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
#define YYCASE_(N, S)                   \
      case N:                           \
        yyformat = S;                   \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  yysize1 = yysize + strlen (yyformat);
  yysize_overflow |= yysize1 < yysize;
  yysize = yysize1;

  if (!yysize_overflow)
    yymsg = (char *) YYMALLOC (yysize);

  if (yymsg)
    {
      char *yyp = yymsg;
      int yyi = 0;
      while ((*yyp = *yyformat))
        {
          if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
            {
              yyp += yytnamerr (yyp, yyarg[yyi++]);
              yyformat += 2;
            }
          else
            {
              yyp++;
              yyformat++;
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
#endif /* YYERROR_VERBOSE */
  yynerrs += 1;
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
	if (yypact_value_is_default (yyj))
	  return;
	yyj += yytoken;
	if (yyj < 0 || YYLAST < yyj || yycheck[yyj] != yytoken)
	  {
	    if (yydefact[yystackp->yytops.yystates[0]->yylrState] != 0)
	      return;
	  }
	else if (! yytable_value_is_error (yytable[yyj]))
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
      if (! yypact_value_is_default (yyj))
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



/* Line 2659 of glr.c  */
#line 1259 "c.y"

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
		sete1(e, copyexpr(e->e1));
		sete2(e, copyexpr(e->e2));
		return;
	}
	duptickid(e->e1);
	duptickid(e->e2);
	duptickid(e->e3);
	duptickid(e->e4);
}

/* we are here for an ambiguous expression of the form:

        (<xid>)+<expr>
        (<xid>)-<expr>
        (<xid>)*<expr>
        (<xid>)&<expr>
	(<xid>)(<expr>, ...)

   <expr> is an expression.
   <xid> is a identifier of one of two forms:

	<id>
	<id>`<id>

   <id> is an identifier.

   the underlying ambiguity is the same for all five
   forms: does the <xid> denote a type name, in which
   case the form is a cast operation, or a value, in
   which case the form is some other operation,
   superfluously parenthesized.

   the first four forms involve the four operators that
   serve both unary and binary roles.  either the form
   is intended to be a cast of the result of unary
   usage, or a binary usage with superfluous parens
   around the left operand.

   the fifth form is either a cast to the result of
   a parenthesized expression, or a call expression
   with superfluous parens around the function value.

   our policy for these cases is harsh but simple: we
   always assume the cast operation was the intended
   one.  if the user actually wanted superfluous
   parens, too bad.

   in the past we supported more a flexible
   interpretation for a limited set of forms.  for
   example, for an expression of the form:

	(dom`T)*p

   we emitted code to dynamically lookup T as a type in
   dom.  if there was type definition, we proceeded
   with a cast to a pointer dereference.  if not, we
   proceeded under the assumption that dom`T is a
   variable reference, and multiplied the result by p.

   there was no flexibility on the fifth form: we
   always assume the cast was intended.  that is
   because dom`T could not evaluate to a function value
   (but we forsee allowing this in the future).

   in the past, expressions of the form

	(T)*p

   were interpreted as multiplication.  to treat T as a
   type name, it had to be prefixed by a tick:


	(`T)*p

   this violated commonly used C syntax. the current
   policy was in put place as we were addressing this
   syntactic problem.

   the gist of this policy is that the user can always
   remove superfluous parens to achieve the desire
   binary (or call) operation.  while such parens may
   result from expansion of defensively written textual
   macros, macros based on syntactic abstraction, like
   those, do not have this problem.

   note that there is no ambiguity for cast expressions
   of any other type name form.  each of these forms is
   unambiguous:

	   (struct dom`T)*p
	   (struct T)*p
	   (dom`T*)*p
	   (T*)*p
	   (int)*p

*/

static unsigned
castcount(Expr *e)
{
	Expr *p;
	unsigned m;
	if(e == 0)
		return 0;
	switch(e->kind){
	case Ecast:
		return 1+castcount(e->e2);
	case Eelist:
		p = e;
		m = 0;
		while(p->kind == Eelist){
			m += castcount(p->e1);
			p = p->e2;
		}
	default:
		return (castcount(e->e1)
			+castcount(e->e2)
			+castcount(e->e3)
			+castcount(e->e4));
	}
}

static Expr*
castmerge(YYSTYPE ye1, YYSTYPE ye2)
{
#if 0
	Expr *cast, *other;
	if(ye1.expr->kind == Ecast){
		cast = ye1.expr;
		other = ye2.expr;
	}else if(ye2.expr->kind == Ecast){
		cast = ye2.expr;
		other = ye1.expr;
	}else{
		printf("ye1:\n");
		printexpr(ye1.expr);
		printf("\nye2:\n");
		printexpr(ye2.expr);
		yyerror(0, "unresolved ambiguity 0");
	}

	/* sanity check */
	if(cast->e1->kind != Etypename)
		yyerror(0, "unresolved ambiguity 1");
	/* cast->e2 could be any expression */
	return cast;
#else
	if(castcount(ye1.expr) > castcount(ye2.expr))
		return ye1.expr;
	else
		return ye2.expr;
#endif
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
		yyerror(0, "unresolved ambiguity 2");

	if(e1->kind == Esizeofe || e1->kind == Etypeofe)
		duptickid(e1->e1);
	else
		duptickid(e2->e1);
	return putsrc(newexpr(Eambig, e1, e2, NULL, NULL), e1->src);
}

