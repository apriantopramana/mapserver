/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
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
     NUMBER = 258,
     STRING = 259,
     TIME = 260,
     SHAPE = 261,
     OR = 262,
     AND = 263,
     NOT = 264,
     IRE = 265,
     IEQ = 266,
     IN = 267,
     GE = 268,
     LE = 269,
     GT = 270,
     LT = 271,
     NE = 272,
     EQ = 273,
     RE = 274,
     DWITHIN = 275,
     BEYOND = 276,
     CONTAINS = 277,
     WITHIN = 278,
     CROSSES = 279,
     OVERLAPS = 280,
     TOUCHES = 281,
     DISJOINT = 282,
     INTERSECTS = 283,
     ROUND = 284,
     COMMIFY = 285,
     LENGTH = 286,
     AREA = 287,
     TOSTRING = 288,
     GENERALIZE = 289,
     SIMPLIFYPT = 290,
     SIMPLIFY = 291,
     DIFFERENCE = 292,
     YYBUFFER = 293,
     NEG = 294
   };
#endif
/* Tokens.  */
#define NUMBER 258
#define STRING 259
#define TIME 260
#define SHAPE 261
#define OR 262
#define AND 263
#define NOT 264
#define IRE 265
#define IEQ 266
#define IN 267
#define GE 268
#define LE 269
#define GT 270
#define LT 271
#define NE 272
#define EQ 273
#define RE 274
#define DWITHIN 275
#define BEYOND 276
#define CONTAINS 277
#define WITHIN 278
#define CROSSES 279
#define OVERLAPS 280
#define TOUCHES 281
#define DISJOINT 282
#define INTERSECTS 283
#define ROUND 284
#define COMMIFY 285
#define LENGTH 286
#define AREA 287
#define TOSTRING 288
#define GENERALIZE 289
#define SIMPLIFYPT 290
#define SIMPLIFY 291
#define DIFFERENCE 292
#define YYBUFFER 293
#define NEG 294




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 30 "mapparser.y"

  double dblval;
  int intval;  
  char *strval;  
  struct tm tmval;
  shapeObj *shpval;



/* Line 2068 of yacc.c  */
#line 138 "mapparser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




