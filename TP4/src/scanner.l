%option yylineno

%{

#include "general.h"
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabla_de_simbolos.h"

int yycolumn = 0;

#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn + 1; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng; // Actualiza la columna con la longitud del token

extern YYLTYPE yylloc;

%}

%x tokenNoReconocido
dataType            long|char|float|double|short|int|struct|enum
controlStructure    goto
otherKeyword        auto|register|typedef|extern|union|signed|volatile|static|_Packed
octal               0[0-7]*
hex                 0[xX][0-9a-fA-F]+
decimal             [1-9][0-9]*
real                [0-9]*\.?[0-9]+([eE][-+]?[0-9]*)?
char_simple         \'[^\\']\'
char_escape         \'\\([abfnrtv\\?\'\"]|0[0-7]*|0[xX][0-9a-fA-F]+)\'
string_literal      \"([^\"\\]|\\.)*\"
identifier          [a-zA-Z_][a-zA-Z0-9_]*
puctuation          [+\-*/%&|^~!=<>?:.,()\[\]{}]

%%
"const"              { yylval.constante = strdup(yytext); return CONST; }
";"                  { return PUNTOCOMA; }
"++"                 { return PLUSPLUS; }
"--"                 { return MINUSMINUS; }
"+="                 { return ADDASSIGN; }
"=="                 { return EQUAL; }
"!="                 { return NOTEQUAL; }
"&&"                 { return AND; }
"||"                 { return OR; }
"<="                 { return LEQ; }
">="                 { return GEQ; }
"*="                 { return MULASSIGN; }
"/="                 { return DIVASSIGN; }
"-="                 { return SUBASSIGN; }
"unsigned"           { yylval.usigned = strdup(yytext); return UNSIGNED; }
"void"               { yylval.type = strdup(yytext); return VOID; }
"default"            { yylloc.tamanio = yyleng; yylval.sentencia = strdup(yytext); return DEFAULT; }
"continue"           { yylloc.tamanio = yyleng; yylval.sentencia = strdup(yytext); return CONTINUE; }
"switch"             { yylloc.tamanio = yyleng; yylval.sentencia = strdup(yytext); return SWITCH; }
"break"              { yylloc.tamanio = yyleng; yylval.sentencia = strdup(yytext); return BREAK; }
"case"               { yylloc.tamanio = yyleng; yylval.sentencia = strdup(yytext); return CASE; }
"if"                 { yylval.sentencia = strdup(yytext); yylloc.tamanio = yyleng; yylloc.tempRow = yylloc.first_line; yylloc.tempColumn = yylloc.first_column; return IF; }
"else"               { yylval.sentencia = strdup(yytext); return ELSE; }
"while"              { yylval.sentencia = strdup(yytext); yylloc.tamanio = yyleng; return WHILE;}
"for"                { yylval.sentencia = strdup(yytext); yylloc.tamanio = yyleng; return FOR; }
"do"                 { yylval.sentencia = strdup(yytext); yylloc.tamanio = yyleng; return DO; }
"return"             { yylval.sentencia = strdup(yytext); yylloc.tamanio = yyleng; return RETURN; }
"sizeof"             { return SIZEOF; }
{dataType}           { yylloc.tamanio = yyleng; yylval.type = strdup(yytext); return TIPO_DATO; }
{controlStructure}   { }
{otherKeyword}       { }
{octal}              { yylval.ctes.cteDecimal = atoi(yytext); return CONSTANTE; }
{hex}                { yylval.ctes.cteDecimal = atoi(yytext); return CONSTANTE; }
{decimal}            { yylval.ctes.cteDecimal = atoi(yytext); return CONSTANTE; }
{real}               { yylval.ctes.cteReal = atof(yytext); return CONSTANTE; }
{char_escape}        { }
{char_simple}        { yylval.ctes.cSimple = strdup(yytext); return CHAR_SIMPLE; }
{string_literal}     { yylval.type = strdup(yytext); return LITERAL_CADENA; }
{identifier}         { yylloc.tamanio = yyleng; yylloc.tempColumn = yylloc.first_column; yylval.id = strdup(yytext); return IDENTIFICADOR; }
{puctuation}         { return *(yytext); }

\n                   { yycolumn = 0; }
[ \t]+               { }
[^ \t\n]             { yyless(0); BEGIN(tokenNoReconocido); }
<<EOF>>              { yylloc.eof = 0; return 0; } 
<tokenNoReconocido>{
    <<EOF>> {
        fprintf(stderr, "\n");
        BEGIN(INITIAL);
    }
    " "|\t|\n {
        yyless(0);
        fprintf(stderr, "\n");
        BEGIN(INITIAL);
    }
    [^ \t\n]+ {
        noReconocidasManager(yytext, yylloc.last_line, yylloc.first_column - 1);
        BEGIN(INITIAL);
    }
}

%%