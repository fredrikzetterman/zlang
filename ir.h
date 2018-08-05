#ifndef IR_H_
#define IR_H_


#ifdef __cplusplus
extern "C" {
#endif

struct ir_context;
struct ir_context* new_ir_context(void);
void delete_ir_context( struct ir_context* ctx );

struct ast;
void convert_ast_to_ir( struct ir_context* ctx, struct ast* a );

#ifdef __cplusplus
}
#endif

#endif

