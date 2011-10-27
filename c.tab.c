
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
#line 90 "c.y"

	static void yyerror(U *ctx, const char *s);
	static Expr* castmerge(YYSTYPE e1, YYSTYPE e2);
	static Expr* ofmerge(YYSTYPE e1, YYSTYPE e2);
	static Expr* tnmerge(YYSTYPE e1, YYSTYPE e2);


/* Line 243 of glr.c  */
#line 113 "c.tab.c"

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
#define YYFINAL  240
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3907

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  110
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  115
/* YYNRULES -- Number of rules.  */
#define YYNRULES  373
/* YYNRULES -- Number of states.  */
#define YYNSTATES  686
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule.  */
#define YYMAXRHS 9
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule.  */
#define YYMAXLEFT 0

/* YYTRANSLATE(X) -- Bison symbol number corresponding to X.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   338

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   100,     2,     2,     2,   102,    95,     2,
      84,    85,    96,    97,    88,    98,    94,   101,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    89,   108,
     103,    93,   104,   107,   109,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    91,     2,    92,   105,     2,    90,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    86,   106,    87,    99,     2,     2,     2,
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
      75,    76,    77,    78,    79,    80,    81,    82,    83
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     5,     8,    13,    15,    17,    19,    21,
      23,    24,    26,    32,    37,    43,    50,    55,    58,    62,
      66,    70,    74,    79,    84,    88,    90,    92,    94,    98,
     102,   106,   111,   115,   117,   121,   123,   127,   129,   131,
     133,   137,   143,   146,   150,   155,   160,   164,   168,   173,
     175,   177,   179,   181,   183,   185,   187,   189,   193,   199,
     202,   206,   211,   216,   220,   224,   229,   234,   238,   243,
     248,   252,   254,   256,   258,   260,   264,   268,   274,   276,
     280,   284,   286,   291,   295,   300,   304,   308,   311,   314,
     323,   325,   329,   331,   333,   336,   339,   342,   345,   350,
     353,   358,   363,   365,   367,   369,   371,   373,   375,   377,
     382,   387,   389,   393,   397,   401,   403,   407,   411,   413,
     417,   421,   423,   427,   431,   435,   439,   441,   445,   449,
     451,   455,   457,   461,   463,   467,   469,   473,   475,   479,
     481,   487,   489,   493,   495,   497,   499,   501,   503,   505,
     507,   509,   511,   513,   515,   517,   521,   523,   526,   528,
     531,   533,   535,   538,   540,   546,   551,   558,   563,   565,
     567,   569,   573,   575,   580,   583,   584,   587,   593,   597,
     599,   603,   605,   607,   609,   611,   613,   615,   617,   619,
     621,   623,   625,   628,   630,   632,   634,   636,   643,   649,
     655,   660,   663,   665,   667,   669,   671,   673,   675,   678,
     684,   689,   692,   696,   705,   711,   714,   717,   721,   723,
     725,   727,   730,   732,   733,   735,   738,   741,   743,   747,
     749,   755,   762,   767,   773,   776,   778,   782,   784,   788,
     791,   793,   795,   799,   804,   808,   814,   819,   821,   823,
     826,   830,   833,   837,   841,   846,   850,   855,   861,   863,
     866,   869,   873,   875,   879,   881,   885,   889,   893,   896,
     898,   901,   903,   906,   909,   911,   915,   919,   921,   924,
     927,   932,   934,   936,   940,   942,   946,   949,   952,   954,
     956,   959,   962,   964,   966,   968,   970,   973,   976,   978,
     980,   983,   987,   990,   994,   998,  1003,  1007,  1012,  1018,
    1021,  1023,  1025,  1029,  1034,  1038,  1043,  1047,  1049,  1051,
    1053,  1055,  1057,  1059,  1061,  1063,  1065,  1067,  1069,  1075,
    1078,  1083,  1087,  1091,  1093,  1096,  1099,  1103,  1107,  1112,
    1114,  1117,  1119,  1122,  1125,  1131,  1139,  1145,  1149,  1154,
    1159,  1166,  1170,  1172,  1173,  1179,  1187,  1197,  1201,  1204,
    1207,  1210,  1214,  1222,  1229,  1234,  1236,  1238,  1245,  1251,
    1253,  1254,  1256,  1259
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const short int yyrhs[] =
{
     222,     0,    -1,     3,    -1,    78,   111,    -1,    78,    84,
     159,    85,    -1,   111,    -1,   112,    -1,     9,    -1,    10,
      -1,   113,    -1,    -1,    46,    -1,   117,    84,   153,    85,
     211,    -1,   117,    84,    85,   211,    -1,    17,   113,    86,
     152,    87,    -1,    17,   113,    86,   152,    88,    87,    -1,
      17,   113,    86,    87,    -1,    48,   211,    -1,    76,   212,
      87,    -1,    76,   159,    87,    -1,    77,   212,    87,    -1,
      77,   159,    87,    -1,    75,    84,   113,    85,    -1,   115,
      84,   135,    85,    -1,   115,    84,    85,    -1,   115,    -1,
     158,    -1,   191,    -1,   123,    88,   158,    -1,   123,    88,
     191,    -1,   114,    84,    85,    -1,   114,    84,   123,    85,
      -1,   158,    89,   158,    -1,   125,    -1,   126,    88,   125,
      -1,   113,    -1,   113,    90,   113,    -1,     4,    -1,     5,
      -1,     6,    -1,    84,   159,    85,    -1,    80,   158,    88,
     158,    81,    -1,    91,    92,    -1,    91,   135,    92,    -1,
      91,   135,    88,    92,    -1,    91,   135,    63,    92,    -1,
      91,    89,    92,    -1,    91,   126,    92,    -1,    91,   126,
      88,    92,    -1,   118,    -1,   119,    -1,   120,    -1,   121,
      -1,   124,    -1,   113,    -1,     5,    -1,     6,    -1,    84,
     128,    85,    -1,    80,   128,    88,   128,    81,    -1,    91,
      92,    -1,    91,   130,    92,    -1,    91,   130,    88,    92,
      -1,    91,   130,    63,    92,    -1,    91,    89,    92,    -1,
      91,   132,    92,    -1,    91,   132,    88,    92,    -1,   113,
      84,   129,    85,    -1,   113,    84,    85,    -1,    75,    84,
     113,    85,    -1,   115,    84,   130,    85,    -1,   115,    84,
      85,    -1,   115,    -1,   130,    -1,   131,    -1,   128,    -1,
     130,    88,   128,    -1,   113,    93,   128,    -1,   131,    88,
     113,    93,   128,    -1,   133,    -1,   132,    88,   133,    -1,
     128,    89,   128,    -1,   127,    -1,   134,    91,   159,    92,
      -1,   134,    84,    85,    -1,   134,    84,   135,    85,    -1,
     134,    94,   113,    -1,   134,    20,   113,    -1,   134,    21,
      -1,   134,    22,    -1,    19,    84,   159,    88,   191,    88,
     113,    85,    -1,   158,    -1,   135,    88,   158,    -1,   134,
      -1,   122,    -1,    21,   136,    -1,    22,   136,    -1,   137,
     138,    -1,    11,   136,    -1,    11,    84,   191,    85,    -1,
      13,   136,    -1,    13,    84,   191,    85,    -1,    12,    84,
     191,    85,    -1,    95,    -1,    96,    -1,    97,    -1,    98,
      -1,    99,    -1,   100,    -1,   136,    -1,    84,   191,    85,
     138,    -1,    86,   159,    87,   138,    -1,   138,    -1,   139,
      96,   138,    -1,   139,   101,   138,    -1,   139,   102,   138,
      -1,   139,    -1,   140,    97,   139,    -1,   140,    98,   139,
      -1,   140,    -1,   141,    23,   140,    -1,   141,    24,   140,
      -1,   141,    -1,   142,   103,   141,    -1,   142,   104,   141,
      -1,   142,    25,   141,    -1,   142,    26,   141,    -1,   142,
      -1,   143,    27,   142,    -1,   143,    28,   142,    -1,   143,
      -1,   144,    95,   143,    -1,   144,    -1,   145,   105,   144,
      -1,   145,    -1,   146,   106,   145,    -1,   146,    -1,   147,
      29,   146,    -1,   147,    -1,   148,    30,   147,    -1,   148,
      -1,   148,   107,   159,    89,   149,    -1,   149,    -1,   136,
     151,   158,    -1,    93,    -1,    34,    -1,    38,    -1,    40,
      -1,    39,    -1,    32,    -1,    33,    -1,    31,    -1,    36,
      -1,    37,    -1,    35,    -1,   113,    -1,   152,    88,   113,
      -1,   152,    -1,   152,    63,    -1,   155,    -1,   154,   155,
      -1,   163,    -1,   161,    -1,     1,   108,    -1,   150,    -1,
      47,   159,    86,   154,    87,    -1,    47,   159,    86,    87,
      -1,    49,    84,   158,    88,   135,    85,    -1,    49,    84,
     158,    85,    -1,   156,    -1,   157,    -1,   158,    -1,   159,
      88,   158,    -1,   149,    -1,    14,   177,   164,   108,    -1,
     109,   160,    -1,    -1,   177,   108,    -1,   109,   160,   177,
     164,   108,    -1,   177,   164,   108,    -1,   183,    -1,   164,
      88,   183,    -1,    59,    -1,    50,    -1,    51,    -1,    52,
      -1,    53,    -1,    54,    -1,    57,    -1,    58,    -1,    55,
      -1,    56,    -1,   165,    -1,   166,   165,    -1,   166,    -1,
     113,    -1,   168,    -1,   180,    -1,   169,   116,    86,   171,
     173,    87,    -1,   169,   116,    86,   171,    87,    -1,   169,
     116,    86,   173,    87,    -1,   169,   116,    86,    87,    -1,
     169,   113,    -1,    60,    -1,    61,    -1,    60,    -1,    61,
      -1,    62,    -1,   172,    -1,   171,   172,    -1,   109,   160,
     177,   178,   108,    -1,   109,   160,   177,   108,    -1,   177,
     108,    -1,   177,   178,   108,    -1,   109,   109,   160,   177,
     179,    89,   160,   108,    -1,   177,   179,    89,   160,   108,
      -1,   113,    89,    -1,     1,   108,    -1,   109,   160,   108,
      -1,     7,    -1,     8,    -1,   174,    -1,   174,   175,    -1,
     175,    -1,    -1,   167,    -1,   175,   167,    -1,   167,   175,
      -1,   179,    -1,   178,    88,   179,    -1,   183,    -1,    62,
     116,    86,   181,    87,    -1,    62,   116,    86,   181,    88,
      87,    -1,    62,   116,    86,    87,    -1,    62,   116,    86,
      88,    87,    -1,    62,   113,    -1,   182,    -1,   181,    88,
     182,    -1,   113,    -1,   113,    93,   160,    -1,   187,   184,
      -1,   184,    -1,   113,    -1,    84,   183,    85,    -1,   184,
      91,   160,    92,    -1,   184,    91,    92,    -1,   184,    84,
     188,    85,   176,    -1,   184,    84,    85,   176,    -1,   187,
      -1,   186,    -1,   187,   186,    -1,    84,   185,    85,    -1,
      91,    92,    -1,    91,   160,    92,    -1,   186,    91,    92,
      -1,   186,    91,   160,    92,    -1,    84,    85,   176,    -1,
     186,    84,    85,   176,    -1,   186,    84,   188,    85,   176,
      -1,    96,    -1,    96,   175,    -1,    96,   187,    -1,    96,
     175,   187,    -1,   189,    -1,   189,    88,    63,    -1,   190,
      -1,   189,    88,   190,    -1,   162,   177,   183,    -1,   162,
     177,   185,    -1,   162,   177,    -1,   192,    -1,   192,   201,
      -1,   193,    -1,   175,   193,    -1,   193,   175,    -1,   166,
      -1,   113,    90,   166,    -1,   113,    90,   113,    -1,   111,
      -1,    90,   111,    -1,   170,   113,    -1,   170,   113,    90,
     113,    -1,   112,    -1,   195,    -1,   195,    88,    63,    -1,
     196,    -1,   195,    88,   196,    -1,   197,   203,    -1,   197,
     201,    -1,   197,    -1,   198,    -1,   198,   175,    -1,   175,
     198,    -1,   166,    -1,   113,    -1,   199,    -1,   200,    -1,
     169,   113,    -1,    62,   113,    -1,   187,    -1,   202,    -1,
     187,   202,    -1,    84,   201,    85,    -1,    91,    92,    -1,
      91,   160,    92,    -1,   202,    91,    92,    -1,   202,    91,
     160,    92,    -1,    84,    85,   176,    -1,   202,    84,    85,
     176,    -1,   202,    84,   194,    85,   176,    -1,   187,   204,
      -1,   204,    -1,   113,    -1,    84,   203,    85,    -1,   204,
      91,   160,    92,    -1,   204,    91,    92,    -1,   204,    84,
     194,    85,    -1,   204,    84,    85,    -1,   211,    -1,   213,
      -1,   214,    -1,   217,    -1,   218,    -1,   221,    -1,   219,
      -1,   215,    -1,   208,    -1,   207,    -1,   206,    -1,   114,
      84,   135,    85,   211,    -1,   114,   211,    -1,    78,    86,
     212,    87,    -1,    44,   152,   108,    -1,    45,   152,   108,
      -1,   209,    -1,   210,   209,    -1,    86,    87,    -1,    86,
     210,    87,    -1,    86,   212,    87,    -1,    86,   210,   212,
      87,    -1,   205,    -1,   212,   205,    -1,   108,    -1,   159,
     108,    -1,     1,   108,    -1,    64,    84,   159,    85,   205,
      -1,    64,    84,   159,    85,   205,    65,   205,    -1,    66,
      84,   159,    85,   211,    -1,   113,    89,   205,    -1,    73,
     159,    89,   205,    -1,    83,   128,    89,   205,    -1,    83,
     128,    29,   159,    89,   205,    -1,    74,    89,   205,    -1,
     159,    -1,    -1,    67,    84,   159,    85,   205,    -1,    68,
     205,    67,    84,   159,    85,   108,    -1,    69,    84,   216,
     108,   216,   108,   216,    85,   205,    -1,    43,   113,   108,
      -1,    70,   108,    -1,    71,   108,    -1,    72,   108,    -1,
      72,   159,   108,    -1,    18,   114,    84,   153,    85,   113,
     211,    -1,    18,   114,    84,   153,    85,   211,    -1,    18,
     114,   113,   211,    -1,    15,    -1,    16,    -1,   220,   113,
      84,   153,    85,   211,    -1,   220,   113,    84,    85,   211,
      -1,   223,    -1,    -1,   224,    -1,   223,   224,    -1,   205,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   100,   100,   105,   107,   112,   113,   117,   122,   127,
     129,   133,   137,   139,   144,   146,   148,   153,   158,   160,
     162,   164,   169,   171,   173,   175,   180,   182,   184,   186,
     191,   193,   198,   208,   210,   215,   216,   218,   220,   222,
     224,   226,   228,   230,   232,   234,   239,   241,   243,   245,
     246,   247,   248,   249,   253,   254,   256,   258,   260,   262,
     264,   266,   268,   273,   275,   277,   279,   281,   283,   285,
     287,   289,   294,   295,   299,   301,   306,   311,   319,   321,
     326,   338,   339,   341,   343,   345,   347,   349,   351,   353,
     358,   360,   365,   366,   367,   369,   371,   373,   375,   377,
     379,   381,   386,   388,   390,   392,   394,   396,   401,   402,
     404,   409,   410,   412,   414,   419,   420,   422,   427,   428,
     430,   435,   436,   438,   440,   442,   447,   448,   450,   455,
     456,   461,   462,   467,   468,   473,   474,   479,   480,   485,
     486,   491,   492,   501,   503,   505,   507,   509,   511,   513,
     515,   517,   519,   521,   526,   528,   533,   535,   543,   545,
     550,   551,   552,   557,   558,   560,   565,   567,   572,   573,
     577,   578,   583,   587,   592,   595,   599,   601,   603,   608,
     610,   615,   617,   619,   621,   623,   625,   627,   629,   631,
     633,   637,   639,   644,   646,   648,   649,   653,   655,   657,
     659,   661,   666,   668,   673,   675,   677,   682,   691,   702,
     704,   706,   708,   710,   712,   715,   717,   722,   728,   729,
     733,   734,   738,   739,   743,   744,   746,   751,   753,   758,
     762,   764,   766,   768,   770,   775,   777,   784,   786,   791,
     793,   797,   799,   801,   803,   805,   807,   812,   813,   814,
     819,   821,   823,   825,   827,   829,   831,   833,   838,   840,
     842,   844,   849,   851,   856,   858,   863,   865,   867,   872,
     874,   879,   880,   882,   887,   889,   892,   895,   898,   902,
     904,   907,   911,   913,   918,   920,   925,   927,   929,   934,
     935,   937,   942,   944,   946,   947,   951,   956,   961,   962,
     963,   968,   970,   972,   974,   976,   978,   980,   982,   987,
     989,   993,   995,   997,   999,  1001,  1003,  1008,  1009,  1010,
    1011,  1012,  1013,  1014,  1015,  1016,  1017,  1018,  1022,  1024,
    1029,  1034,  1039,  1044,  1046,  1051,  1061,  1071,  1083,  1098,
    1100,  1105,  1107,  1109,  1114,  1116,  1118,  1124,  1126,  1128,
    1130,  1132,  1137,  1139,  1143,  1145,  1147,  1152,  1154,  1156,
    1158,  1160,  1165,  1167,  1169,  1174,  1176,  1181,  1183,  1188,
    1189,  1193,  1194,  1198
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
  "CONTINUE", "BREAK", "RETURN", "CASE", "DEFAULT", "SYNTAXIDID",
  "SYNTAXQUOTE", "SYNTAXQUASI", "SYNTAXUNQUOTE", "SYNTAXSPLICE", "LPAIR",
  "RPAIR", "NOBIND_PRE", "MATCH", "'('", "')'", "'{'", "'}'", "','", "':'",
  "'`'", "'['", "']'", "'='", "'.'", "'&'", "'*'", "'+'", "'-'", "'~'",
  "'!'", "'/'", "'%'", "'<'", "'>'", "'^'", "'|'", "'?'", "';'", "'@'",
  "$accept", "_id", "unquote_expr", "id", "atid", "syntaxid", "maybeid",
  "lambda", "lambda_expression", "defrec_expression", "let_expression",
  "quote_expression", "syntax_expression", "margument_expression_list",
  "mcall_expression", "table_init", "table_init_list",
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
  "statement", "mcall_statement", "unquote_statement", "global_statement",
  "local", "local_list", "compound_statement", "statement_list",
  "expression_statement", "selection_statement", "labeled_statement",
  "maybe_expression", "iteration_statement", "jump_statement",
  "defstx_statement", "define", "define_statement", "translation_unit_seq",
  "translation_unit", "external_declaration", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   110,   111,   112,   112,   113,   113,   114,   115,   116,
     116,   117,   118,   118,   119,   119,   119,   120,   121,   121,
     121,   121,   122,   122,   122,   122,   123,   123,   123,   123,
     124,   124,   125,   126,   126,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   128,   128,   128,   128,   128,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   128,   128,
     128,   128,   129,   129,   130,   130,   131,   131,   132,   132,
     133,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     135,   135,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   137,   137,   137,   137,   137,   137,   138,   138,
     138,   139,   139,   139,   139,   140,   140,   140,   141,   141,
     141,   142,   142,   142,   142,   142,   143,   143,   143,   144,
     144,   145,   145,   146,   146,   147,   147,   148,   148,   149,
     149,   150,   150,   151,   151,   151,   151,   151,   151,   151,
     151,   151,   151,   151,   152,   152,   153,   153,   154,   154,
     155,   155,   155,   156,   156,   156,   157,   157,   158,   158,
     159,   159,   160,   161,   162,   162,   163,   163,   163,   164,
     164,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   166,   166,   167,   167,   167,   167,   168,   168,   168,
     168,   168,   169,   169,   170,   170,   170,   171,   171,   172,
     172,   172,   172,   172,   172,   172,   172,   173,   174,   174,
     175,   175,   176,   176,   177,   177,   177,   178,   178,   179,
     180,   180,   180,   180,   180,   181,   181,   182,   182,   183,
     183,   184,   184,   184,   184,   184,   184,   185,   185,   185,
     186,   186,   186,   186,   186,   186,   186,   186,   187,   187,
     187,   187,   188,   188,   189,   189,   190,   190,   190,   191,
     191,   192,   192,   192,   193,   193,   193,   193,   193,   193,
     193,   193,   194,   194,   195,   195,   196,   196,   196,   197,
     197,   197,   198,   198,   198,   198,   199,   200,   201,   201,
     201,   202,   202,   202,   202,   202,   202,   202,   202,   203,
     203,   204,   204,   204,   204,   204,   204,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   206,   206,
     207,   208,   209,   210,   210,   211,   211,   211,   211,   212,
     212,   213,   213,   213,   214,   214,   214,   215,   215,   215,
     215,   215,   216,   216,   217,   217,   217,   218,   218,   218,
     218,   218,   219,   219,   219,   220,   220,   221,   221,   222,
     222,   223,   223,   224
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     2,     4,     1,     1,     1,     1,     1,
       0,     1,     5,     4,     5,     6,     4,     2,     3,     3,
       3,     3,     4,     4,     3,     1,     1,     1,     3,     3,
       3,     4,     3,     1,     3,     1,     3,     1,     1,     1,
       3,     5,     2,     3,     4,     4,     3,     3,     4,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     5,     2,
       3,     4,     4,     3,     3,     4,     4,     3,     4,     4,
       3,     1,     1,     1,     1,     3,     3,     5,     1,     3,
       3,     1,     4,     3,     4,     3,     3,     2,     2,     8,
       1,     3,     1,     1,     2,     2,     2,     2,     4,     2,
       4,     4,     1,     1,     1,     1,     1,     1,     1,     4,
       4,     1,     3,     3,     3,     1,     3,     3,     1,     3,
       3,     1,     3,     3,     3,     3,     1,     3,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       5,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     2,     1,     2,
       1,     1,     2,     1,     5,     4,     6,     4,     1,     1,
       1,     3,     1,     4,     2,     0,     2,     5,     3,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     1,     1,     1,     6,     5,     5,
       4,     2,     1,     1,     1,     1,     1,     1,     2,     5,
       4,     2,     3,     8,     5,     2,     2,     3,     1,     1,
       1,     2,     1,     0,     1,     2,     2,     1,     3,     1,
       5,     6,     4,     5,     2,     1,     3,     1,     3,     2,
       1,     1,     3,     4,     3,     5,     4,     1,     1,     2,
       3,     2,     3,     3,     4,     3,     4,     5,     1,     2,
       2,     3,     1,     3,     1,     3,     3,     3,     2,     1,
       2,     1,     2,     2,     1,     3,     3,     1,     2,     2,
       4,     1,     1,     3,     1,     3,     2,     2,     1,     1,
       2,     2,     1,     1,     1,     1,     2,     2,     1,     1,
       2,     3,     2,     3,     3,     4,     3,     4,     5,     2,
       1,     1,     3,     4,     3,     4,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     5,     2,
       4,     3,     3,     1,     2,     2,     3,     3,     4,     1,
       2,     1,     2,     2,     5,     7,     5,     3,     4,     4,
       6,     3,     1,     0,     5,     7,     9,     3,     2,     2,
       2,     3,     7,     6,     4,     1,     1,     6,     5,     1,
       0,     1,     2,     1
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
       0,     0,     0,     0,     2,     1,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0
};

/* YYMERGER[RULE-NUM] -- Index of merging function for rule #RULE-NUM.  */
static const unsigned char yymerger[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     1,     1,     1,     1,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     2,     2,     2,
       2,     0,     0,     0,     0,     0,     0,     0,     3,     3,
       0,     3,     3,     0,     0,     3,     3,     3,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     3,
       3,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error.  */
static const unsigned short int yydefact[] =
{
       0,     0,     2,    37,    38,    39,     7,     8,     0,     0,
       0,   365,   366,     0,     0,     0,     0,     0,     0,     0,
      11,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   102,   103,   104,   105,   106,   107,   341,
       5,     6,    35,     0,    25,     0,    49,    50,    51,    52,
      93,    53,    81,    92,   108,     0,   111,   115,   118,   121,
     126,   129,   131,   133,   135,   137,   139,   141,   163,   168,
     169,   170,     0,   373,   327,   326,   325,   317,   318,   319,
     324,   320,   321,   323,     0,   322,     0,     0,   371,   343,
       0,     0,    35,     0,    97,     0,     0,    99,     0,     0,
       0,     0,    94,    95,     0,   154,     0,     0,     0,     0,
      17,     0,     0,     0,     0,     0,   353,   358,   359,   360,
       0,     0,     0,     0,     0,   339,     0,     0,     0,     0,
       0,     3,     0,    55,    56,     0,     0,     0,     0,    54,
      71,     0,   218,   219,   182,   183,   184,   185,   186,   189,
     190,   187,   188,   181,   204,   205,   206,     0,     5,     6,
      35,     0,   191,   274,     0,   220,     0,     0,   269,   271,
       0,   335,     0,   333,     0,     0,     0,    42,    33,     0,
       0,    90,     0,     0,     0,   329,     0,     0,     0,    87,
      88,     0,     0,     0,   150,   148,   149,   144,   153,   151,
     152,   145,   147,   146,   143,     0,   108,    96,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   342,     0,
       1,   372,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   357,     0,   331,     0,     0,     0,     0,     0,     0,
       0,   352,     0,   361,     0,   351,     0,    19,    18,   340,
      21,    20,     0,     0,     0,     0,     0,     0,     0,    59,
      74,     0,     0,    78,     0,     0,     0,     0,   278,     0,
      40,   192,   279,   221,   272,     0,     0,     0,   258,   298,
     270,   299,   273,     0,     0,   336,   334,     0,   337,    46,
       0,    47,     0,     0,    43,     0,   347,    36,    30,     0,
       0,    26,    27,    24,     0,    90,     0,   156,     0,    86,
      83,     0,     0,    85,   142,   112,   113,   114,   116,   117,
     119,   120,   124,   125,   122,   123,   127,   128,   130,   132,
     134,   136,   138,     0,   171,     0,    98,    26,     0,   101,
     100,    16,     0,     0,   364,     0,   155,     0,     0,   202,
     203,    10,   165,     0,   194,     0,   158,   161,   160,   193,
     224,   195,    10,     0,     0,   196,   167,     0,     0,     0,
       0,     0,   353,   348,    22,     4,   330,     0,     0,     0,
      57,    63,     0,     0,     0,    60,     0,    64,    67,    54,
      74,     0,    72,    73,    70,     0,     0,   349,    36,   275,
       0,   109,   223,     0,   302,   172,     0,   259,   260,   300,
       0,     0,   332,   110,   338,    48,    34,     0,    45,    44,
      91,    32,    31,     0,     0,     0,    23,    13,   157,     0,
      84,    82,     0,     0,     0,   276,    14,     0,     0,     0,
     162,     0,   234,     0,     0,   164,   159,   226,   201,     0,
     225,     0,   176,   241,     0,   179,   240,     0,     0,   344,
     346,   354,     0,     0,    41,    68,     0,    80,    62,    61,
      75,    65,     0,    79,     0,    66,     0,     0,    69,     0,
     280,   222,   306,   301,   303,   261,     0,   223,   293,   292,
       0,     0,     0,   282,   284,   288,   289,   294,   295,   304,
       0,    28,    29,   328,    12,   140,   368,     0,    15,     0,
     363,     0,     0,     0,     0,     0,     0,     0,   178,   175,
       0,   239,   166,     0,     0,   353,    58,    76,     0,   350,
     297,   307,   296,   291,   223,     0,     0,   311,   298,   287,
     286,   310,   290,   305,   367,   362,     0,   173,   232,     0,
     237,     0,   235,     0,     0,   200,     0,   194,     0,   207,
       0,     0,   242,   180,   223,     0,     0,     0,   262,   264,
     244,     0,   345,   355,     0,     0,   308,   283,   285,     0,
     309,     0,     0,    89,   233,     0,   230,     0,   177,   216,
       0,     0,   215,   198,   208,     0,   199,   211,     0,   227,
     229,   246,   174,   268,   223,   175,   243,     0,    77,   312,
     316,     0,   314,     0,   238,   231,   236,     0,   217,     0,
     197,     0,   212,     0,     0,     0,   266,   267,   248,   247,
     245,   263,   265,   356,   315,   313,     0,   210,     0,   227,
     228,     0,   223,     0,   251,     0,   175,     0,   249,     0,
     209,   214,   255,   250,   252,   223,     0,   253,     0,     0,
     256,   223,   254,     0,   257,   213
};

/* YYPDEFGOTO[NTERM-NUM].  */
static const short int yydefgoto[] =
{
      -1,    50,    51,   102,   103,    54,   463,    55,    56,    57,
      58,    59,    60,   319,    61,   188,   189,    62,   410,   411,
     281,   413,   282,   283,    63,   190,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,   215,   327,   328,   375,   376,    79,    80,    81,    82,
     426,   377,   586,   378,   474,   172,   379,   380,   381,   382,
     174,   578,   579,   580,   175,   383,   502,   384,   618,   619,
     385,   571,   572,   620,   476,   647,   648,   477,   587,   588,
     589,   322,   178,   179,   512,   513,   514,   515,   516,   517,
     518,   423,   301,   560,   561,   135,    84,    85,    86,   183,
     184,    87,   185,    88,    89,    90,   262,    91,    92,    93,
      94,    95,    96,    97,    98
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -474
static const short int yypact[] =
{
     950,   -81,  -474,  -474,  -474,  -474,  -474,  -474,  3663,   -52,
    3761,  -474,  -474,    45,    73,    -5,  3807,  3807,    45,    45,
    -474,  2931,   -31,     3,     9,    30,    38,  2038,   108,   -36,
     125,   803,  2931,   154,   193,  2038,  2038,    18,  2931,  1098,
    2343,  1338,  2441,  -474,  -474,  -474,  -474,  -474,  -474,  -474,
    -474,  -474,    94,   226,   216,   241,  -474,  -474,  -474,  -474,
    -474,  -474,  -474,   739,  1162,  3617,  -474,   -18,   322,   486,
     230,   501,   243,   206,   238,   325,   -19,  -474,  -474,  -474,
    -474,  -474,   -17,  -474,  -474,  -474,  -474,  -474,  -474,  -474,
    -474,  -474,  -474,  -474,    45,  -474,   362,  1051,  -474,  -474,
      32,  2343,   268,   288,  -474,  1311,  2343,  -474,   290,   118,
    2931,  2931,  -474,  -474,   276,  -474,    57,  2931,    63,  1338,
    -474,  2931,  2931,  2931,  2931,   328,  2931,  -474,  -474,  -474,
     157,   471,  2038,    45,   149,  -474,  1538,   186,  1638,  2931,
    2038,  -474,   309,  -474,  -474,   319,  1098,  1098,   491,   327,
     331,    29,  -474,  -474,  -474,  -474,  -474,  -474,  -474,  -474,
    -474,  -474,  -474,  -474,  -474,  -474,  -474,   406,   540,   633,
     349,   187,  -474,  1213,    45,   583,  2114,   347,   155,   583,
      45,  -474,   198,  -474,  1438,  1738,   363,  -474,  -474,   -23,
     -32,   345,  2038,    45,  2245,  -474,  2539,    23,    45,  -474,
    -474,  2637,  2931,    45,  -474,  -474,  -474,  -474,  -474,  -474,
    -474,  -474,  -474,  -474,  -474,  2931,  -474,  -474,  3617,  3617,
    3617,  3617,  3617,  3617,  3617,  3617,  3617,  3617,  3617,  3617,
    3617,  3617,  3617,  3617,  3617,  3617,  2931,  2931,  -474,   357,
    -474,  -474,   365,  2245,   368,   377,   391,    12,    45,   -31,
     396,  -474,    45,  -474,   508,  1153,   282,   297,   339,   358,
     409,   400,   407,  -474,  2038,  -474,   413,  -474,  -474,  -474,
    -474,  -474,   372,  1838,  2931,    45,   416,   449,   446,  -474,
     441,   -29,   150,  -474,   648,   904,  2931,  2038,  -474,  1614,
    -474,  -474,   454,  -474,  -474,  3617,   422,  3029,    10,   104,
    -474,   245,  -474,   172,  3617,  -474,  -474,  1938,  -474,  -474,
    2735,  -474,   460,  2833,  -474,  2931,  -474,  -474,  -474,   383,
     384,   390,  -474,  -474,   404,  -474,   -31,   -27,   478,  -474,
    -474,   426,   210,  -474,  -474,  -474,  -474,  -474,   -18,   -18,
     322,   322,   486,   486,   486,   486,   230,   230,   501,   243,
     206,   238,   325,   510,  -474,    70,  -474,  -474,  1614,  -474,
    -474,  -474,   516,   480,  -474,  2343,  -474,   465,  2717,  -474,
    -474,    45,  -474,  3617,  -474,  1167,  -474,  -474,  -474,  1213,
     583,  -474,    45,  1414,     2,  -474,  -474,  2931,  2038,   -31,
    2038,  2931,  2931,  -474,  -474,  -474,  -474,   498,   503,  1098,
    -474,  -474,  1098,   518,   702,  -474,   999,  -474,  -474,   -16,
    -474,   515,   505,   527,  -474,   435,   517,  -474,   747,  1213,
      45,  -474,   583,   533,  -474,  -474,   546,   536,  -474,   245,
    2423,  3127,  -474,  -474,  -474,  -474,  -474,   345,  -474,  -474,
    -474,  -474,  -474,  2343,   -31,  2931,  -474,  -474,  -474,   -31,
    -474,  -474,  3617,   -31,   557,  -474,  -474,    49,    86,   556,
    -474,    51,   560,   562,  2717,  -474,  -474,  -474,   560,   566,
    -474,    51,  -474,  -474,   196,  -474,   255,   259,   448,   591,
    -474,  -474,   468,   565,  -474,  -474,   584,  -474,  -474,  -474,
    -474,  -474,   441,  -474,  1098,  -474,  1098,    45,  -474,  2038,
    -474,  -474,  -474,  -474,  -474,  -474,    45,   583,  -474,  1213,
      45,  1514,   590,   595,  -474,   302,   583,  -474,  -474,  -474,
     592,  -474,  -474,  -474,  -474,  -474,  -474,   -31,  -474,   -31,
    -474,    45,   213,   403,    51,  1229,   600,    51,  -474,   -45,
    3225,   255,  -474,  2038,   582,  2931,  -474,  -474,   598,  -474,
    -474,  -474,  -474,  -474,   583,  2619,    35,  -474,    46,  -474,
    -474,   258,  -474,  -474,  -474,  -474,   608,  -474,  -474,   607,
     602,   521,  -474,   215,   588,  -474,  2138,   609,  1243,  -474,
     610,    89,  -474,  -474,   583,  3617,  2717,   614,   612,  -474,
    -474,   611,  -474,  -474,   616,  1098,  -474,  -474,  -474,   619,
     258,  2521,  3323,  -474,  -474,  3617,  -474,    91,  -474,  -474,
    3617,   865,  -474,  -474,  -474,   622,  -474,  -474,   227,   617,
    -474,  -474,  -474,   370,   583,   -33,  -474,  2038,  -474,  -474,
    -474,   625,  -474,   624,  -474,  -474,  -474,  2717,  -474,   329,
    -474,    51,  -474,  3617,   344,  3421,  -474,  -474,   269,    72,
    -474,  -474,  -474,  -474,  -474,  -474,    51,  -474,   240,  -474,
    -474,   606,   583,   634,  -474,   628,   -35,  3519,   269,   641,
    -474,  -474,  -474,  -474,  -474,   583,   637,  -474,   639,  3617,
    -474,   583,  -474,   626,  -474,  -474
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -474,   445,    88,     6,   438,   270,   353,  -474,  -474,  -474,
    -474,  -474,  -474,  -474,  -474,   427,  -474,  -474,   120,  -474,
     342,  -474,  -474,   330,  -474,  -157,    -8,  -474,   -12,   439,
     440,   174,   450,   507,   512,   509,   519,   514,  -474,   261,
    -474,  -474,    -6,  -238,  -474,   375,  -474,  -474,   547,   646,
    -288,  -474,  -474,  -474,  -439,  -150,    56,   382,  -474,  -414,
    -474,  -474,   173,   180,  -474,    65,  -306,  -259,   128,  -273,
    -474,  -474,   164,  -303,  -473,   129,   126,  -164,   110,  -474,
     153,   -39,  -474,   603,   182,  -474,   219,  -474,   277,  -474,
    -474,  -175,  -287,   225,   229,    43,  -474,  -474,  -474,   605,
    -474,    -2,    -7,  -474,  -474,  -474,  -385,  -474,  -474,  -474,
    -474,  -474,  -474,  -474,   694
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -371
static const short int yytable[] =
{
     104,   177,   107,   300,   541,     2,    52,   483,   112,   113,
     363,   235,   429,   116,   299,     2,   510,   152,   153,   108,
     120,     2,   532,   291,   114,   115,     2,    99,   136,   138,
     651,   312,   105,    52,   403,     2,   448,   320,     2,   324,
     584,    52,    52,    83,   331,   149,   170,    52,     2,     2,
     675,   195,     2,   217,     2,   119,   313,   216,   286,   404,
     314,   252,   242,   405,   585,   310,   245,   246,   284,   311,
     125,   237,   127,     2,   585,     2,   585,   494,   218,   110,
     100,   475,     6,   219,   220,   464,   471,   121,   236,     2,
     100,   238,     2,   122,     2,   573,   173,   510,   298,   361,
     239,   100,   139,    52,   140,   176,   298,   170,   326,   461,
     472,   244,   170,   100,   123,   249,   139,   454,   287,   556,
     422,     2,   124,   100,   100,    52,   297,   100,   169,   100,
     556,   298,   299,   273,   428,   471,   528,   297,    52,   266,
      83,   510,    52,   520,    52,   252,    52,   298,   100,   255,
     100,   237,   149,   149,   149,   453,   644,   173,   475,   151,
     594,   173,   173,   645,   100,   253,   176,   100,   536,   100,
     176,   176,   119,   471,   303,   265,   541,   307,   635,   269,
     292,   269,   244,   192,   193,   298,   115,   510,   296,   169,
      52,    52,   126,   169,   169,   297,   100,   617,    52,   317,
     170,   551,   248,   115,   329,   534,   335,   336,   337,   333,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   269,   291,
     478,   475,   173,   128,   583,   316,   267,   237,   406,   296,
     293,   362,   407,   132,   302,   237,   297,   364,   596,   170,
     173,   298,   591,   115,   115,   225,   226,   238,   366,   176,
     252,   374,     2,   505,   169,   263,   276,   277,   280,   291,
      52,   429,   290,   270,   237,   237,   581,   133,   621,    52,
     432,   398,   169,   421,   537,   304,   237,   216,   611,   216,
     409,   149,   433,    52,   238,   418,   216,   622,   237,   173,
     196,   537,   451,   537,   538,     2,   238,   393,   176,   150,
     194,   232,   119,    52,   633,   641,   269,   634,   650,   581,
     646,   567,   637,   608,   447,   197,   459,   623,   641,   430,
     417,   169,     2,   227,   228,   642,   431,   100,   231,   539,
     559,   536,   601,   471,   233,   419,   540,     2,   670,   602,
     269,   558,   639,   666,   234,   661,   672,   665,   193,   291,
     667,   115,   240,   427,   455,   216,   659,   386,   660,   680,
     387,   170,   243,     2,   374,   684,   247,   462,   656,   678,
     100,   374,   388,   669,   251,   237,   556,   480,   468,   374,
     473,   683,   558,   297,    52,   260,    52,   274,   298,   342,
     343,   344,   345,   275,   522,   149,     2,   100,   149,     2,
     149,   284,   149,   471,   419,   285,   150,   150,   150,   221,
     222,   173,   100,   216,   389,   298,   500,   237,   644,   662,
     176,   479,   295,   481,   315,   645,   508,   657,    53,   289,
     298,   355,   523,   390,   216,   467,   237,   524,   100,   170,
     356,   526,   109,   169,   644,   309,   530,   395,   358,   649,
     237,   645,   359,   366,   529,    53,   298,   473,   442,   444,
     374,   443,   445,    53,    53,   -26,   360,   473,   -26,    53,
     649,   100,   141,   473,   365,   168,   509,   501,   237,   446,
     568,   569,   445,   391,     2,   511,   143,   144,   394,   173,
     149,     7,   149,   548,   399,    52,   296,   422,   176,   223,
     224,   450,   550,   297,   445,   392,   552,   508,   298,   486,
     498,   557,   487,   496,   490,   564,   492,   565,   229,   230,
     402,   169,   216,   542,   400,    53,   445,   566,   401,   570,
     473,   577,   549,   473,   420,   141,   168,  -277,  -277,    52,
     168,   168,   438,   544,   150,   150,   237,    53,   425,   237,
     264,   508,   557,   449,   557,   458,   145,   509,   216,   100,
      53,   146,   501,   460,    53,   147,    53,   216,    53,   484,
     278,   562,   148,   279,   577,   142,   592,   473,   485,   191,
     152,   153,   374,   496,   216,   304,   237,   216,   237,   452,
     495,   149,   216,   456,   457,   237,   499,   508,   606,   607,
     488,   509,   288,   570,   547,   497,   490,   374,   503,   501,
     511,   168,    53,    53,    -5,    -5,   412,   415,    -5,   473,
      53,    -5,   298,    52,   425,   216,    -5,   216,   504,   168,
    -281,  -281,   527,   374,   531,   473,    -9,   473,   533,   501,
     473,     2,   535,   143,   144,   473,   543,   509,     7,   216,
     338,   339,   473,   340,   341,   546,   511,   118,   256,   150,
     653,   216,   150,   545,   150,   554,   150,   130,   131,   346,
     347,   134,   137,   555,   563,   582,   171,   182,   168,   501,
     593,   595,   425,   603,   604,   605,   609,   616,   612,   624,
     625,   627,    53,   626,   629,     2,   643,   143,   144,   640,
     654,    53,     7,   525,   671,   628,   655,    -6,    -6,   673,
     674,    -6,   681,   145,    -6,    53,   100,   501,   146,    -6,
     679,   682,   147,   408,   685,   469,   493,   436,   348,   148,
     501,   321,   350,   325,   349,    53,   501,   171,   325,   352,
     466,   614,   171,   351,  -276,  -276,   250,   171,   615,   198,
     199,   200,   334,   254,   150,   470,   150,   658,   257,   258,
     259,   636,   261,   663,   598,   668,   676,   145,   652,   294,
     100,   599,   146,   631,   354,   272,   147,   600,   553,   306,
     357,   241,     0,   148,   489,     0,     0,     0,     0,     0,
       0,   425,     0,     0,     0,     0,     2,     3,     4,     5,
     168,     0,     6,     7,     8,     9,    10,     0,     0,     0,
      13,   397,    15,   201,    16,    17,    53,     0,    53,     0,
     202,   -36,   -36,   203,     0,   -36,     0,   425,   -36,     0,
       0,     0,     0,   -36,     0,     0,   425,     0,   332,    20,
      21,    22,    23,     0,     0,     0,     0,   437,     0,     0,
     440,     0,   441,   425,     0,   150,   425,     0,     2,     0,
       0,   425,   152,   153,     0,     0,     0,     0,    34,    35,
      36,   100,   353,    38,     0,     0,     0,    40,   168,   117,
       0,     0,     0,     0,    42,     0,     0,     0,    43,    44,
      45,    46,    47,    48,   425,     0,   425,     2,     0,   143,
     144,   129,   354,     0,     7,   154,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   369,   370,   371,   425,     0,
       0,     0,   416,     0,   325,     0,     0,    53,     0,     0,
     425,     0,     0,   100,     0,     0,     0,     0,     0,     0,
    -370,     1,     0,     2,     3,     4,     5,     0,     0,     6,
       7,     8,     9,    10,     0,    11,    12,    13,    14,    15,
       0,    16,    17,   638,     0,     0,     0,     0,     0,   145,
       0,    53,   100,     0,   146,     0,     0,     0,   147,   414,
     521,     0,   440,    18,    19,   148,    20,    21,    22,    23,
       0,     0,     2,     0,   143,   144,     0,     0,     0,     7,
       0,     0,     0,     0,    24,     0,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,     0,
      38,     0,     0,    39,    40,     0,    41,   482,   261,     0,
       0,    42,     0,     0,     0,    43,    44,    45,    46,    47,
      48,  -369,     1,     0,     2,     3,     4,     5,    49,     0,
       6,     7,     8,     9,    10,    53,    11,    12,    13,    14,
      15,     0,    16,    17,   145,     0,     0,   100,     0,   146,
       0,     0,     0,   147,     0,     0,     0,     0,     0,     0,
     148,   491,     0,     0,    18,    19,     0,    20,    21,    22,
      23,     2,     0,   143,   144,     0,     0,     0,     7,     0,
       0,     0,     0,     0,     0,    24,     0,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
       0,    38,     0,     0,    39,    40,     0,    41,     0,     0,
       0,     0,    42,     0,     0,     0,    43,    44,    45,    46,
      47,    48,     0,     0,   367,     0,     2,     0,     0,    49,
     152,   153,     0,     0,     0,     0,     0,   368,   367,     0,
       2,     0,     0,   145,   152,   153,   100,     0,   146,     0,
       0,   368,   147,     0,     0,     0,     0,     0,     0,   148,
       0,   261,     0,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   369,   370,   371,     0,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   369,   370,   371,
     574,   100,     2,     0,     0,     0,   152,   153,     0,     0,
     372,     0,     0,     0,   574,   100,     2,     0,     0,     0,
     152,   153,     0,     0,   465,   214,     0,     0,     0,     0,
       0,     0,   373,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,     0,     0,     0,   373,     0,     0,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   369,
     370,   371,     0,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   369,   370,   371,     0,   100,     0,     0,
       0,     0,     0,     0,     2,     0,   575,     0,   152,   153,
       0,   100,     0,     0,     0,     0,     0,     0,     0,     0,
     613,     0,     0,     0,     0,     0,     0,     0,   576,     1,
       0,     2,     3,     4,     5,     0,     0,     6,     7,     8,
       9,    10,   576,    11,    12,    13,    14,    15,     0,    16,
      17,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,     0,     0,     0,     0,     0,     0,
       0,    18,    19,   180,    20,    21,    22,    23,     0,   100,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   167,    24,     0,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,     2,    38,     0,
       0,    39,    40,     0,    41,   181,     0,     0,     0,    42,
       0,     0,     0,    43,    44,    45,    46,    47,    48,     1,
       0,     2,     3,     4,     5,     0,    49,     6,     7,     8,
       9,    10,     0,    11,    12,    13,    14,    15,     0,    16,
      17,     0,     0,     0,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,   369,   370,   371,     0,     0,     0,
       0,    18,    19,   180,    20,    21,    22,    23,     0,     0,
       0,     0,   100,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    24,     0,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,     2,    38,     0,
       0,    39,    40,     0,    41,   305,     0,     0,     0,    42,
       0,     0,     0,    43,    44,    45,    46,    47,    48,     1,
       0,     2,     3,     4,     5,     0,    49,     6,     7,     8,
       9,    10,     0,    11,    12,    13,    14,    15,     0,    16,
      17,     0,     0,     0,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,   369,   370,   506,     0,     0,     0,
       0,    18,    19,     0,    20,    21,    22,    23,     0,     0,
       0,     0,   100,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    24,     0,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,     2,    38,     0,
       0,    39,    40,     0,    41,   268,     0,     0,     0,    42,
       0,     0,     0,    43,    44,    45,    46,    47,    48,     1,
       0,     2,     3,     4,     5,     0,    49,     6,     7,     8,
       9,    10,     0,    11,    12,    13,    14,    15,     0,    16,
      17,     0,     0,     0,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,     0,     0,     0,     0,     0,     0,
       0,    18,    19,     0,    20,    21,    22,    23,     0,     0,
       0,     0,   100,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    24,     0,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,     0,    38,     0,
       0,    39,    40,     0,    41,   271,     0,     0,     0,    42,
       0,     0,     0,    43,    44,    45,    46,    47,    48,     1,
       0,     2,     3,     4,     5,     0,    49,     6,     7,     8,
       9,    10,     0,    11,    12,    13,    14,    15,     0,    16,
      17,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    18,    19,     0,    20,    21,    22,    23,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    24,     0,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,     0,    38,     0,
       0,    39,    40,     0,    41,   308,     0,     0,     0,    42,
       0,     0,     0,    43,    44,    45,    46,    47,    48,     1,
       0,     2,     3,     4,     5,     0,    49,     6,     7,     8,
       9,    10,     0,    11,    12,    13,    14,    15,     0,    16,
      17,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    18,    19,     0,    20,    21,    22,    23,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    24,     0,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,     0,    38,     0,
       0,    39,    40,     0,    41,   396,     0,     0,     0,    42,
       0,     0,     0,    43,    44,    45,    46,    47,    48,     1,
       0,     2,     3,     4,     5,     0,    49,     6,     7,     8,
       9,    10,     0,    11,    12,    13,    14,    15,     0,    16,
      17,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    18,    19,     0,    20,    21,    22,    23,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    24,     0,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,     0,    38,     0,
       0,    39,    40,     0,    41,   434,     0,     0,     0,    42,
       0,     0,     0,    43,    44,    45,    46,    47,    48,     1,
       0,     2,     3,     4,     5,     0,    49,     6,     7,     8,
       9,    10,     0,    11,    12,    13,    14,    15,     0,    16,
      17,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    18,    19,     0,    20,    21,    22,    23,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    24,     0,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,     2,    38,     0,
       0,    39,    40,     0,    41,     0,     0,     0,     0,    42,
       0,     0,     0,    43,    44,    45,    46,    47,    48,     0,
       0,     2,     3,     4,     5,     0,    49,     6,     7,     8,
       9,    10,     0,     0,     0,    13,     0,    15,     0,    16,
      17,     0,     0,     0,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,   164,   165,   166,     0,     0,     0,
       0,     0,     0,     0,    20,     0,    22,     0,     0,     0,
       0,     0,   100,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   167,     0,     0,     0,     0,     0,
       0,     0,     0,    34,    35,    36,   100,     0,    38,     0,
       0,     0,    40,     0,   117,     0,     0,     0,     0,    42,
       0,     0,     0,    43,    44,    45,    46,    47,    48,     0,
       0,     0,     0,     0,     0,     0,     0,   610,     2,     3,
       4,     5,   152,   153,     6,     7,     8,     9,    10,     0,
       0,     0,    13,     0,    15,     0,    16,    17,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    20,    21,    22,    23,   154,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   164,   165,   166,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      34,    35,    36,   100,     0,    38,     0,     0,     0,    40,
     318,   117,     0,     0,     0,   167,    42,     0,     0,     0,
      43,    44,    45,    46,    47,    48,     2,     3,     4,     5,
     152,   153,     6,     7,     8,     9,    10,     0,     0,     0,
      13,     0,    15,     0,    16,    17,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    20,
      21,    22,    23,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    35,
      36,   100,     0,    38,     0,     0,     2,    40,     0,   117,
     152,   153,     0,   167,    42,     0,     0,     0,    43,    44,
      45,    46,    47,    48,     2,     3,     4,     5,     0,     0,
       6,     7,     8,     9,    10,     0,     0,     0,    13,     0,
      15,     0,    16,    17,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   369,   370,   506,     0,    20,    21,    22,
      23,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   100,     0,     0,     0,     0,     0,     0,   507,     0,
       0,     0,     0,     0,     0,     0,    34,    35,    36,   100,
       0,    38,     0,     0,     2,    40,     0,   117,   152,   153,
     186,     0,    42,   187,     0,     0,    43,    44,    45,    46,
      47,    48,     2,     3,     4,     5,     0,     0,     6,     7,
       8,     9,    10,     0,     0,     0,    13,     0,    15,     0,
      16,    17,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   369,   370,   506,     0,    20,    21,    22,    23,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   100,
       0,     0,     0,     0,     0,     0,   630,     0,     0,     0,
       0,     0,     0,     0,    34,    35,    36,   100,     0,    38,
       0,     0,     2,    40,   323,   117,   152,   153,     0,     0,
      42,     0,     0,     0,    43,    44,    45,    46,    47,    48,
       2,     3,     4,     5,     0,     0,     6,     7,     8,     9,
      10,     0,     0,     0,    13,     0,    15,     0,    16,    17,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   369,
     370,   506,   597,    20,    21,    22,    23,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   100,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    34,    35,    36,   100,     0,    38,     0,     0,
       2,    40,   330,   117,   152,   153,     0,     0,    42,     0,
       0,     0,    43,    44,    45,    46,    47,    48,     2,     3,
       4,     5,     0,     0,     6,     7,     8,     9,    10,     0,
       0,     0,    13,     0,    15,     0,    16,    17,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   369,   370,   371,
       0,    20,    21,    22,    23,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   100,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      34,    35,    36,   100,     0,    38,     0,     0,     0,    40,
       0,   117,     0,     0,     0,     0,    42,   435,     0,     0,
      43,    44,    45,    46,    47,    48,     2,     3,     4,     5,
       0,     0,     6,     7,     8,     9,    10,     0,     0,     0,
      13,     0,    15,     0,    16,    17,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    20,
      21,    22,    23,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    35,
      36,   100,     0,    38,     0,     0,     0,    40,     0,   117,
       0,     0,     0,     0,    42,   439,     0,     0,    43,    44,
      45,    46,    47,    48,     2,     3,     4,     5,     0,     0,
       6,     7,     8,     9,    10,     0,     0,     0,    13,     0,
      15,     0,    16,    17,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    20,    21,    22,
      23,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    34,    35,    36,   100,
       0,    38,     0,     0,     0,    40,     0,   117,     0,     0,
       0,     0,    42,     0,     0,     0,    43,    44,    45,    46,
      47,    48,     2,     3,     4,     5,     0,     0,     6,     7,
       8,     9,    10,     0,     0,     0,    13,     0,    15,     0,
      16,    17,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    20,     0,    22,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    35,    36,   100,     0,    38,
       0,     0,     0,    40,     0,   117,     0,     0,     0,     0,
      42,   424,     0,     0,    43,    44,    45,    46,    47,    48,
       2,     3,     4,     5,     0,     0,     6,     7,     8,     9,
      10,     0,     0,     0,    13,     0,    15,     0,    16,    17,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    20,     0,    22,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    34,    35,    36,   100,     0,    38,     0,     0,
       0,    40,     0,   117,     0,     0,     0,     0,    42,   519,
       0,     0,    43,    44,    45,    46,    47,    48,     2,     3,
       4,     5,     0,     0,     6,     7,     8,     9,    10,     0,
       0,     0,    13,     0,    15,     0,    16,    17,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    20,     0,    22,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      34,    35,    36,   100,     0,    38,     0,     0,     0,    40,
       0,   117,     0,     0,     0,     0,    42,   590,     0,     0,
      43,    44,    45,    46,    47,    48,     2,     3,     4,     5,
       0,     0,     6,     7,     8,     9,    10,     0,     0,     0,
      13,     0,    15,     0,    16,    17,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    20,
       0,    22,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    35,
      36,   100,     0,    38,     0,     0,     0,    40,     0,   117,
       0,     0,     0,     0,    42,   632,     0,     0,    43,    44,
      45,    46,    47,    48,     2,     3,     4,     5,     0,     0,
       6,     7,     8,     9,    10,     0,     0,     0,    13,     0,
      15,     0,    16,    17,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    20,     0,    22,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    34,    35,    36,   100,
       0,    38,     0,     0,     0,    40,     0,   117,     0,     0,
       0,     0,    42,   664,     0,     0,    43,    44,    45,    46,
      47,    48,     2,     3,     4,     5,     0,     0,     6,     7,
       8,     9,    10,     0,     0,     0,    13,     0,    15,     0,
      16,    17,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    20,     0,    22,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    35,    36,   100,     0,    38,
       0,     0,     0,    40,     0,   117,     0,     0,     0,     0,
      42,   677,     0,     0,    43,    44,    45,    46,    47,    48,
       2,     3,     4,     5,     0,     0,     6,     7,     8,     9,
      10,     0,     0,     0,    13,     0,    15,     0,    16,    17,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    20,     0,    22,     2,     3,     4,     5,
       0,     0,     6,     7,     8,     9,    10,     0,     0,     0,
      13,     0,    15,     0,    16,    17,     0,     0,     0,     0,
       0,     0,    34,    35,    36,   100,     0,    38,     0,     0,
       0,    40,     0,   117,     0,     0,     0,     0,    42,    20,
       0,    22,    43,    44,    45,    46,    47,    48,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    35,
      36,   100,     0,    38,     0,     0,     0,   101,     0,     0,
       0,     0,     0,     0,    42,     0,     0,     0,    43,    44,
      45,    46,    47,    48,     2,     3,     4,     5,     0,     0,
       6,     7,     8,     9,    10,     0,     0,     0,    13,     0,
      15,     0,    16,    17,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    20,     0,    22,
       2,     3,     4,     5,     0,     0,     6,     7,     8,     9,
      10,     0,     0,     0,    13,     0,    15,     0,    16,    17,
       0,     0,     0,     0,     0,     0,    34,    35,    36,   100,
       0,    38,     0,     0,     0,   106,     0,     0,     0,     0,
       0,     0,    42,    20,     0,    22,    43,    44,    45,    46,
      47,    48,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    34,    35,    36,   100,     0,    38,     0,     0,
       0,   111,     0,     0,     0,     0,     0,     0,    42,     0,
       0,     0,    43,    44,    45,    46,    47,    48
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
       0,     0,     0,     0,     0,    21,     0,     0,    23,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     1,     3,     0,     0,     5,     0,
       0,     7,     0,     0,     0,     0,     9,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    35,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    11,    13,     0,
       0,    15,     0,     0,    17,     0,     0,     0,     0,    19,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    25,    27,     0,     0,    29,     0,     0,    31,     0,
       0,     0,     0,    33,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0
};

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short int yyconfl[] =
{
       0,   277,     0,   277,     0,   277,     0,   277,     0,   277,
       0,   281,     0,   281,     0,   281,     0,   281,     0,   281,
       0,    90,     0,    90,     0,   276,     0,   276,     0,   276,
       0,   276,     0,   276,     0,   344,     0
};

static const short int yycheck[] =
{
       8,    40,    10,   178,   477,     3,     0,   392,    16,    17,
     248,    30,   299,    19,   178,     3,   430,     7,     8,    13,
      22,     3,   461,   173,    18,    19,     3,   108,    35,    36,
      63,    63,    84,    27,    63,     3,    63,   194,     3,   196,
      85,    35,    36,     0,   201,    39,    40,    41,     3,     3,
      85,    53,     3,    65,     3,    86,    88,    65,    29,    88,
      92,    88,   101,    92,   109,    88,   105,   106,    84,    92,
      27,    88,   108,     3,   109,     3,   109,    93,    96,    84,
      78,   384,     9,   101,   102,   373,    84,    84,   107,     3,
      78,   108,     3,    84,     3,   534,    40,   511,    96,    87,
      94,    78,    84,    97,    86,    40,    96,   101,    85,   368,
     108,   105,   106,    78,    84,   109,    84,   355,    89,    84,
      85,     3,    84,    78,    78,   119,    91,    78,    40,    78,
      84,    96,   296,   140,   298,    84,    87,    91,   132,   133,
      97,   555,   136,   431,   138,    88,   140,    96,    78,    86,
      78,    88,   146,   147,   148,    85,    84,   101,   461,    39,
     545,   105,   106,    91,    78,   108,   101,    78,   471,    78,
     105,   106,    86,    84,   180,   132,   649,   184,    87,   136,
     174,   138,   176,    89,    90,    96,   180,   601,    84,   101,
     184,   185,    84,   105,   106,    91,    78,   108,   192,   193,
     194,   507,    84,   197,   198,   464,   218,   219,   220,   203,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   185,   379,
     387,   534,   176,   108,   537,   192,    87,    88,    88,    84,
     175,   247,    92,    89,   179,    88,    91,   249,   554,   243,
     194,    96,   540,   247,   248,    25,    26,   108,   252,   194,
      88,   255,     3,   427,   176,   108,   146,   147,   148,   419,
     264,   558,    85,    87,    88,    88,   535,    84,   584,   273,
     108,   275,   194,   295,    88,    87,    88,   295,   576,   297,
     284,   285,   304,   287,   108,   289,   304,   585,    88,   243,
      84,    88,    92,    88,   108,     3,   108,   264,   243,    39,
      84,   105,    86,   307,   602,    88,   273,   605,   624,   578,
     623,   108,   610,   108,   326,    84,   365,   586,    88,    84,
     287,   243,     3,   103,   104,   108,    91,    78,    95,    84,
     515,   644,    84,    84,   106,   289,    91,     3,   108,    91,
     307,   515,   611,    84,    29,   643,   662,   645,    90,   509,
      91,   355,     0,   298,   358,   373,   639,    85,   641,   675,
      88,   365,    84,     3,   368,   681,    86,   371,   637,   667,
      78,   375,    85,   656,   108,    88,    84,   389,   382,   383,
     384,   679,   556,    91,   388,    67,   390,    88,    96,   225,
     226,   227,   228,    84,   443,   399,     3,    78,   402,     3,
     404,    84,   406,    84,   358,    84,   146,   147,   148,    97,
      98,   365,    78,   431,    85,    96,   420,    88,    84,    85,
     365,   388,    85,   390,    89,    91,   430,   108,     0,    90,
      96,    84,   444,    85,   452,   380,    88,   449,    78,   443,
      85,   453,    14,   365,    84,    92,   458,    85,    90,   623,
      88,    91,    85,   457,   458,    27,    96,   461,    85,    85,
     464,    88,    88,    35,    36,    85,    85,   471,    88,    41,
     644,    78,    37,   477,    88,    40,   430,   422,    88,    85,
      87,    88,    88,    84,     3,   430,     5,     6,    85,   443,
     494,    10,   496,   497,    88,   499,    84,    85,   443,    23,
      24,    85,   506,    91,    88,   108,   510,   511,    96,   399,
      85,   515,   402,    88,   404,   527,   406,   529,    27,    28,
      89,   443,   540,    85,    85,    97,    88,   531,    92,   533,
     534,   535,   499,   537,    90,   100,   101,     7,     8,   543,
     105,   106,    92,    85,   284,   285,    88,   119,   297,    88,
      89,   555,   556,    85,   558,    85,    75,   511,   576,    78,
     132,    80,   507,   108,   136,    84,   138,   585,   140,    81,
      89,   516,    91,    92,   578,    38,   543,   581,    85,    42,
       7,     8,   586,    88,   602,    87,    88,   605,    88,    89,
      85,   595,   610,    87,    88,    88,    89,   601,    87,    88,
      92,   555,   167,   607,   494,    88,   496,   611,    85,   554,
     555,   176,   184,   185,    84,    85,   284,   285,    88,   623,
     192,    91,    96,   627,   373,   643,    96,   645,    92,   194,
       7,     8,    85,   637,    88,   639,    86,   641,    86,   584,
     644,     3,    86,     5,     6,   649,    65,   601,    10,   667,
     221,   222,   656,   223,   224,    81,   601,    21,   121,   399,
     627,   679,   402,   108,   404,    85,   406,    31,    32,   229,
     230,    35,    36,    88,    92,    85,    40,    41,   243,   624,
     108,    93,   431,    85,    87,    93,   108,    87,    89,    85,
      88,    85,   264,    92,    85,     3,    89,     5,     6,    87,
      85,   273,    10,   452,   108,   595,    92,    84,    85,    85,
      92,    88,    85,    75,    91,   287,    78,   662,    80,    96,
      89,    92,    84,    85,   108,   382,   406,   310,   231,    91,
     675,   194,   233,   196,   232,   307,   681,   101,   201,   235,
     375,   578,   106,   234,     7,     8,   110,   111,   578,    20,
      21,    22,   215,   117,   494,   383,   496,   639,   122,   123,
     124,   607,   126,   644,   555,   649,   666,    75,   625,   176,
      78,   556,    80,   601,   237,   139,    84,   558,   511,   184,
     243,    97,    -1,    91,    92,    -1,    -1,    -1,    -1,    -1,
      -1,   540,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
     365,    -1,     9,    10,    11,    12,    13,    -1,    -1,    -1,
      17,   274,    19,    84,    21,    22,   388,    -1,   390,    -1,
      91,    84,    85,    94,    -1,    88,    -1,   576,    91,    -1,
      -1,    -1,    -1,    96,    -1,    -1,   585,    -1,   202,    46,
      47,    48,    49,    -1,    -1,    -1,    -1,   310,    -1,    -1,
     313,    -1,   315,   602,    -1,   595,   605,    -1,     3,    -1,
      -1,   610,     7,     8,    -1,    -1,    -1,    -1,    75,    76,
      77,    78,   236,    80,    -1,    -1,    -1,    84,   443,    86,
      -1,    -1,    -1,    -1,    91,    -1,    -1,    -1,    95,    96,
      97,    98,    99,   100,   643,    -1,   645,     3,    -1,     5,
       6,   108,   365,    -1,    10,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,   667,    -1,
      -1,    -1,   286,    -1,   387,    -1,    -1,   499,    -1,    -1,
     679,    -1,    -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,
       0,     1,    -1,     3,     4,     5,     6,    -1,    -1,     9,
      10,    11,    12,    13,    -1,    15,    16,    17,    18,    19,
      -1,    21,    22,   108,    -1,    -1,    -1,    -1,    -1,    75,
      -1,   543,    78,    -1,    80,    -1,    -1,    -1,    84,    85,
     443,    -1,   445,    43,    44,    91,    46,    47,    48,    49,
      -1,    -1,     3,    -1,     5,     6,    -1,    -1,    -1,    10,
      -1,    -1,    -1,    -1,    64,    -1,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    -1,
      80,    -1,    -1,    83,    84,    -1,    86,   391,   392,    -1,
      -1,    91,    -1,    -1,    -1,    95,    96,    97,    98,    99,
     100,     0,     1,    -1,     3,     4,     5,     6,   108,    -1,
       9,    10,    11,    12,    13,   627,    15,    16,    17,    18,
      19,    -1,    21,    22,    75,    -1,    -1,    78,    -1,    80,
      -1,    -1,    -1,    84,    -1,    -1,    -1,    -1,    -1,    -1,
      91,    92,    -1,    -1,    43,    44,    -1,    46,    47,    48,
      49,     3,    -1,     5,     6,    -1,    -1,    -1,    10,    -1,
      -1,    -1,    -1,    -1,    -1,    64,    -1,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      -1,    80,    -1,    -1,    83,    84,    -1,    86,    -1,    -1,
      -1,    -1,    91,    -1,    -1,    -1,    95,    96,    97,    98,
      99,   100,    -1,    -1,     1,    -1,     3,    -1,    -1,   108,
       7,     8,    -1,    -1,    -1,    -1,    -1,    14,     1,    -1,
       3,    -1,    -1,    75,     7,     8,    78,    -1,    80,    -1,
      -1,    14,    84,    -1,    -1,    -1,    -1,    -1,    -1,    91,
      -1,   545,    -1,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    -1,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
       1,    78,     3,    -1,    -1,    -1,     7,     8,    -1,    -1,
      87,    -1,    -1,    -1,     1,    78,     3,    -1,    -1,    -1,
       7,     8,    -1,    -1,    87,    93,    -1,    -1,    -1,    -1,
      -1,    -1,   109,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    -1,    -1,    -1,   109,    -1,    -1,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    -1,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    -1,    78,    -1,    -1,
      -1,    -1,    -1,    -1,     3,    -1,    87,    -1,     7,     8,
      -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,     1,
      -1,     3,     4,     5,     6,    -1,    -1,     9,    10,    11,
      12,    13,   109,    15,    16,    17,    18,    19,    -1,    21,
      22,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    44,    45,    46,    47,    48,    49,    -1,    78,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    90,    64,    -1,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,     3,    80,    -1,
      -1,    83,    84,    -1,    86,    87,    -1,    -1,    -1,    91,
      -1,    -1,    -1,    95,    96,    97,    98,    99,   100,     1,
      -1,     3,     4,     5,     6,    -1,   108,     9,    10,    11,
      12,    13,    -1,    15,    16,    17,    18,    19,    -1,    21,
      22,    -1,    -1,    -1,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    -1,    -1,    -1,
      -1,    43,    44,    45,    46,    47,    48,    49,    -1,    -1,
      -1,    -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    64,    -1,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,     3,    80,    -1,
      -1,    83,    84,    -1,    86,    87,    -1,    -1,    -1,    91,
      -1,    -1,    -1,    95,    96,    97,    98,    99,   100,     1,
      -1,     3,     4,     5,     6,    -1,   108,     9,    10,    11,
      12,    13,    -1,    15,    16,    17,    18,    19,    -1,    21,
      22,    -1,    -1,    -1,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    -1,    -1,    -1,
      -1,    43,    44,    -1,    46,    47,    48,    49,    -1,    -1,
      -1,    -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    64,    -1,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,     3,    80,    -1,
      -1,    83,    84,    -1,    86,    87,    -1,    -1,    -1,    91,
      -1,    -1,    -1,    95,    96,    97,    98,    99,   100,     1,
      -1,     3,     4,     5,     6,    -1,   108,     9,    10,    11,
      12,    13,    -1,    15,    16,    17,    18,    19,    -1,    21,
      22,    -1,    -1,    -1,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    44,    -1,    46,    47,    48,    49,    -1,    -1,
      -1,    -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    64,    -1,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    -1,    80,    -1,
      -1,    83,    84,    -1,    86,    87,    -1,    -1,    -1,    91,
      -1,    -1,    -1,    95,    96,    97,    98,    99,   100,     1,
      -1,     3,     4,     5,     6,    -1,   108,     9,    10,    11,
      12,    13,    -1,    15,    16,    17,    18,    19,    -1,    21,
      22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    44,    -1,    46,    47,    48,    49,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    64,    -1,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    -1,    80,    -1,
      -1,    83,    84,    -1,    86,    87,    -1,    -1,    -1,    91,
      -1,    -1,    -1,    95,    96,    97,    98,    99,   100,     1,
      -1,     3,     4,     5,     6,    -1,   108,     9,    10,    11,
      12,    13,    -1,    15,    16,    17,    18,    19,    -1,    21,
      22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    44,    -1,    46,    47,    48,    49,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    64,    -1,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    -1,    80,    -1,
      -1,    83,    84,    -1,    86,    87,    -1,    -1,    -1,    91,
      -1,    -1,    -1,    95,    96,    97,    98,    99,   100,     1,
      -1,     3,     4,     5,     6,    -1,   108,     9,    10,    11,
      12,    13,    -1,    15,    16,    17,    18,    19,    -1,    21,
      22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    44,    -1,    46,    47,    48,    49,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    64,    -1,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    -1,    80,    -1,
      -1,    83,    84,    -1,    86,    87,    -1,    -1,    -1,    91,
      -1,    -1,    -1,    95,    96,    97,    98,    99,   100,     1,
      -1,     3,     4,     5,     6,    -1,   108,     9,    10,    11,
      12,    13,    -1,    15,    16,    17,    18,    19,    -1,    21,
      22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    44,    -1,    46,    47,    48,    49,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    64,    -1,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,     3,    80,    -1,
      -1,    83,    84,    -1,    86,    -1,    -1,    -1,    -1,    91,
      -1,    -1,    -1,    95,    96,    97,    98,    99,   100,    -1,
      -1,     3,     4,     5,     6,    -1,   108,     9,    10,    11,
      12,    13,    -1,    -1,    -1,    17,    -1,    19,    -1,    21,
      22,    -1,    -1,    -1,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    -1,    48,    -1,    -1,    -1,
      -1,    -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    90,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    75,    76,    77,    78,    -1,    80,    -1,
      -1,    -1,    84,    -1,    86,    -1,    -1,    -1,    -1,    91,
      -1,    -1,    -1,    95,    96,    97,    98,    99,   100,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    -1,
      -1,    -1,    17,    -1,    19,    -1,    21,    22,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      75,    76,    77,    78,    -1,    80,    -1,    -1,    -1,    84,
      85,    86,    -1,    -1,    -1,    90,    91,    -1,    -1,    -1,
      95,    96,    97,    98,    99,   100,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    -1,    -1,    -1,
      17,    -1,    19,    -1,    21,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    75,    76,
      77,    78,    -1,    80,    -1,    -1,     3,    84,    -1,    86,
       7,     8,    -1,    90,    91,    -1,    -1,    -1,    95,    96,
      97,    98,    99,   100,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    12,    13,    -1,    -1,    -1,    17,    -1,
      19,    -1,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    -1,    46,    47,    48,
      49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    75,    76,    77,    78,
      -1,    80,    -1,    -1,     3,    84,    -1,    86,     7,     8,
      89,    -1,    91,    92,    -1,    -1,    95,    96,    97,    98,
      99,   100,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    17,    -1,    19,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    -1,    46,    47,    48,    49,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,
      -1,    -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    76,    77,    78,    -1,    80,
      -1,    -1,     3,    84,    85,    86,     7,     8,    -1,    -1,
      91,    -1,    -1,    -1,    95,    96,    97,    98,    99,   100,
       3,     4,     5,     6,    -1,    -1,     9,    10,    11,    12,
      13,    -1,    -1,    -1,    17,    -1,    19,    -1,    21,    22,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    46,    47,    48,    49,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    75,    76,    77,    78,    -1,    80,    -1,    -1,
       3,    84,    85,    86,     7,     8,    -1,    -1,    91,    -1,
      -1,    -1,    95,    96,    97,    98,    99,   100,     3,     4,
       5,     6,    -1,    -1,     9,    10,    11,    12,    13,    -1,
      -1,    -1,    17,    -1,    19,    -1,    21,    22,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      -1,    46,    47,    48,    49,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    78,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      75,    76,    77,    78,    -1,    80,    -1,    -1,    -1,    84,
      -1,    86,    -1,    -1,    -1,    -1,    91,    92,    -1,    -1,
      95,    96,    97,    98,    99,   100,     3,     4,     5,     6,
      -1,    -1,     9,    10,    11,    12,    13,    -1,    -1,    -1,
      17,    -1,    19,    -1,    21,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      47,    48,    49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    75,    76,
      77,    78,    -1,    80,    -1,    -1,    -1,    84,    -1,    86,
      -1,    -1,    -1,    -1,    91,    92,    -1,    -1,    95,    96,
      97,    98,    99,   100,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    12,    13,    -1,    -1,    -1,    17,    -1,
      19,    -1,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    47,    48,
      49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    75,    76,    77,    78,
      -1,    80,    -1,    -1,    -1,    84,    -1,    86,    -1,    -1,
      -1,    -1,    91,    -1,    -1,    -1,    95,    96,    97,    98,
      99,   100,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    17,    -1,    19,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    -1,    48,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    76,    77,    78,    -1,    80,
      -1,    -1,    -1,    84,    -1,    86,    -1,    -1,    -1,    -1,
      91,    92,    -1,    -1,    95,    96,    97,    98,    99,   100,
       3,     4,     5,     6,    -1,    -1,     9,    10,    11,    12,
      13,    -1,    -1,    -1,    17,    -1,    19,    -1,    21,    22,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    -1,    48,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    75,    76,    77,    78,    -1,    80,    -1,    -1,
      -1,    84,    -1,    86,    -1,    -1,    -1,    -1,    91,    92,
      -1,    -1,    95,    96,    97,    98,    99,   100,     3,     4,
       5,     6,    -1,    -1,     9,    10,    11,    12,    13,    -1,
      -1,    -1,    17,    -1,    19,    -1,    21,    22,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    46,    -1,    48,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      75,    76,    77,    78,    -1,    80,    -1,    -1,    -1,    84,
      -1,    86,    -1,    -1,    -1,    -1,    91,    92,    -1,    -1,
      95,    96,    97,    98,    99,   100,     3,     4,     5,     6,
      -1,    -1,     9,    10,    11,    12,    13,    -1,    -1,    -1,
      17,    -1,    19,    -1,    21,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    75,    76,
      77,    78,    -1,    80,    -1,    -1,    -1,    84,    -1,    86,
      -1,    -1,    -1,    -1,    91,    92,    -1,    -1,    95,    96,
      97,    98,    99,   100,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    12,    13,    -1,    -1,    -1,    17,    -1,
      19,    -1,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    48,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    75,    76,    77,    78,
      -1,    80,    -1,    -1,    -1,    84,    -1,    86,    -1,    -1,
      -1,    -1,    91,    92,    -1,    -1,    95,    96,    97,    98,
      99,   100,     3,     4,     5,     6,    -1,    -1,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    17,    -1,    19,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    -1,    48,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    76,    77,    78,    -1,    80,
      -1,    -1,    -1,    84,    -1,    86,    -1,    -1,    -1,    -1,
      91,    92,    -1,    -1,    95,    96,    97,    98,    99,   100,
       3,     4,     5,     6,    -1,    -1,     9,    10,    11,    12,
      13,    -1,    -1,    -1,    17,    -1,    19,    -1,    21,    22,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    -1,    48,     3,     4,     5,     6,
      -1,    -1,     9,    10,    11,    12,    13,    -1,    -1,    -1,
      17,    -1,    19,    -1,    21,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    75,    76,    77,    78,    -1,    80,    -1,    -1,
      -1,    84,    -1,    86,    -1,    -1,    -1,    -1,    91,    46,
      -1,    48,    95,    96,    97,    98,    99,   100,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    75,    76,
      77,    78,    -1,    80,    -1,    -1,    -1,    84,    -1,    -1,
      -1,    -1,    -1,    -1,    91,    -1,    -1,    -1,    95,    96,
      97,    98,    99,   100,     3,     4,     5,     6,    -1,    -1,
       9,    10,    11,    12,    13,    -1,    -1,    -1,    17,    -1,
      19,    -1,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    48,
       3,     4,     5,     6,    -1,    -1,     9,    10,    11,    12,
      13,    -1,    -1,    -1,    17,    -1,    19,    -1,    21,    22,
      -1,    -1,    -1,    -1,    -1,    -1,    75,    76,    77,    78,
      -1,    80,    -1,    -1,    -1,    84,    -1,    -1,    -1,    -1,
      -1,    -1,    91,    46,    -1,    48,    95,    96,    97,    98,
      99,   100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    75,    76,    77,    78,    -1,    80,    -1,    -1,
      -1,    84,    -1,    -1,    -1,    -1,    -1,    -1,    91,    -1,
      -1,    -1,    95,    96,    97,    98,    99,   100
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     1,     3,     4,     5,     6,     9,    10,    11,    12,
      13,    15,    16,    17,    18,    19,    21,    22,    43,    44,
      46,    47,    48,    49,    64,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    80,    83,
      84,    86,    91,    95,    96,    97,    98,    99,   100,   108,
     111,   112,   113,   114,   115,   117,   118,   119,   120,   121,
     122,   124,   127,   134,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   156,
     157,   158,   159,   205,   206,   207,   208,   211,   213,   214,
     215,   217,   218,   219,   220,   221,   222,   223,   224,   108,
      78,    84,   113,   114,   136,    84,    84,   136,   113,   114,
      84,    84,   136,   136,   113,   113,   152,    86,   159,    86,
     211,    84,    84,    84,    84,   205,    84,   108,   108,   108,
     159,   159,    89,    84,   159,   205,   212,   159,   212,    84,
      86,   111,   158,     5,     6,    75,    80,    84,    91,   113,
     115,   128,     7,     8,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    90,   111,   112,
     113,   159,   165,   166,   170,   174,   175,   191,   192,   193,
      45,    87,   159,   209,   210,   212,    89,    92,   125,   126,
     135,   158,    89,    90,    84,   211,    84,    84,    20,    21,
      22,    84,    91,    94,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    93,   151,   136,   138,    96,   101,
     102,    97,    98,    23,    24,    25,    26,   103,   104,    27,
      28,    95,   105,   106,    29,    30,   107,    88,   108,   113,
       0,   224,   191,    84,   113,   191,   191,    86,    84,   113,
     159,   108,    88,   108,   159,    86,   158,   159,   159,   159,
      67,   159,   216,   108,    89,   205,   113,    87,    87,   205,
      87,    87,   159,   212,    88,    84,   128,   128,    89,    92,
     128,   130,   132,   133,    84,    84,    29,    89,   111,    90,
      85,   165,   113,   175,   193,    85,    84,    91,    96,   187,
     201,   202,   175,   152,    87,    87,   209,   212,    87,    92,
      88,    92,    63,    88,    92,    89,   205,   113,    85,   123,
     135,   158,   191,    85,   135,   158,    85,   152,   153,   113,
      85,   135,   159,   113,   158,   138,   138,   138,   139,   139,
     140,   140,   141,   141,   141,   141,   142,   142,   143,   144,
     145,   146,   147,   159,   158,    84,    85,   158,    90,    85,
      85,    87,   152,   153,   211,    88,   113,     1,    14,    60,
      61,    62,    87,   109,   113,   154,   155,   161,   163,   166,
     167,   168,   169,   175,   177,   180,    85,    88,    85,    85,
      85,    84,   108,   205,    85,    85,    87,   158,   113,    88,
      85,    92,    89,    63,    88,    92,    88,    92,    85,   113,
     128,   129,   130,   131,    85,   130,   159,   205,   113,   166,
      90,   138,    85,   201,    92,   149,   160,   175,   187,   202,
      84,    91,   108,   138,    87,    92,   125,   158,    92,    92,
     158,   158,    85,    88,    85,    88,    85,   211,    63,    85,
      85,    92,    89,    85,   153,   113,    87,    88,    85,   191,
     108,   177,   113,   116,   160,    87,   155,   175,   113,   116,
     167,    84,   108,   113,   164,   183,   184,   187,   135,   205,
     211,   205,   159,   216,    81,    85,   128,   128,    92,    92,
     128,    92,   128,   133,    93,    85,    88,    88,    85,    89,
     113,   175,   176,    85,    92,   187,    62,    85,   113,   166,
     169,   175,   194,   195,   196,   197,   198,   199,   200,    92,
     160,   158,   191,   211,   211,   149,   211,    85,    87,   113,
     211,    88,   164,    86,   177,    86,   183,    88,   108,    84,
      91,   184,    85,    65,    85,   108,    81,   128,   113,   205,
     113,   176,   113,   198,    85,    88,    84,   113,   187,   201,
     203,   204,   175,    92,   211,   211,   113,   108,    87,    88,
     113,   181,   182,   164,     1,    87,   109,   113,   171,   172,
     173,   177,    85,   183,    85,   109,   162,   188,   189,   190,
      92,   160,   205,   108,   216,    93,   176,    63,   196,   203,
     204,    84,    91,    85,    87,    93,    87,    88,   108,   108,
     109,   160,    89,    87,   172,   173,    87,   108,   178,   179,
     183,   176,   160,   177,    85,    88,    92,    85,   128,    85,
      85,   194,    92,   160,   160,    87,   182,   160,   108,   177,
      87,    88,   108,    89,    84,    91,   183,   185,   186,   187,
     176,    63,   190,   205,    85,    92,   177,   108,   178,   179,
     179,   160,    85,   185,    92,   160,    84,    91,   186,   179,
     108,   108,   176,    85,    92,    85,   188,    92,   160,    89,
     176,    85,    92,   160,   176,   108
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
#line 101 "c.y"
    { ((*yyvalp).expr) = doidnsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 3:

/* Line 936 of glr.c  */
#line 106 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxunquote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 4:

/* Line 936 of glr.c  */
#line 108 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxunquote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 7:

/* Line 936 of glr.c  */
#line 118 "c.y"
    { ((*yyvalp).expr) = doidnsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p+1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len-1); ;}
    break;

  case 8:

/* Line 936 of glr.c  */
#line 123 "c.y"
    { ((*yyvalp).expr) = doidnsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p+1, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len-1); ;}
    break;

  case 10:

/* Line 936 of glr.c  */
#line 129 "c.y"
    { ((*yyvalp).expr) = 0 ;}
    break;

  case 12:

/* Line 936 of glr.c  */
#line 138 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elambda, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 13:

/* Line 936 of glr.c  */
#line 140 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elambda, nullelist(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 14:

/* Line 936 of glr.c  */
#line 145 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefrec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 15:

/* Line 936 of glr.c  */
#line 147 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefrec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 16:

/* Line 936 of glr.c  */
#line 149 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefrec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 17:

/* Line 936 of glr.c  */
#line 154 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 18:

/* Line 936 of glr.c  */
#line 159 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxquote, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 19:

/* Line 936 of glr.c  */
#line 161 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxquote, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 20:

/* Line 936 of glr.c  */
#line 163 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxquasi, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 21:

/* Line 936 of glr.c  */
#line 165 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxquasi, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 22:

/* Line 936 of glr.c  */
#line 170 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estx, doid("id"), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 23:

/* Line 936 of glr.c  */
#line 172 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 24:

/* Line 936 of glr.c  */
#line 174 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 25:

/* Line 936 of glr.c  */
#line 176 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 26:

/* Line 936 of glr.c  */
#line 181 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 27:

/* Line 936 of glr.c  */
#line 183 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 28:

/* Line 936 of glr.c  */
#line 185 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 29:

/* Line 936 of glr.c  */
#line 187 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 30:

/* Line 936 of glr.c  */
#line 192 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Emcall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 31:

/* Line 936 of glr.c  */
#line 194 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Emcall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 32:

/* Line 936 of glr.c  */
#line 199 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, Eelist,
				     (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0),
			  0, 0);
	;}
    break;

  case 33:

/* Line 936 of glr.c  */
#line 209 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 34:

/* Line 936 of glr.c  */
#line 211 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 36:

/* Line 936 of glr.c  */
#line 217 "c.y"
    { ((*yyvalp).expr) = dotickesrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 37:

/* Line 936 of glr.c  */
#line 219 "c.y"
    { ((*yyvalp).expr) = dosym((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 38:

/* Line 936 of glr.c  */
#line 221 "c.y"
    { ((*yyvalp).expr) = doconst(ctx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 39:

/* Line 936 of glr.c  */
#line 223 "c.y"
    { ((*yyvalp).expr) = dostr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 40:

/* Line 936 of glr.c  */
#line 225 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 41:

/* Line 936 of glr.c  */
#line 227 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epair, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 42:

/* Line 936 of glr.c  */
#line 229 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, nullelist(), 0, 0, 0); ;}
    break;

  case 43:

/* Line 936 of glr.c  */
#line 231 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 44:

/* Line 936 of glr.c  */
#line 233 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 45:

/* Line 936 of glr.c  */
#line 235 "c.y"
    { Expr *ell = newexprsrc(&ctx->inp->src, Eellipsis, 0, 0, 0, 0);
          ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist,
                          invert(newexprsrc(&ctx->inp->src, Eelist,
                                            ell, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), 0, 0)), 0, 0, 0); ;}
    break;

  case 46:

/* Line 936 of glr.c  */
#line 240 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, nullelist(), 0, 0, 0); ;}
    break;

  case 47:

/* Line 936 of glr.c  */
#line 242 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 48:

/* Line 936 of glr.c  */
#line 244 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 55:

/* Line 936 of glr.c  */
#line 255 "c.y"
    { ((*yyvalp).expr) = doconst(ctx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 56:

/* Line 936 of glr.c  */
#line 257 "c.y"
    { ((*yyvalp).expr) = dostr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).p, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.chars).len); ;}
    break;

  case 57:

/* Line 936 of glr.c  */
#line 259 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 58:

/* Line 936 of glr.c  */
#line 261 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epair, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 59:

/* Line 936 of glr.c  */
#line 263 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, nullelist(), 0, 0, 0); ;}
    break;

  case 60:

/* Line 936 of glr.c  */
#line 265 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 61:

/* Line 936 of glr.c  */
#line 267 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 62:

/* Line 936 of glr.c  */
#line 269 "c.y"
    { Expr *ell = newexprsrc(&ctx->inp->src, Eellipsis, 0, 0, 0, 0);
          ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elist,
                          invert(newexprsrc(&ctx->inp->src, Eelist,
                                            ell, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), 0, 0)), 0, 0, 0); ;}
    break;

  case 63:

/* Line 936 of glr.c  */
#line 274 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, nullelist(), 0, 0, 0); ;}
    break;

  case 64:

/* Line 936 of glr.c  */
#line 276 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 65:

/* Line 936 of glr.c  */
#line 278 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etab, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 66:

/* Line 936 of glr.c  */
#line 280 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 67:

/* Line 936 of glr.c  */
#line 282 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 68:

/* Line 936 of glr.c  */
#line 284 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estx, doid("id"), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 69:

/* Line 936 of glr.c  */
#line 286 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 70:

/* Line 936 of glr.c  */
#line 288 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 71:

/* Line 936 of glr.c  */
#line 290 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 74:

/* Line 936 of glr.c  */
#line 300 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 75:

/* Line 936 of glr.c  */
#line 302 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 76:

/* Line 936 of glr.c  */
#line 307 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
                          newbinopsrc(&ctx->inp->src, Eeq, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)),
                          nullelist(),
                          0, 0); ;}
    break;

  case 77:

