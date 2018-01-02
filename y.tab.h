/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
     T_Void = 258,
     T_Bool = 259,
     T_Int = 260,
     T_LessEqual = 261,
     T_GreaterEqual = 262,
     T_EQ = 263,
     T_NE = 264,
     T_LeftAngle = 265,
     T_RightAngle = 266,
     T_And = 267,
     T_Or = 268,
     T_Equal = 269,
     T_MulAssign = 270,
     T_DivAssign = 271,
     T_AddAssign = 272,
     T_SubAssign = 273,
     T_While = 274,
     T_For = 275,
     T_If = 276,
     T_Else = 277,
     T_Return = 278,
     T_Break = 279,
     T_Continue = 280,
     T_Inc = 281,
     T_Dec = 282,
     T_Semicolon = 283,
     T_Comma = 284,
     T_Dash = 285,
     T_Plus = 286,
     T_Star = 287,
     T_Slash = 288,
     T_LeftParen = 289,
     T_RightParen = 290,
     T_LeftBracket = 291,
     T_RightBracket = 292,
     T_LeftBrace = 293,
     T_RightBrace = 294,
     T_Public = 295,
     T_Private = 296,
     T_Static = 297,
     T_Class = 298,
     T_Identifier = 299,
     T_IntConstant = 300,
     T_BoolConstant = 301,
     LOWEST = 302,
     LOWER_THAN_ELSE = 303
   };
#endif
/* Tokens.  */
#define T_Void 258
#define T_Bool 259
#define T_Int 260
#define T_LessEqual 261
#define T_GreaterEqual 262
#define T_EQ 263
#define T_NE 264
#define T_LeftAngle 265
#define T_RightAngle 266
#define T_And 267
#define T_Or 268
#define T_Equal 269
#define T_MulAssign 270
#define T_DivAssign 271
#define T_AddAssign 272
#define T_SubAssign 273
#define T_While 274
#define T_For 275
#define T_If 276
#define T_Else 277
#define T_Return 278
#define T_Break 279
#define T_Continue 280
#define T_Inc 281
#define T_Dec 282
#define T_Semicolon 283
#define T_Comma 284
#define T_Dash 285
#define T_Plus 286
#define T_Star 287
#define T_Slash 288
#define T_LeftParen 289
#define T_RightParen 290
#define T_LeftBracket 291
#define T_RightBracket 292
#define T_LeftBrace 293
#define T_RightBrace 294
#define T_Public 295
#define T_Private 296
#define T_Static 297
#define T_Class 298
#define T_Identifier 299
#define T_IntConstant 300
#define T_BoolConstant 301
#define LOWEST 302
#define LOWER_THAN_ELSE 303




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 41 "parser.y"
{
    int integerConstant;
    bool boolConstant;
    char identifier[MaxIdentLen+1]; // +1 for terminating null

    Type *varType;
    Decl *decl;
    VarDecl *varDecl;
    List<Decl*> *declList;

    FnDecl *funcDecl;
    Expr *expr;
    List<VarDecl *> *varDeclList;
    List<Stmt*> *stmtList;
    List<Expr*> *argList;
    Stmt       *stmt;
    Operator *ops;
    Identifier *funcId;
}
/* Line 1529 of yacc.c.  */
#line 165 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYLTYPE yylloc;
