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
  ir_context( const char* name ) : _ir( _llvm_ctx ), _module( name, _llvm_ctx ) {
  }

  LLVMContext _llvm_ctx;
  IRBuilder<> _ir;
  Module      _module;
};

static Type*
get_llvm_type( struct ir_context* ctx, struct ast* a ) {
  assert( ret->_type == AST_SYMBOL );
  if ( a == nullptr ) {
    return Type::getVoidTy( ctx->_llvm_ctx );
  }

  switch ( a->_symbol._sym_type ) {
  case SYM_INT:
  case SYM_UINT:
    switch ( a->_symbol._primitive_type._bits ) {
    case 128: return Type::getInt128Ty( ctx->_llvm_ctx );
    case 64: return Type::getInt64Ty( ctx->_llvm_ctx );
    case 32: return Type::getInt32Ty( ctx->_llvm_ctx );
    case 16: return Type::getInt16Ty( ctx->_llvm_ctx );
    case 8: return Type::getInt8Ty( ctx->_llvm_ctx );
    default:
      assert( !"Implement" );
    }
    break;
  case SYM_FLOAT:
    switch ( a->_symbol._primitive_type._bits ) {
    case 128: return Type::getFP128Ty( ctx->_llvm_ctx );
    case 64: return Type::getDoubleTy( ctx->_llvm_ctx );
    case 32: return Type::getFloatTy( ctx->_llvm_ctx );
    case 16: return Type::getHalfTy( ctx->_llvm_ctx );
    default:
      assert( !"Implement" );
    }
    break;
  case SYM_UDT:
    assert( !"Implement" );
    break;
  }

  return nullptr;
}

static int ir_visitor( struct walk_ast_data wad ) {
  ir_context* ctx = static_cast<ir_context*>( wad._user_data );
  switch ( wad._curr->_type ) {
  case AST_BINARY_OP:
    //fprintf( stderr, "%*sAST_BINARY_OP: %c\n", depth, "", wad._curr->_binary._op );
    break;
  case AST_ASSIGNMENT:
    //fprintf( stderr, "%*sAST_ASSIGNMENT: %s\n", depth, "", wad._curr->_assignment._name );
    break;
  case AST_FUNCTION:
    {
      ast* ret = wad._curr->_function._return_parameters;
      assert( ret == nullptr || ret->_next == nullptr );
      Type* ret_type = get_llvm_type( ctx, ret );

      Type* params[256];
      unsigned int curr = 0;
      ast* curr_p = wad._curr->_function._parameters;

      while ( curr < sizeof(params)/sizeof(params[0]) && curr_p ) {
        params[curr++] = get_llvm_type( ctx, curr_p );
        curr_p = curr_p->_next;
      }

      assert( curr < sizeof(params)/sizeof(params[0]) );

      ArrayRef<Type*> actual_params( params, params + curr );

      FunctionType* ft = FunctionType::get( ret_type, actual_params, false );
      Function* f = Function::Create( ft, Function::InternalLinkage, wad._curr->_function._name, &ctx->_module );
      (void)f;
    }
    return 0;
  case AST_SYMBOL:
    //fprintf( stderr, "%*sAST_SYMBOL: %s\n", depth, "", wad._curr->_symbol._name );
    break;
  case AST_REF:
    //fprintf( stderr, "%*sAST_REF: %s\n", depth, "", wad._curr->_ref._name );
    break;
  case AST_CONSTANT:
    //fprintf( stderr, "%*sAST_CONSTANT: %s\n", depth, "", wad._curr->_constant._value );
    break;
  }
  return 1;
}

struct ir_context*
new_ir_context( const char* name ) {
  return new ir_context( name );
}

void
delete_ir_context( struct ir_context* ctx ) {
  delete ctx;
}

void
convert_ast_to_ir( struct ir_context* ctx, struct ast* a ) {
  struct walk_ast_data wad;
  memset( &wad, 0, sizeof(wad) );
  wad._curr = a;
  wad._visit = ir_visitor;
  wad._user_data = ctx;

  walk_ast( wad );

  ctx->_module.print( errs(), nullptr );
}

