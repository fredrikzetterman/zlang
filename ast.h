#ifndef AST_H_
#define AST_H_

#ifdef __cplusplus
extern "C" {
#endif

struct ast;
enum ast_node_type {
  AST_BINARY_OP,
  AST_REF,
  AST_CONSTANT,
  AST_FUNCTION,
  AST_SYMBOL,
  AST_SYMBOL_TYPE, // TODO: Shouldn't be a node in AST, but a temp struct
  AST_ASSIGNMENT,
};

enum sym_type {
  SYM_INT,
  SYM_UINT,
  SYM_FLOAT,
  SYM_STRUCT,
  SYM_UNION
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

struct function {
  const char* _name;
  struct ast* _parameters;
  struct ast* _return_parameters;
  struct ast* _body;
};

struct symbol {
  const char* _name;
  struct ast* _type_and_modifiers;
};

struct symbol_type {
  enum sym_type _sym_type;
  unsigned int _bits;
};

struct assignment {
  const char* _name;
  struct ast* _expr;
};

struct ast {
  enum ast_node_type _type;
  struct ast*        _next;
  struct ast*        _parent;
  union {
    struct binary_op    _binary;
    struct ref          _ref;
    struct constant     _constant;
    struct function     _function;
    struct symbol       _symbol;
    struct symbol_type  _symbol_type;
    struct assignment   _assignment;
  };
};

static inline void append_ast( struct ast* first, struct ast* second ) {
  while ( first->_next ) {
    first = first->_next;
  }
  first->_next = second;
}

struct ast_context;

struct ast_context* new_ast_context(void);
void delete_ast_context( struct ast_context* ast );
void set_ast_start( struct ast_context* ctx, struct ast* a );
struct ast* get_ast_start( struct ast_context* ctx );

struct ast* new_binary_op( struct ast_context* ctx, int binary_op, struct ast* l, struct ast* r );
struct ast* new_ref( struct ast_context* ctx, const char* sym );
struct ast* new_constant( struct ast_context* ctx, const char* c );
struct ast* new_func( struct ast_context* ctx, const char* name, struct ast* parameters, struct ast* return_parameters, struct ast* body );
struct ast* new_symbol( struct ast_context* ctx, const char* name, struct ast* type_and_modifiers );
struct ast* new_symbol_type( struct ast_context* ctx, enum sym_type st, unsigned int bits );
struct ast* new_assignment( struct ast_context* ctx, const char* name, struct ast* expr );

void walk_ast( struct ast* a, int depth, int (*visit)( struct ast* a, int depth ) );

int ast_printer( struct ast* a, int depth );

#ifdef __cplusplus
}
#endif

#endif