/* Line 936 of glr.c  */
#line 312 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
                          newbinopsrc(&ctx->inp->src, Eeq, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr)),
                          (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr),
                          0, 0); ;}
    break;

  case 78:

/* Line 936 of glr.c  */
#line 320 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 79:

/* Line 936 of glr.c  */
#line 322 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 80:

/* Line 936 of glr.c  */
#line 327 "c.y"
    { /* key may not actually bind variables, but this will get
             caught during compilation. */
          ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, Eelist,
				     (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0),
			  0, 0);
	;}
    break;

  case 82:

/* Line 936 of glr.c  */
#line 340 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 83:

/* Line 936 of glr.c  */
#line 342 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 84:

/* Line 936 of glr.c  */
#line 344 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 85:

/* Line 936 of glr.c  */
#line 346 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edot, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 86:

/* Line 936 of glr.c  */
#line 348 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earrow, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 87:

/* Line 936 of glr.c  */
#line 350 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epostinc, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 88:

/* Line 936 of glr.c  */
#line 352 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epostdec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 89:

/* Line 936 of glr.c  */
#line 354 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Econtainer, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 90:

/* Line 936 of glr.c  */
#line 359 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 91:

/* Line 936 of glr.c  */
#line 361 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 94:

/* Line 936 of glr.c  */
#line 368 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epreinc, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 95:

