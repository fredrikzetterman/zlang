#include "ast.h"
#include <vector>
#include <cstdio>
#include <cstring>

struct ast_context {
  std::vector<struct ast>   _ast;
  struct ast*               _start = nullptr;
  bool                      _debug_print = false;
};

static struct ast*
get_ast( struct ast_context* ctx, ast_node_type ant ) {
  // TODO: If this ever happens, we need to remap all AST pointers to new memory offsets
  ASSERT( ctx->_ast.size() < ctx->_ast.capacity() );
  ctx->_ast.push_back( ast() );
  ast& ret = ctx->_ast.back();
  memset( &ret, 0, sizeof(ret) );
  ret._type = ant;
  return &ret;
}

struct ast*
new_binary_op( struct ast_context* ctx, int binary_op, struct ast* l, struct ast* r ) {
  if ( ctx->_debug_print ) {
    fprintf( stderr, "new_binary_op\n" );
  }
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
  if ( ctx->_debug_print ) {
    fprintf( stderr, "new_ref '%s'\n", name );
  }
  struct ast* ret = get_ast( ctx, AST_REF );
  ret->_ref._name = name;
  return ret;
}

struct ast*
new_constant( struct ast_context* ctx, const char* c ) {
  if ( ctx->_debug_print ) {
    fprintf( stderr, "new_constant '%s'\n", c );
  }
  struct ast* ret = get_ast( ctx, AST_CONSTANT );
  ret->_constant._value = c;
  return ret;
}

struct ast*
new_func( struct ast_context* ctx, const char* name, struct ast* parameters, struct ast* return_parameters, struct ast* body ) {
  if ( ctx->_debug_print ) {
    fprintf( stderr, "new_func '%s'\n", name );
  }
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
new_symbol( struct ast_context* ctx, const char* name, struct primitive_type pt ) {
  if ( ctx->_debug_print ) {
    fprintf( stderr, "new_symbol '%s'\n", name ? name : "<anonymous>" );
  }
  struct ast* ret = get_ast( ctx, AST_SYMBOL );
  symbol& s = ret->_symbol;
  s._name = name;
  s._sym_type = pt._sym_type;
  s._primitive_type = pt;

  return ret;
}

void
set_symbol_name( struct ast_context* ctx, struct ast* node, const char* name ) {
  if ( ctx->_debug_print ) {
    fprintf( stderr, "set_symbol_name '%s'\n", name );
  }
  ASSERT( node && name );
  node->_symbol._name = name;
}

struct ast*
new_assignment( struct ast_context* ctx, const char* name, struct ast* expr ) {
  if ( ctx->_debug_print ) {
    fprintf( stderr, "new_assignment '%s'\n", name );
  }
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
  ret->_debug_print = true;
  ret->_ast.reserve( 65535 );
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

unsigned int
get_ast_count( struct ast_context* ctx, struct ast** begin ) {
  *begin = &ctx->_ast.front();
  return static_cast<unsigned int>( ctx->_ast.size() );
}


static void
print_node( struct walk_ast_data wad ) {
  int depth = wad._depth * 2;
  switch ( wad._curr->_type ) {
  case AST_BINARY_OP:
    fprintf( stderr, "%*sAST_BINARY_OP: %c\n", depth, "", wad._curr->_binary._op );
    break;
  case AST_ASSIGNMENT:
    fprintf( stderr, "%*sAST_ASSIGNMENT: %s\n", depth, "", wad._curr->_assignment._name );
    break;
  case AST_FUNCTION:
    fprintf( stderr, "%*sAST_FUNCTION: %s\n", depth, "", wad._curr->_function._name );
    break;
  case AST_SYMBOL:
    fprintf( stderr, "%*sAST_SYMBOL: %s\n", depth, "", wad._curr->_symbol._name );
    break;
  case AST_REF:
    fprintf( stderr, "%*sAST_REF: %s\n", depth, "", wad._curr->_ref._name );
    break;
  case AST_CONSTANT:
    fprintf( stderr, "%*sAST_CONSTANT: %s\n", depth, "", wad._curr->_constant._value );
    break;
  }
}

void
print_ast( struct walk_ast_data wad ) {
  if ( wad._curr == nullptr ) {
    return;
  }
  ASSERT( wad._depth == 0 || wad._curr->_parent );
  while ( wad._curr ) {
    print_node( wad );
    walk_ast_data other = wad;
    other._depth += 1;
    switch ( wad._curr->_type ) {
    case AST_BINARY_OP:
      other._curr = wad._curr->_binary._l;
      print_ast( other );
      other._curr = wad._curr->_binary._r;
      print_ast( other );
      break;
    case AST_ASSIGNMENT:
      other._curr = wad._curr->_assignment._expr;
      print_ast( other );
      break;
    case AST_FUNCTION:
      other._curr = wad._curr->_function._parameters;
      print_ast( other );
      other._curr = wad._curr->_function._return_parameters;
      print_ast( other );
      other._curr = wad._curr->_function._body;
      print_ast( other );
      break;
    case AST_SYMBOL:
    case AST_REF:
    case AST_CONSTANT:
      break;
    }
    wad._curr = wad._curr->_next;
  }
}

