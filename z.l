D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			([Ee][+-]?{D}+)
%{
#include "ast.h"
#include "z.tab.h"
#include <stdnoreturn.h>
void match( const char* t, const char* text );
noreturn void no_match( const char* text );
void yyerror( struct YYLTYPE* locp, void* sc, const char* msg );

#define CLONE_STRING \
  const char* text = yytext; \
  int len = yyleng; \
  yylval->_str = (char*)malloc( len + 1 ); \
  memcpy( yylval->_str, text, len ); \
  yylval->_str[len] = 0; 

#define MATCH( x ) match( #x, yytext ); return x; 

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
#pragma clang diagnostic ignored "-Wstrict-prototypes"
#pragma clang diagnostic ignored "-Wimplicit-function-declaration"
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wunneeded-internal-declaration"
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wsign-conversion"
%}
%option noyywrap nodefault yylineno reentrant bison-bridge bison-locations
%option header-file="z.lex.h"
%option outfile="z.lex.c"

%%

"alias"                       { MATCH( ALIAS ) }
"fn"                          { MATCH( FN ) }
"if"                          { MATCH( IF ) }
"else"                        { MATCH( ELSE ) }
"type"                        { MATCH( TYPE ) }
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
"}" { char tmp[2] = { yytext[0], 0 }; match( tmp, yytext ); return yytext[0]; }

i([1-9]|[1-9]{D}|[1-9]{D}{D}) { yylval->_bits = atoi( yytext + 1 ); MATCH( INT ) }
u([1-9]|[1-9]{D}|[1-9]{D}{D}) { yylval->_bits = atoi( yytext + 1 ); MATCH( UINT ) }
f(16|32|64|128)               { yylval->_bits = atoi( yytext + 1 ); MATCH( FLOAT ) }
[_a-zA-Z][_a-zA-Z0-9]*        { CLONE_STRING; MATCH( IDENTIFIER ) }

0[xX]{H}+		                  { CLONE_STRING; MATCH( CONSTANT ) }
0[0-7]*		                    { CLONE_STRING; MATCH( CONSTANT ) }
[1-9]{D}*		                  { CLONE_STRING; MATCH( CONSTANT ) }
{D}+{E}?		                  { CLONE_STRING; MATCH( CONSTANT ) }
{D}+"."{D}+{E}?	              { CLONE_STRING; MATCH( CONSTANT ) }
[\x0a]                        { MATCH( EOL ) }

[ ]                           {}
.                             { no_match( yytext ); }

%%

#include <stdio.h>

#pragma clang diagnostic pop

void match( const char* t, const char* text ) {
  if ( text[0] < 32 ) {
    switch ( text[0] ) {
    case '\n':
      text = "\\n";
      break;
    default:
      ASSERT( !"Implement" );
      break;
    }
  }
  fprintf( stdout, "Matched '%s' as '%s'\n", text, t );
}

noreturn void no_match( const char* text ) {
  fprintf( stderr, "ERROR: No matching token for '%c'(0x%02x)\n", text[0], text[0] );
  exit(-1);
}

void yyerror( struct YYLTYPE* locp, void* sc, const char* msg ) {
  (void)locp;
  (void)sc;
  fprintf( stderr, "ERROR: %s\n", msg );
}