/* Line 936 of glr.c  */
#line 370 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Epredec, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 96:

/* Line 936 of glr.c  */
#line 372 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 97:

/* Line 936 of glr.c  */
#line 374 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Esizeofe, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 98:

/* Line 936 of glr.c  */
#line 376 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Esizeoft, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 99:

/* Line 936 of glr.c  */
#line 378 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypeofe, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 100:

/* Line 936 of glr.c  */
#line 380 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypeoft, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 101:

/* Line 936 of glr.c  */
#line 382 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Emkctype, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 102:

/* Line 936 of glr.c  */
#line 387 "c.y"
    { ((*yyvalp).kind) = Eref; ;}
    break;

  case 103:

/* Line 936 of glr.c  */
#line 389 "c.y"
    { ((*yyvalp).kind) = Ederef; ;}
    break;

  case 104:

/* Line 936 of glr.c  */
#line 391 "c.y"
    { ((*yyvalp).kind) = Euplus; ;}
    break;

  case 105:

/* Line 936 of glr.c  */
#line 393 "c.y"
    { ((*yyvalp).kind) = Euminus; ;}
    break;

  case 106:

/* Line 936 of glr.c  */
#line 395 "c.y"
    { ((*yyvalp).kind) = Eutwiddle; ;}
    break;

  case 107:

