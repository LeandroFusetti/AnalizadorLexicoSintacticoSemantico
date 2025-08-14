%{

#include "general.h"

#include "parser.tab.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern YYLTYPE yylloc;

int cont=0;
// Declaración de funciones

/*Nodo* insertarOrdenadoIds(Nodo*, Info);
Nodo* buscar(Nodo*, Info);
Nodo* insertarSinRepetirIds(Nodo*, Info);
tNodo* insertarAlFinal(tNodo*,tInfo);
Nodo* insertarAlFinalPunc(Nodo*,Info);
Nodo* insertarSinRepetirPunctuation(Nodo*, Info);*/

%}

%x tokenNoReconocido
dataType            long|char|float|const|double|short|int|struct|enum
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
";"                  { updateColumn(yyleng); return PUNTOCOMA; }
"++"                 { updateColumn(yyleng); return PLUSPLUS; }
"--"                 { updateColumn(yyleng); return MINUSMINUS; }
"+="                 { updateColumn(yyleng); return ADDASSIGN; }
"=="                 { updateColumn(yyleng); return EQUAL; }
"!="                 { updateColumn(yyleng); return NOTEQUAL; }
"&&"                 { updateColumn(yyleng); return AND; }
"||"                 { updateColumn(yyleng); return OR; }
"<="                 { updateColumn(yyleng); return LEQ; }
">="                 { updateColumn(yyleng); return GEQ; }
"*="                 { updateColumn(yyleng); return MULASSIGN; }
"/="                 { updateColumn(yyleng); return DIVASSIGN; }
"-="                 { updateColumn(yyleng); return SUBASSIGN; }
"unsigned"           { updateColumn(yyleng); yylval.usigned=strdup(yytext); return UNSIGNED; }
"void"               { updateColumn(yyleng); yylval.type=strdup(yytext); return VOID; }
"default"            { updateColumn(yyleng); yylloc.tamanio=yyleng; yylval.sentencia=strdup(yytext); return DEFAULT; }
"continue"           { updateColumn(yyleng); yylloc.tamanio=yyleng; yylval.sentencia=strdup(yytext); return CONTINUE; }
"switch"             { updateColumn(yyleng); yylloc.tamanio=yyleng; yylval.sentencia=strdup(yytext); return SWITCH; }
"break"              { updateColumn(yyleng); yylloc.tamanio=yyleng; yylval.sentencia=strdup(yytext); return BREAK; }
"case"               { updateColumn(yyleng); yylloc.tamanio=yyleng; yylval.sentencia=strdup(yytext); return CASE; }
"if"                 { updateColumn(yyleng); yylval.sentencia=strdup(yytext); yylloc.tamanio=yyleng; yylloc.tempRow=yylloc.first_line; yylloc.tempColumn=yylloc.last_column; return IF; }
"else"               { updateColumn(yyleng); yylval.sentencia=strdup(yytext); return ELSE; }
"while"              { updateColumn(yyleng); yylval.sentencia=strdup(yytext); yylloc.tamanio=yyleng; return WHILE;}
"for"                { updateColumn(yyleng); yylval.sentencia=strdup(yytext); yylloc.tamanio=yyleng; return FOR; }
"do"                 { updateColumn(yyleng); yylval.sentencia=strdup(yytext); yylloc.tamanio=yyleng; return DO; }
"return"             { updateColumn(yyleng); yylval.sentencia=strdup(yytext); yylloc.tamanio=yyleng; return RETURN; }
"sizeof"             { updateColumn(yyleng); return SIZEOF; }
{dataType}           { updateColumn(yyleng); yylval.type=strdup(yytext); return TIPO_DATO; }
{controlStructure}   { updateColumn(yyleng); }
{otherKeyword}       { updateColumn(yyleng); }
{octal}              { updateColumn(yyleng); return CONSTANTE; }
{hex}                { updateColumn(yyleng); return CONSTANTE; }
{decimal}            { updateColumn(yyleng); return CONSTANTE; }
{real}               { updateColumn(yyleng); return CONSTANTE; }
{char_escape}        { updateColumn(yyleng); }
{char_simple}        { updateColumn(yyleng); }
{string_literal}     { updateColumn(yyleng); return LITERAL_CADENA; }
{identifier}         { updateColumn(yyleng); yylval.id=strdup(yytext); return IDENTIFICADOR; }
{puctuation}         { updateColumn(yyleng); return *(yytext); }


\n                   { yylloc.first_line = yylloc.last_line += 1; yylloc.last_column=1; }
[ \t]+               { updateColumn(yyleng); }
[^ \t\n]             { yyless(0); BEGIN(tokenNoReconocido); }
<<EOF>>              { yylloc.eof=0; return 0; } 
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
        updateColumn(yyleng);
        //fprintf(stderr, "%s\n", yytext);
        //yyoutManager(yytext);
        noReconocidasManager(yytext, yylloc.last_line, yylloc.last_column-yyleng);
        return NORECONOCIDO;
        BEGIN(INITIAL);
    }
}

%%