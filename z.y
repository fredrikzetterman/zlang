%{
#include "ast.h"
#include <stddef.h>

static struct ast_context* ctx = NULL;

struct YYLTYPE;
int yylex(int *lvalp, struct YYLTYPE *locp);
void yyerror( struct YYLTYPE* locp, const char* msg );

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wused-but-marked-unused"
#pragma clang diagnostic ignored "-Wunreachable-code"
%}


%token INT UINT FLOAT
%token IDENTIFIER CONSTANT STRING_LITERAL
%token ALIAS TYPEDEF
%token EOL
%token FN
%token STRUCT UNION
%token CONST
%token IF ELSE
%token UMINUS ASSIGN

%locations
%define api.pure full
%define parse.error verbose

%expect 0

%start translation_unit

%right ASSIGN
%left '+' '-'
%left '*' '/' '%'
%nonassoc UMINUS

%%

type_specifier
  : INT
  | UINT
  | FLOAT
  ;

type_qualifier
  : CONST
  ;

type_qualifier_list
	: type_qualifier
	| type_qualifier_list type_qualifier
	;

type_declaration
  : type_specifier
  | type_qualifier_list type_specifier
  ;

declaration_specifiers
  : type_declaration
  ;

declaration
  : declaration_specifiers IDENTIFIER EOL
  ;

argument
  : type_declaration IDENTIFIER
  ;

assignment_operator
  : '='
  ;

expression
  : expression '+' expression       { $$ = new_binary_op( ctx, '+', $1, $3 ); }
  | expression '-' expression       { $$ = new_binary_op( ctx, '-', $1, $3 ); }
  | expression '*' expression       { $$ = new_binary_op( ctx, '*', $1, $3 ); }
  | expression '/' expression       { $$ = new_binary_op( ctx, '/', $1, $3 ); }
  | expression '%' expression       { $$ = new_binary_op( ctx, '%', $1, $3 ); }
  | '(' expression ')'
  //| '-' expression %prec UMINUS
  | IDENTIFIER                      { $$ = new_ref( ctx, $1 ); }
  | CONSTANT                        { $$ = new_constant( ctx, $1 ); }
  ;

assignment_expression
  : IDENTIFIER assignment_operator expression
  ;

expression_statement
  : assignment_expression 
  | expression
  ;

argument_list
  : %empty
  | argument
  | argument_list ',' argument
  ;

statement
  : compound_statement
  | expression_statement
  ;

block_item
  : declaration
  | statement
  ;

block_item_list
  : block_item
  | block_item_list block_item
  ;

compound_statement
  : '{' '}'
  | '{' block_item_list '}'
  ;

function_definition
  : FN IDENTIFIER '(' argument_list ')' compound_statement
  | FN IDENTIFIER '(' argument_list ':' argument_list ')' compound_statement
  ;

external_declaration
	: declaration
  | function_definition
	;

translation_unit
  : external_declaration
  | translation_unit external_declaration
  ;

%%
#pragma clang diagnostic pop
#include <stdio.h>
extern char yytext[];
extern int column;

int main( int argc, const char* const* argv ) {
  (void)argc;
  (void)argv;
  ctx = new_ast_context();
  yyparse();
  free_ast_context( ctx );
  ctx = NULL;
}