/* Line 936 of glr.c  */
#line 397 "c.y"
    { ((*yyvalp).kind) = Eunot; ;}
    break;

  case 109:

/* Line 936 of glr.c  */
#line 403 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecast, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 110:

/* Line 936 of glr.c  */
#line 405 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Excast, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 112:

/* Line 936 of glr.c  */
#line 411 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Emul, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 113:

/* Line 936 of glr.c  */
#line 413 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ediv, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 114:

/* Line 936 of glr.c  */
#line 415 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Emod, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 116:

/* Line 936 of glr.c  */
#line 421 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eadd, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 117:

/* Line 936 of glr.c  */
#line 423 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Esub, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 119:

/* Line 936 of glr.c  */
#line 429 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eshl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 120:

/* Line 936 of glr.c  */
#line 431 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eshr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 122:

/* Line 936 of glr.c  */
#line 437 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Elt, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 123:

/* Line 936 of glr.c  */
#line 439 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Egt, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 124:

/* Line 936 of glr.c  */
#line 441 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ele, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 125:

/* Line 936 of glr.c  */
#line 443 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ege, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 127:

/* Line 936 of glr.c  */
#line 449 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eeq, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 128:

/* Line 936 of glr.c  */
#line 451 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eneq, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 130:

/* Line 936 of glr.c  */
#line 457 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Eband, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 132:

