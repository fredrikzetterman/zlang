#ifndef IR_H_
#define IR_H_


#ifdef __cplusplus
extern "C" {
#endif

struct ir_context;
struct ast_context;
struct ir_context* new_ir_context( const char* name, struct ast_context* ast_ctx );
void delete_ir_context( struct ir_context* ctx );

void convert_ast_to_ir( struct ir_context* ctx );

#ifdef __cplusplus
}
#endif

#endif

