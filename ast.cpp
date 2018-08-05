#include "ast.h"
#include <vector>
#include <cstdio>

#define AST_INCREMENT 8096

struct ast_context {
  std::vector<std::vector<struct ast>>      _ast;
};

static struct ast*
get_ast( struct ast_context* ctx, ast_node_type ant ) {
  if ( ctx->_ast.back().size() == ctx->_ast.back().capacity() ) {
    ctx->_ast.resize( ctx->_ast.size() + 1 );
    ctx->_ast.back().reserve( AST_INCREMENT );
  }

  ctx->_ast.back().push_back( ast() );
  ast& ret = ctx->_ast.back().back();
  ret._type = ant;
  return &ret;
}

struct ast*
new_binary_op( struct ast_context* ctx, int binary_op, struct ast* l, struct ast* r ) {
  struct ast* ret = get_ast( ctx, AST_BINARY_OP );
  ret->_binary._op = binary_op;
  ret->_binary._l = l;
  ret->_binary._r = r;
  fprintf( stderr, "AST binary_op\n" );
  return ret;
}

struct ast*
new_ref( struct ast_context* ctx, const char* name ) {
  struct ast* ret = get_ast( ctx, AST_REF );
  ret->_ref._name = name;
  fprintf( stderr, "AST ref %s\n", name );
  return ret;
}

struct ast*
new_constant( struct ast_context* ctx, const char* c ) {
  struct ast* ret = get_ast( ctx, AST_CONSTANT );
  ret->_constant._value = c;
  fprintf( stderr, "AST constant\n" );
  return ret;
}

struct ast*
new_func( struct ast_context* ctx, const char* name, struct ast* parameters, struct ast* return_parameters, struct ast* body ) {
  struct ast* ret = get_ast( ctx, AST_FUNCTION );
  function& f = ret->_function;
  f._name = name;
  f._parameters = parameters;
  f._return_parameters = return_parameters;
  f._body = body;
  fprintf( stderr, "AST function %s\n", name );
  return ret;
}

struct ast*
new_symbol( struct ast_context* ctx, const char* name, struct ast* type_and_modifiers ) {
  struct ast* ret = get_ast( ctx, AST_SYMBOL );
  symbol& s = ret->_symbol;
  s._name = name;
  s._type_and_modifiers = type_and_modifiers;
  fprintf( stderr, "AST symbol %s\n", name );
  return ret;
}

struct ast*
new_symbol_type( struct ast_context* ctx, enum sym_type st, unsigned int bits ) {
  struct ast* ret = get_ast( ctx, AST_SYMBOL_TYPE );
  symbol_type& t = ret->_symbol_type;
  t._sym_type = st;
  t._bits = bits;
  fprintf( stderr, "AST symbol type %i\n", bits );
  return ret;
}

struct ast*
new_assignment( struct ast_context* ctx, const char* name, struct ast* expr ) {
  struct ast* ret = get_ast( ctx, AST_ASSIGNMENT );
  assignment& a = ret->_assignment;
  a._name = name;
  a._expr = expr;
  fprintf( stderr, "AST assignment %s\n", name );
  return ret;
}

struct ast_context*
new_ast_context(void) {
  struct ast_context* ret = new ast_context;
  ret->_ast.resize( 1 );
  ret->_ast.back().reserve( AST_INCREMENT );
  return ret;
}


void
free_ast_context( struct ast_context* ast ) {
  delete ast;
}