/* Line 936 of glr.c  */
#line 463 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ebxor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 134:

/* Line 936 of glr.c  */
#line 469 "c.y"
    { ((*yyvalp).expr) = newbinopsrc(&ctx->inp->src, Ebor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 136:

/* Line 936 of glr.c  */
#line 475 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eland, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 138:

/* Line 936 of glr.c  */
#line 481 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 140:

/* Line 936 of glr.c  */
#line 487 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Econd, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 142:

/* Line 936 of glr.c  */
#line 493 "c.y"
    { if((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.kind) == Eg)
	  	((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0);
	  else
	  	((*yyvalp).expr) = newgopsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr));
	;}
    break;

  case 143:

/* Line 936 of glr.c  */
#line 502 "c.y"
    { ((*yyvalp).kind) = Eg; ;}
    break;

  case 144:

/* Line 936 of glr.c  */
#line 504 "c.y"
    { ((*yyvalp).kind) = Egadd; ;}
    break;

  case 145:

/* Line 936 of glr.c  */
#line 506 "c.y"
    { ((*yyvalp).kind) = Egband; ;}
    break;

  case 146:

/* Line 936 of glr.c  */
#line 508 "c.y"
    { ((*yyvalp).kind) = Egbor; ;}
    break;

  case 147:

/* Line 936 of glr.c  */
#line 510 "c.y"
    { ((*yyvalp).kind) = Egbxor; ;}
    break;

  case 148:

/* Line 936 of glr.c  */
#line 512 "c.y"
    { ((*yyvalp).kind) = Egdiv; ;}
    break;

  case 149:

/* Line 936 of glr.c  */
#line 514 "c.y"
    { ((*yyvalp).kind) = Egmod; ;}
    break;

  case 150:

/* Line 936 of glr.c  */
#line 516 "c.y"
    { ((*yyvalp).kind) = Egmul; ;}
    break;

  case 151:

/* Line 936 of glr.c  */
#line 518 "c.y"
    { ((*yyvalp).kind) = Egshl; ;}
    break;

  case 152:

/* Line 936 of glr.c  */
#line 520 "c.y"
    { ((*yyvalp).kind) = Egshr; ;}
    break;

  case 153:

/* Line 936 of glr.c  */
#line 522 "c.y"
    { ((*yyvalp).kind) = Egsub; ;}
    break;

  case 154:

/* Line 936 of glr.c  */
#line 527 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 155:

/* Line 936 of glr.c  */
#line 529 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 156:

/* Line 936 of glr.c  */
#line 534 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 157:

/* Line 936 of glr.c  */
#line 536 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
			  newexprsrc(&ctx->inp->src, Eellipsis, 0, 0, 0, 0),
			  (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0);
	;}
    break;

  case 158:

