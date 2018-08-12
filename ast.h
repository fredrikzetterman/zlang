#ifndef AST_H_
#define AST_H_

#ifdef __cplusplus
extern "C" {
#endif

struct ast;
enum ast_node_type {
  AST_BINARY_OP,
  AST_REF,
  AST_CONSTANT_INT,
  AST_CONSTANT_FLOAT,
  AST_FUNCTION,
  AST_SYMBOL,
  AST_ASSIGNMENT,
  AST_SCOPE_BEGIN,
  AST_SCOPE_END,
};

enum sym_type {
  SYM_UNDEFINED,
  SYM_INT,
  SYM_UINT,
  SYM_FLOAT,
  SYM_UDT,
  SYM_CONSTANT,
};

enum sym_type_modifiers {
  SYM_M_CONST = 1<<0
};

struct binary_op {
  int         _op;
  struct ast* _l;
  struct ast* _r;
};

struct ref {
  const char*   _name;
};

struct constant {
  const char*   _value;
};

struct primitive_type {
  enum sym_type _sym_type;
  unsigned int  _bits;
};

struct function {
  const char* _name;
  struct ast* _parameters;
  struct ast* _return_parameters;
  struct ast* _body;
};


struct symbol {
  const char*   _name;
  enum sym_type _sym_type;
  union {
    struct ast*           _udt;
    struct primitive_type _primitive_type;
  };
};

struct assignment {
  const char* _name;
  struct ast* _expr;
};

struct ast {
  enum ast_node_type _type;
  struct ast*        _next;
  struct ast*        _parent;
  struct primitive_type _result;
  union {
    struct binary_op    _binary;
    struct ref          _ref;
    struct constant     _constant;
    struct function     _function;
    struct symbol       _symbol;
    struct assignment   _assignment;
  };
};

static inline struct ast* append_ast( struct ast* first, struct ast* second ) {
  if ( !first ) {
    return second;
  }
  if ( !second ) {
    return first;
  }
  while ( first->_next ) {
    first = first->_next;
  }
  first->_next = second;
  return first;
}

struct ast_context;

struct ast_context* new_ast_context(void);
void delete_ast_context( struct ast_context* ast );
void set_ast_start( struct ast_context* ctx, struct ast* a );
struct ast* get_ast_start( struct ast_context* ctx );
unsigned int get_ast_count( struct ast_context* ctx, struct ast** begin );

struct ast* new_binary_op( struct ast_context* ctx, int binary_op, struct ast* l, struct ast* r );
struct ast* new_ref( struct ast_context* ctx, const char* sym );
struct ast* new_constant( struct ast_context* ctx, const char* c, enum ast_node_type type );
struct ast* new_func( struct ast_context* ctx, const char* name, struct ast* parameters, struct ast* return_parameters, struct ast* body );
struct ast* new_symbol( struct ast_context* ctx, const char* name, struct primitive_type pt );
struct ast* new_assignment( struct ast_context* ctx, const char* name, struct ast* expr );
struct ast* new_scope( struct ast_context* ctx, enum ast_node_type type );

void set_symbol_name( struct ast_context* ctx, struct ast* node, const char* name );

struct walk_ast_data {
  void*       _user_data;
  struct ast* _curr;
  int         _depth;
};
void print_ast( struct walk_ast_data wad );

#ifdef __cplusplus
}
#endif

#endif

