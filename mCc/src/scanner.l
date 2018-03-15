%option prefix="mCc_parser_"

%option batch
%option bison-bridge
%option noinput
%option nounput
%option noyywrap
%option reentrant

%{
#include "parser.tab.h"

#define YYSTYPE MCC_PARSER_STYPE
%}

int_literal   -?[0-9]+
float_literal -?[0-9]+\.[0-9]+

%%

{int_literal}     { yylval->TK_INT_LITERAL = atol(yytext); return TK_INT_LITERAL; }

{float_literal}   { yylval->TK_FLOAT_LITERAL = atof(yytext); return TK_FLOAT_LITERAL; }

"+"               { return TK_PLUS; }
"-"               { return TK_MINUS; }
"*"               { return TK_ASTER; }
"/"               { return TK_SLASH; }
">"				{ return TK_GREATER; }
"<"				{ return TK_SMALLER; }
">="			{ return TK_GREATER_EQUAL; }
"<="			{ return TK_SMALLER_EQUAL; }

"("               { return TK_LPARENTH; }
")"               { return TK_RPARENTH; }

"/*"            {return TK_BEGINCOM; }
"*/"            {return TK_ENDCOM; }

"bool"          {return TK_BOOL; }
"int"           {return TK_INT; }
"float"         {return TK_FLOAT;}
"string"        {return TK_STRING; }



[ \t\r\n]+        { /* ignore */ }

<<EOF>>           { return TK_END; }

.                 { fprintf(stderr, "invalid character '%c'\n", yytext[0]); }