/* Line 936 of glr.c  */
#line 544 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 159:

/* Line 936 of glr.c  */
#line 546 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 162:

/* Line 936 of glr.c  */
#line 553 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 164:

/* Line 936 of glr.c  */
#line 559 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Enames, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 165:

/* Line 936 of glr.c  */
#line 561 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Enames, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 166:

/* Line 936 of glr.c  */
#line 566 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elapply, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 167:

/* Line 936 of glr.c  */
#line 568 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elapply, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 171:

/* Line 936 of glr.c  */
#line 579 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecomma, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 173:

/* Line 936 of glr.c  */
#line 588 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 174:

/* Line 936 of glr.c  */
#line 593 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 175:

/* Line 936 of glr.c  */
#line 595 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 176:

/* Line 936 of glr.c  */
#line 600 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecls, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 177:

/* Line 936 of glr.c  */
#line 602 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecls, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 178:

/* Line 936 of glr.c  */
#line 604 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecls, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 179:

/* Line 936 of glr.c  */
#line 609 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 180:

/* Line 936 of glr.c  */
#line 611 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 181:

/* Line 936 of glr.c  */
#line 616 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Evoid, 0, 0, 0, 0); ;}
    break;

  case 182:

/* Line 936 of glr.c  */
#line 618 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebool, 0, 0, 0, 0); ;}
    break;

  case 183:

