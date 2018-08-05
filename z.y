%{
#include "ast.h"
#include <stddef.h>
#include <string.h>

static struct ast_context* ctx = NULL;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wused-but-marked-unused"
#pragma clang diagnostic ignored "-Wunreachable-code"
%}

%union {
  char*         _str;
  struct ast*   _ast;
  unsigned int  _bits;
  struct primitive_type _primitive_type;
}

%{
void yyerror( struct YYLTYPE* locp, void* sc, const char* msg );
#include "z.lex.h"
%}

%token INT UINT FLOAT
%token IDENTIFIER CONSTANT STRING_LITERAL
%token ALIAS TYPE
%token EOL
%token FN
%token CONST
%token IF ELSE
%token UMINUS ASSIGN

%type<_str> IDENTIFIER CONSTANT
%type<_primitive_type> type_specifier
%type<_bits> INT UINT FLOAT
%type<_ast> expression function_definition parameter_list compound_statement parameter type_declaration block_item_list block_item statement expression_statement assignment_expression external_declaration translation_unit


%locations
%define api.pure full
%define parse.error verbose

%lex-param   { void* sc }
%parse-param { void* sc }

%expect 0

%start translation_unit

%right ASSIGN
%left '+' '-'
%left '*' '/' '%'
%nonassoc UMINUS

%%

type_specifier
  : INT   { struct primitive_type tmp = { SYM_INT, $1 }; $$ = tmp; }
  | UINT  { struct primitive_type tmp = { SYM_UINT, $1 }; $$ = tmp; }
  | FLOAT { struct primitive_type tmp = { SYM_FLOAT, $1 }; $$ = tmp; }
  ;

/*
type_qualifier
  : CONST
  ;

type_qualifier_list
	: type_qualifier
	| type_qualifier_list type_qualifier
	;
*/

type_declaration
  : type_specifier { $$ = new_symbol( ctx, NULL, $1 ); }
  //| type_qualifier_list type_specifier
  ;

parameter
  : type_declaration IDENTIFIER { $1->_symbol._name = $2; $$ = $1; }

expression
  : expression '+' expression       { $$ = new_binary_op( ctx, '+', $1, $3 ); }
  | expression '-' expression       { $$ = new_binary_op( ctx, '-', $1, $3 ); }
  | expression '*' expression       { $$ = new_binary_op( ctx, '*', $1, $3 ); }
  | expression '/' expression       { $$ = new_binary_op( ctx, '/', $1, $3 ); }
  | expression '%' expression       { $$ = new_binary_op( ctx, '%', $1, $3 ); }
  | '(' expression ')'              { $$ = $2; }
  //| '-' expression %prec UMINUS
  | IDENTIFIER                      { $$ = new_ref( ctx, $1 ); }
  | CONSTANT                        { $$ = new_constant( ctx, $1 ); }
  ;

assignment_expression
  : IDENTIFIER '=' expression { $$ = new_assignment( ctx, $1, $3 ); }
  ;

expression_statement
  : assignment_expression 
  | expression
  ;

parameter_list
  : parameter { $$ = $1; }
  | parameter_list ',' parameter { append_ast( $1, $3 ); $$ = $1; }
  ;

statement
  : compound_statement
  | expression_statement
  ;

block_item
  : statement
  //| declaration
  ;

block_item_list
  : block_item 
  | block_item_list block_item
  ;

compound_statement
  : '{' '}' { $$ = NULL; }
  | '{' block_item_list '}' { $$ = $2; }
  ;

function_definition
  : FN IDENTIFIER '(' parameter_list ')' compound_statement { $$ = new_func( ctx, $2, $4, NULL, $6 ); }
  | FN IDENTIFIER '(' parameter_list ':' parameter_list ')' compound_statement { $$ = new_func( ctx, $2, $4, $6, $8 ); }
  | FN IDENTIFIER '(' ')' compound_statement { $$ = new_func( ctx, $2, NULL, NULL, $5 ); }
  | FN IDENTIFIER '(' ':' parameter_list ')' compound_statement { $$ = new_func( ctx, $2, NULL, $5, $7 ); }
  ;

external_declaration
  : function_definition { $$ = $1; }
	//| declaration
	;

translation_unit
  : external_declaration                  { $$ = $1; set_ast_start( ctx, $1 ); }
  | translation_unit external_declaration { append_ast( $1, $2 ); $$ = $1; set_ast_start( ctx, $1 ); }
  ;

%%
#pragma clang diagnostic pop
#include <stdio.h>
#include "ir.h"

int main( int argc, const char* const* argv ) {
  yyscan_t sc;

  yylex_init(&sc);

  const char* module_name = "stdin";
  FILE* f = NULL;
  if ( argc > 1 ) {
    f = fopen( argv[1], "r" );
    if ( !f ) {
      fprintf( stderr, "Unable to open '%s' for reading", argv[1] );
      exit(-1);
    }
    yyset_in( f, sc );
    // TODO: Sanitize argv[1]
    module_name = argv[1];
  }
  else {
    yyset_in( stdin, sc );
  }

  ctx = new_ast_context();
  yyparse( sc );

  struct walk_ast_data wad;
  memset( &wad, 0, sizeof(wad) );
  wad._curr = get_ast_start( ctx );
  wad._visit = ast_printer;

  walk_ast( wad );

  fprintf( stderr, "===\nIR:\n===\n" );

  struct ir_context* ir_ctx = new_ir_context( module_name );
  convert_ast_to_ir( ir_ctx, get_ast_start( ctx ) );

  delete_ir_context( ir_ctx );
  delete_ast_context( ctx );
  ctx = NULL;

  if ( f ) {
    fclose( f );
    f = NULL;
  }

  yylex_destroy(sc);
}

