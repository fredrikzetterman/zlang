#ifndef AST_H_
#define AST_H_

#ifdef __cplusplus
extern "C" {
#endif

struct ast;
enum ast_node_type {
  AST_BINARY_OP,
  AST_SYMBOL,
  AST_REF,
  AST_CONSTANT,
  AST_FUNCTION,
  AST_DECLARATION,
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

struct symbol {
  enum sym_type _type;
  unsigned int  _modifiers;
  const char*   _name;
  struct ref*   _ref_list;
};

struct ref {
  const char*   _name;
  struct ret*   _next;
  const char*   _filename;
  unsigned int  _line_no;
  unsigned int  _column_no;
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

struct ast {
  enum ast_node_type _type;
  struct ast*        _next;
  union {
    struct binary_op  _binary;
    struct symbol     _symbol;
    struct ref        _ref;
    struct constant   _constant;
    struct function   _function;
  };
};


struct ast_context;

struct ast_context* new_ast_context(void);
void free_ast_context( struct ast_context* ast );

struct ast* new_binary_op( struct ast_context* ctx, int binary_op, struct ast* l, struct ast* r );
struct ast* new_ref( struct ast_context* ctx, const char* sym );
struct ast* new_constant( struct ast_context* ctx, const char* c );
struct ast* new_func( struct ast_context* ctx, const char* name, struct ast* parameters, struct ast* return_parameters, struct ast* body );
struct ast* new_symbol( struct ast_context* ctx, const char* name, struct ast* type_and_modifiers );
struct ast* new_type( struct ast_context* ctx, enum sym_type st, unsigned int bits );
struct ast* new_assignment( struct ast_context* ctx, const char* name, struct ast* expr );

#ifdef __cplusplus
}
#endif

#endif