/* Line 936 of glr.c  */
#line 620 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Echar, 0, 0, 0, 0); ;}
    break;

  case 184:

/* Line 936 of glr.c  */
#line 622 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eshort, 0, 0, 0, 0); ;}
    break;

  case 185:

/* Line 936 of glr.c  */
#line 624 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eint, 0, 0, 0, 0); ;}
    break;

  case 186:

/* Line 936 of glr.c  */
#line 626 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Elong, 0, 0, 0, 0); ;}
    break;

  case 187:

/* Line 936 of glr.c  */
#line 628 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efloat, 0, 0, 0, 0); ;}
    break;

  case 188:

/* Line 936 of glr.c  */
#line 630 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edouble, 0, 0, 0, 0); ;}
    break;

  case 189:

/* Line 936 of glr.c  */
#line 632 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Esigned, 0, 0, 0, 0); ;}
    break;

  case 190:

/* Line 936 of glr.c  */
#line 634 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eunsigned, 0, 0, 0, 0); ;}
    break;

  case 191:

/* Line 936 of glr.c  */
#line 638 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 192:

/* Line 936 of glr.c  */
#line 640 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 193:

/* Line 936 of glr.c  */
#line 645 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 194:

/* Line 936 of glr.c  */
#line 647 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 197:

/* Line 936 of glr.c  */
#line 654 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (6))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 198:

/* Line 936 of glr.c  */
#line 656 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 199:

/* Line 936 of glr.c  */
#line 658 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), Znull(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 200:

/* Line 936 of glr.c  */
#line 660 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 201:

/* Line 936 of glr.c  */
#line 662 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 202:

/* Line 936 of glr.c  */
#line 667 "c.y"
    { ((*yyvalp).kind) = Estruct; ;}
    break;

  case 203:

/* Line 936 of glr.c  */
#line 669 "c.y"
    { ((*yyvalp).kind) = Eunion; ;}
    break;

  case 204:

/* Line 936 of glr.c  */
#line 674 "c.y"
    { ((*yyvalp).kind) = Estruct; ;}
    break;

  case 205:

/* Line 936 of glr.c  */
#line 676 "c.y"
    { ((*yyvalp).kind) = Eunion; ;}
    break;

  case 206:

/* Line 936 of glr.c  */
#line 678 "c.y"
    { ((*yyvalp).kind) = Eenum; ;}
    break;

  case 207:

