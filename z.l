D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			([Ee][+-]?{D}+)
%{
#include <stdnoreturn.h>
#include "z.tab.h"
void match( const char* t );
noreturn void no_match(void);

struct YYLTYPE;
void yyerror( struct YYLTYPE* locp, const char* msg );

#define MATCH( x ) match( #x ); return x; 

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
#pragma clang diagnostic ignored "-Wstrict-prototypes"
#pragma clang diagnostic ignored "-Wimplicit-function-declaration"
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wunneeded-internal-declaration"
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wsign-conversion"
%}
%option noyywrap nodefault yylineno

%%

"alias"                       { MATCH( ALIAS ) }
"fn"                          { MATCH( FN ) }
"if"                          { MATCH( IF ) }
"else"                        { MATCH( ELSE ) }
"typedef"                     { MATCH( TYPEDEF ) }
"struct"                      { MATCH( STRUCT ) }
"union"                       { MATCH( UNION ) }
"const"                       { MATCH( CONST ) }

"+" |
"-" |
"*" |
"/" |
"%" |
"=" |
"," |
":" |
"(" |
")" |
"{" |
"}" { char tmp[2] = { yytext[0], 0 }; match( tmp ); return yytext[0]; }

i([1-9]|[1-9]{D}|[1-9]{D}{D}) { MATCH( INT ) }
u([1-9]|[1-9]{D}|[1-9]{D}{D}) { MATCH( UINT ) }
f(16|32|64)                   { MATCH( FLOAT ) }
[_a-zA-Z][_a-zA-Z0-9]*        { MATCH( IDENTIFIER ) }

0[xX]{H}+		                  { MATCH( CONSTANT ) }
0[0-7]*		                    { MATCH( CONSTANT ) }
[1-9]{D}*		                  { MATCH( CONSTANT ) }
{D}+{E}?		                  { MATCH( CONSTANT ) }
{D}+"."{D}+{E}?	              { MATCH( CONSTANT ) }

[ \x0a]                       {}
.                             { no_match(); }

%%

#include <stdio.h>

#pragma clang diagnostic pop

void match( const char* t ) {
  fprintf( stdout, "Matched '%s' as '%s'\n", yytext, t );
}

noreturn void no_match(void) {
  fprintf( stderr, "ERROR: No matching token for '%c'(0x%02x)\n", yytext[0], yytext[0] );
  exit(-1);
}

void yyerror( struct YYLTYPE* locp, const char* msg ) {
  (void)locp;
  fprintf( stderr, "ERROR: %s\n", msg );
}

