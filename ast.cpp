#include "ast.h"
#include <vector>
#include <cstdio>
#include <cassert>

#define AST_INCREMENT 8096

struct ast_context {
  std::vector<std::vector<struct ast>>      _ast;
  struct ast* _start = nullptr;
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
  ret._next = nullptr;
  ret._parent = nullptr;
  return &ret;
}

struct ast*
new_binary_op( struct ast_context* ctx, int binary_op, struct ast* l, struct ast* r ) {
  struct ast* ret = get_ast( ctx, AST_BINARY_OP );
  ret->_binary._op = binary_op;
  ret->_binary._l = l;
  ret->_binary._r = r;

  l->_parent = ret;
  r->_parent = ret;
  return ret;
}

struct ast*
new_ref( struct ast_context* ctx, const char* name ) {
  struct ast* ret = get_ast( ctx, AST_REF );
  ret->_ref._name = name;
  return ret;
}

struct ast*
new_constant( struct ast_context* ctx, const char* c ) {
  struct ast* ret = get_ast( ctx, AST_CONSTANT );
  ret->_constant._value = c;
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

  parameters->_parent = ret;
  return_parameters->_parent = ret;
  body->_parent = ret;

  return ret;
}

struct ast*
new_symbol( struct ast_context* ctx, const char* name, struct ast* type_and_modifiers ) {
  struct ast* ret = get_ast( ctx, AST_SYMBOL );
  symbol& s = ret->_symbol;
  s._name = name;
  s._type_and_modifiers = type_and_modifiers;

  type_and_modifiers->_parent = ret;

  return ret;
}

struct ast*
new_symbol_type( struct ast_context* ctx, enum sym_type st, unsigned int bits ) {
  struct ast* ret = get_ast( ctx, AST_SYMBOL_TYPE );
  symbol_type& t = ret->_symbol_type;
  t._sym_type = st;
  t._bits = bits;
  return ret;
}

struct ast*
new_assignment( struct ast_context* ctx, const char* name, struct ast* expr ) {
  struct ast* ret = get_ast( ctx, AST_ASSIGNMENT );
  assignment& a = ret->_assignment;
  a._name = name;
  a._expr = expr;

  expr->_parent = ret;

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
delete_ast_context( struct ast_context* ast ) {
  delete ast;
}

void
set_ast_start( struct ast_context* ctx, struct ast* a ) {
  ctx->_start = a;
}

struct ast*
get_ast_start( struct ast_context* ctx ) {
  return ctx->_start;
}

void
walk_ast( struct ast* a, int depth, int (*visit)( struct ast* a, int depth ) ) {
  if ( a == nullptr ) {
    return;
  }
  assert( depth == 0 || a->_parent );
  while ( a ) {
    int recurr = visit( a, depth );
    if ( recurr ) {
      switch ( a->_type ) {
      case AST_BINARY_OP:
        walk_ast( a->_binary._l, depth + 1, visit );
        walk_ast( a->_binary._r, depth + 1, visit );
        break;
      case AST_ASSIGNMENT:
        walk_ast( a->_assignment._expr, depth + 1, visit );
        break;
      case AST_FUNCTION:
        walk_ast( a->_function._parameters, depth + 1, visit );
        walk_ast( a->_function._return_parameters, depth + 1, visit );
        walk_ast( a->_function._body, depth + 1, visit );
        break;
      case AST_SYMBOL:
        walk_ast( a->_symbol._type_and_modifiers, depth + 1, visit );
        break;
      case AST_REF:
      case AST_CONSTANT:
      case AST_SYMBOL_TYPE:
        break;
      }
    }
    a = a->_next;
  }
}

int
ast_printer( struct ast* a, int depth ) {
  depth *= 2;
  switch ( a->_type ) {
  case AST_BINARY_OP:
    fprintf( stderr, "%*sAST_BINARY_OP: %c\n", depth, "", a->_binary._op );
    break;
  case AST_ASSIGNMENT:
    fprintf( stderr, "%*sAST_ASSIGNMENT: %s\n", depth, "", a->_assignment._name );
    break;
  case AST_FUNCTION:
    fprintf( stderr, "%*sAST_FUNCTION: %s\n", depth, "", a->_function._name );
    break;
  case AST_SYMBOL:
    fprintf( stderr, "%*sAST_SYMBOL: %s\n", depth, "", a->_symbol._name );
    break;
  case AST_REF:
    fprintf( stderr, "%*sAST_REF: %s\n", depth, "", a->_ref._name );
    break;
  case AST_CONSTANT:
    fprintf( stderr, "%*sAST_CONSTANT: %s\n", depth, "", a->_constant._value );
    break;
  case AST_SYMBOL_TYPE:
    fprintf( stderr, "%*sAST_SYMBOL_TYPE: %i\n", depth, "", a->_symbol_type._bits );
    break;
  }
  return 1;
}