/* Line 936 of glr.c  */
#line 683 "c.y"
    {
		/* labels yield null struct_declarations */
		if((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr))
			((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist,
					(((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0);
		else
			((*yyvalp).expr) = Znull();
	;}
    break;

  case 208:

/* Line 936 of glr.c  */
#line 692 "c.y"
    {
		/* labels yield null struct_declarations */
	 	if((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr))
			((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0);
		else
			((*yyvalp).expr) = Znull();
	;}
    break;

  case 209:

/* Line 936 of glr.c  */
#line 703 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efields, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 210:

/* Line 936 of glr.c  */
#line 705 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efields, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), Znull(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 211:

/* Line 936 of glr.c  */
#line 707 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efields, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 212:

/* Line 936 of glr.c  */
#line 709 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efields, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 213:

/* Line 936 of glr.c  */
#line 711 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebitfield, (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (8))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (8))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 214:

/* Line 936 of glr.c  */
#line 713 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebitfield, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 215:

/* Line 936 of glr.c  */
#line 716 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 216:

/* Line 936 of glr.c  */
#line 718 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 217:

/* Line 936 of glr.c  */
#line 723 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 225:

/* Line 936 of glr.c  */
#line 745 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 226:

/* Line 936 of glr.c  */
#line 747 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 227:

/* Line 936 of glr.c  */
#line 752 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 228:

/* Line 936 of glr.c  */
#line 754 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 230:

/* Line 936 of glr.c  */
#line 763 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (5))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 231:

/* Line 936 of glr.c  */
#line 765 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), 0, 0); ;}
    break;

  case 232:

/* Line 936 of glr.c  */
#line 767 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 233:

/* Line 936 of glr.c  */
#line 769 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 234:

/* Line 936 of glr.c  */
#line 771 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 235:

/* Line 936 of glr.c  */
#line 776 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 236:

/* Line 936 of glr.c  */
#line 778 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 237:

/* Line 936 of glr.c  */
#line 785 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenumel, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 238:

/* Line 936 of glr.c  */
#line 787 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenumel, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 239:

/* Line 936 of glr.c  */
#line 792 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 241:

/* Line 936 of glr.c  */
#line 798 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 242:

/* Line 936 of glr.c  */
#line 800 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 243:

/* Line 936 of glr.c  */
#line 802 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 244:

/* Line 936 of glr.c  */
#line 804 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 245:

/* Line 936 of glr.c  */
#line 806 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 246:

/* Line 936 of glr.c  */
#line 808 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 249:

/* Line 936 of glr.c  */
#line 815 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 250:

/* Line 936 of glr.c  */
#line 820 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 251:

/* Line 936 of glr.c  */
#line 822 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, 0, 0, 0); ;}
    break;

  case 252:

/* Line 936 of glr.c  */
#line 824 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 253:

/* Line 936 of glr.c  */
#line 826 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 254:

/* Line 936 of glr.c  */
#line 828 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 255:

/* Line 936 of glr.c  */
#line 830 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, 0, Znull(), 0, 0); ;}
    break;

  case 256:

/* Line 936 of glr.c  */
#line 832 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 257:

/* Line 936 of glr.c  */
#line 834 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 258:

/* Line 936 of glr.c  */
#line 839 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, 0, 0, 0, 0); ;}
    break;

  case 259:

/* Line 936 of glr.c  */
#line 841 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, 0, 0, 0, 0); ;}
    break;

  case 260:

/* Line 936 of glr.c  */
#line 843 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 261:

/* Line 936 of glr.c  */
#line 845 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eptr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 262:

/* Line 936 of glr.c  */
#line 850 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 263:

/* Line 936 of glr.c  */
#line 852 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 264:

/* Line 936 of glr.c  */
#line 857 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 265:

/* Line 936 of glr.c  */
#line 859 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 266:

/* Line 936 of glr.c  */
#line 864 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 267:

/* Line 936 of glr.c  */
#line 866 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 268:

/* Line 936 of glr.c  */
#line 868 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 269:

/* Line 936 of glr.c  */
#line 873 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypename, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 270:

/* Line 936 of glr.c  */
#line 875 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypename, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 272:

/* Line 936 of glr.c  */
#line 881 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 273:

/* Line 936 of glr.c  */
#line 883 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 274:

/* Line 936 of glr.c  */
#line 888 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 275:

/* Line 936 of glr.c  */
#line 890 "c.y"
    { ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0)); ;}
    break;

  case 276:

/* Line 936 of glr.c  */
#line 893 "c.y"
    { ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0)); ;}
    break;

  case 277:

/* Line 936 of glr.c  */
#line 896 "c.y"
    { ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, 0,
			  newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0)); ;}
    break;

  case 278:

/* Line 936 of glr.c  */
#line 899 "c.y"
    { cwarnln(ctx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), "deprecated use of backtick on unqualified type name");
	  ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, 0,
			  newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0)); ;}
    break;

  case 279:

/* Line 936 of glr.c  */
#line 903 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 280:

/* Line 936 of glr.c  */
#line 905 "c.y"
    { ((*yyvalp).expr) = doticktsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr),
			  newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), 0, 0, 0)); ;}
    break;

  case 282:

/* Line 936 of glr.c  */
#line 912 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 283:

/* Line 936 of glr.c  */
#line 914 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 284:

/* Line 936 of glr.c  */
#line 919 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), Znull(), 0, 0); ;}
    break;

  case 285:

/* Line 936 of glr.c  */
#line 921 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 286:

/* Line 936 of glr.c  */
#line 926 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 287:

/* Line 936 of glr.c  */
#line 928 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 288:

/* Line 936 of glr.c  */
#line 930 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edecl, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 290:

/* Line 936 of glr.c  */
#line 936 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 291:

/* Line 936 of glr.c  */
#line 938 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 292:

/* Line 936 of glr.c  */
#line 943 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 293:

/* Line 936 of glr.c  */
#line 945 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Etypedef, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 296:

/* Line 936 of glr.c  */
#line 952 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 297:

/* Line 936 of glr.c  */
#line 957 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eenum, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 300:

/* Line 936 of glr.c  */
#line 964 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 301:

/* Line 936 of glr.c  */
#line 969 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 302:

/* Line 936 of glr.c  */
#line 971 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, 0, 0, 0); ;}
    break;

  case 303:

/* Line 936 of glr.c  */
#line 973 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 304:

/* Line 936 of glr.c  */
#line 975 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 305:

/* Line 936 of glr.c  */
#line 977 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 306:

/* Line 936 of glr.c  */
#line 979 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, 0, nullelist(), 0, 0); ;}
    break;

  case 307:

/* Line 936 of glr.c  */
#line 981 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 308:

/* Line 936 of glr.c  */
#line 983 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 309:

/* Line 936 of glr.c  */
#line 988 "c.y"
    { ((*yyvalp).expr) = ptrto((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 311:

/* Line 936 of glr.c  */
#line 994 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 312:

/* Line 936 of glr.c  */
#line 996 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 313:

/* Line 936 of glr.c  */
#line 998 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 314:

/* Line 936 of glr.c  */
#line 1000 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Earr, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 315:

/* Line 936 of glr.c  */
#line 1002 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 316:

/* Line 936 of glr.c  */
#line 1004 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efun, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 328:

/* Line 936 of glr.c  */
#line 1023 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Emcall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 329:

/* Line 936 of glr.c  */
#line 1025 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Emcall, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 330:

/* Line 936 of glr.c  */
#line 1030 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Estxunquote, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 331:

/* Line 936 of glr.c  */
#line 1035 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eglobal, invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)), 0, 0, 0); ;}
    break;

  case 332:

/* Line 936 of glr.c  */
#line 1040 "c.y"
    { ((*yyvalp).expr) = invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 333:

/* Line 936 of glr.c  */
#line 1045 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 334:

/* Line 936 of glr.c  */
#line 1047 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 335:

/* Line 936 of glr.c  */
#line 1052 "c.y"
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

  case 336:

/* Line 936 of glr.c  */
#line 1062 "c.y"
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

  case 337:

/* Line 936 of glr.c  */
#line 1072 "c.y"
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
	;}
    break;

  case 338:

/* Line 936 of glr.c  */
#line 1084 "c.y"
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
	;}
    break;

  case 339:

/* Line 936 of glr.c  */
#line 1099 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr), nullelist(), 0, 0); ;}
    break;

  case 340:

/* Line 936 of glr.c  */
#line 1101 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eelist, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (2))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 341:

/* Line 936 of glr.c  */
#line 1106 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Enil, 0, 0, 0, 0); ;}
    break;

  case 342:

/* Line 936 of glr.c  */
#line 1108 "c.y"
    { ((*yyvalp).expr) = (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (2))].yystate.yysemantics.yysval.expr); ;}
    break;

  case 343:

/* Line 936 of glr.c  */
#line 1110 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 344:

/* Line 936 of glr.c  */
#line 1115 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eif, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 345:

/* Line 936 of glr.c  */
#line 1117 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eif, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (7))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (7))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (7))].yystate.yysemantics.yysval.expr), 0); ;}
    break;

  case 346:

/* Line 936 of glr.c  */
#line 1120 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eswitch, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 347:

/* Line 936 of glr.c  */
#line 1125 "c.y"
    { ((*yyvalp).expr) = Zlabelsrc((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr)->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (3))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 348:

/* Line 936 of glr.c  */
#line 1127 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ecase, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), NULL, NULL); ;}
    break;

  case 349:

/* Line 936 of glr.c  */
#line 1129 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ematch, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr), NULL, NULL); ;}
    break;

  case 350:

/* Line 936 of glr.c  */
#line 1131 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ematch, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (6))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr), NULL); ;}
    break;

  case 351:

/* Line 936 of glr.c  */
#line 1133 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefault, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (3))].yystate.yysemantics.yysval.expr), NULL, NULL, NULL); ;}
    break;

  case 353:

/* Line 936 of glr.c  */
#line 1139 "c.y"
    { ((*yyvalp).expr) = 0; ;}
    break;

  case 354:

/* Line 936 of glr.c  */
#line 1144 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ewhile, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (5))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 355:

/* Line 936 of glr.c  */
#line 1146 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edo, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (7))].yystate.yysemantics.yysval.expr), 0, 0); ;}
    break;

  case 356:

/* Line 936 of glr.c  */
#line 1148 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Efor, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (9))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (9))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (9))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((9) - (9))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 357:

/* Line 936 of glr.c  */
#line 1153 "c.y"
    { ((*yyvalp).expr) = Zgotosrc(&ctx->inp->src, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 358:

/* Line 936 of glr.c  */
#line 1155 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Econtinue, 0, 0, 0, 0); ;}
    break;

  case 359:

/* Line 936 of glr.c  */
#line 1157 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Ebreak, 0, 0, 0, 0); ;}
    break;

  case 360:

/* Line 936 of glr.c  */
#line 1159 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eret, 0, 0, 0, 0); ;}
    break;

  case 361:

/* Line 936 of glr.c  */
#line 1161 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Eret, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (3))].yystate.yysemantics.yysval.expr), 0, 0, 0); ;}
    break;

  case 362:

/* Line 936 of glr.c  */
#line 1166 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefstx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (7))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (7))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (7))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((7) - (7))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 363:

/* Line 936 of glr.c  */
#line 1168 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefstx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (6))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 364:

/* Line 936 of glr.c  */
#line 1170 "c.y"
    { ((*yyvalp).expr) = newexprsrc(&ctx->inp->src, Edefstx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (4))].yystate.yysemantics.yysval.expr), 0, (((yyGLRStackItem const *)yyvsp)[YYFILL ((3) - (4))].yystate.yysemantics.yysval.expr), (((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (4))].yystate.yysemantics.yysval.expr)); ;}
    break;

  case 365:

/* Line 936 of glr.c  */
#line 1175 "c.y"
    { ((*yyvalp).kind) = Edefine; ;}
    break;

  case 366:

/* Line 936 of glr.c  */
#line 1177 "c.y"
    { ((*yyvalp).kind) = Edefloc; ;}
    break;

  case 367:

/* Line 936 of glr.c  */
#line 1182 "c.y"
    { ((*yyvalp).expr) = putsrc(newexpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (6))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr), invert((((yyGLRStackItem const *)yyvsp)[YYFILL ((4) - (6))].yystate.yysemantics.yysval.expr)), (((yyGLRStackItem const *)yyvsp)[YYFILL ((6) - (6))].yystate.yysemantics.yysval.expr), 0), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (6))].yystate.yysemantics.yysval.expr)->src); ;}
    break;

  case 368:

/* Line 936 of glr.c  */
#line 1184 "c.y"
    { ((*yyvalp).expr) = putsrc(newexpr((((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (5))].yystate.yysemantics.yysval.kind), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr), nullelist(), (((yyGLRStackItem const *)yyvsp)[YYFILL ((5) - (5))].yystate.yysemantics.yysval.expr), 0), (((yyGLRStackItem const *)yyvsp)[YYFILL ((2) - (5))].yystate.yysemantics.yysval.expr)->src); ;}
    break;

  case 373:

/* Line 936 of glr.c  */
#line 1198 "c.y"
    { dotop(ctx, (((yyGLRStackItem const *)yyvsp)[YYFILL ((1) - (1))].yystate.yysemantics.yysval.expr)); ;}
    break;



/* Line 936 of glr.c  */
#line 4819 "c.tab.c"
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
        case 1: yy0->expr = tnmerge (*yy0, *yy1); break;
  case 2: yy0->expr = ofmerge (*yy0, *yy1); break;
  case 3: yy0->expr = castmerge (*yy0, *yy1); break;

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
#line 1201 "c.y"

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
		yyerror(0, "unresolved ambiguity");

	if(e1->kind == Esizeofe || e1->kind == Etypeofe)
		duptickid(e1->e1);
	else
		duptickid(e2->e1);
	return putsrc(newexpr(Eambig, e1, e2, NULL, NULL), e1->src);
}

static Expr*
tnmerge(YYSTYPE ye1, YYSTYPE ye2)
{
	Expr *e1, *e2;

	e1 = ye1.expr;
	e2 = ye2.expr;
	if(e1->kind == Eelist && e1->e1->kind == Eid)
		return e1;
	if(e2->kind == Eelist && e2->e1->kind == Eid)
		return e2;
	yyerror(0, "unresolved ambiguity");
	abort();
}

