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
  AST_CONSTANT
};

struct binary_op {
  int         _op;
  struct ast* _l;
  struct ast* _r;
};

struct symbol {
  char*         _name;
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

struct ast {
  enum ast_node_type _type;
  union {
    struct binary_op  _binary;
    struct symbol     _symbol;
    struct ref        _ref;
    struct constant   _constant;
  };
};


struct ast_context;

struct ast_context* new_ast_context(void);
void free_ast_context( struct ast_context* ast );

struct ast* new_binary_op( struct ast_context* ctx, int binary_op, struct ast* l, struct ast* r );
struct ast* new_ref( struct ast_context* ctx, const char* sym );
struct ast* new_constant( struct ast_context* ctx, const char* c );

#ifdef __cplusplus
}
#endif

#endif

