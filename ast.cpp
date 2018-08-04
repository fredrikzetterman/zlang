#include "ast.h"
#include <vector>

#define AST_INCREMENT 8096
#define SYMBOL_INCREMENT 8096
#define REF_INCREMENT 8096

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
  return ret;
}

struct ast*
new_ref( struct ast_context* ctx, const char* sym ) {
  struct ast* ret = get_ast( ctx, AST_REF );
  ret->_ref._name = sym;
  return ret;
}

struct ast*
new_constant( struct ast_context* ctx, const char* c ) {
  struct ast* ret = get_ast( ctx, AST_CONSTANT );
  ret->_constant._value = c;
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

