#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include "ast.h"
#include "ir.h"

using namespace llvm;


struct ir_context {
  ir_context() : _ir( _llvm_ctx ) {

  }

  LLVMContext _llvm_ctx;
  IRBuilder<> _ir;

};


struct ir_context*
new_ir_context(void) {
  return new ir_context;
}

void
delete_ir_context( struct ir_context* ctx ) {
  delete ctx;
}

void
convert_ast_to_ir( struct ir_context* ctx, struct ast* a ) {
  (void)ctx;
  (void)a;
}

