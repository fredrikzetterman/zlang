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

struct ir_data {
  Value*  _val;
};

struct ir_context {
  ir_context( const char* name, struct ast_context* ast_ctx ) :
    _ir( _llvm_ctx ),
    _module( name, _llvm_ctx ),
    _ast_ctx( ast_ctx )
  {
    _ir_data.resize( get_ast_count( _ast_ctx, &_ast_begin ) );
    memset( &_ir_data.front(), 0, sizeof( ir_data ) * _ir_data.size() );
    FastMathFlags fmf;

    // Always set fast-math
    fmf.setUnsafeAlgebra(); // later changed to setFast( true )
    _ir.setFastMathFlags( fmf );
  }

  LLVMContext           _llvm_ctx;
  IRBuilder<>           _ir;
  Module                _module;
  struct ast_context*   _ast_ctx;
  struct ast*           _ast_begin;
  std::vector<ir_data>  _ir_data;
};

static Type*
get_llvm_type( struct ir_context* ctx, struct ast* a ) {
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
      ASSERT( !"Implement" );
    }
    break;
  case SYM_FLOAT:
    switch ( a->_symbol._primitive_type._bits ) {
    case 128: return Type::getFP128Ty( ctx->_llvm_ctx );
    case 64: return Type::getDoubleTy( ctx->_llvm_ctx );
    case 32: return Type::getFloatTy( ctx->_llvm_ctx );
    case 16: return Type::getHalfTy( ctx->_llvm_ctx );
    default:
      ASSERT( !"Implement" );
    }
    break;
  case SYM_UDT:
  case SYM_UNDEFINED:
  case SYM_CONSTANT:
    ASSERT( !"Implement" );
    break;
  }

  return nullptr;
}

struct ir_visit_data {
  struct ir_context* _ctx;
  struct ast*        _curr;
};

static ir_data&
get_ir_data( struct ir_context* ctx, struct ir_visit_data& ivd ) {
  return ctx->_ir_data[static_cast<size_t>( ivd._curr - ctx->_ast_begin )];
}

static struct primitive_type
check_valid( ir_visit_data& l, ir_visit_data& r ) {
  ASSERT( l._curr->_result._sym_type != SYM_UNDEFINED );
  ASSERT( r._curr->_result._sym_type != SYM_UNDEFINED );
  if ( l._curr->_result._sym_type == SYM_CONSTANT ) {
    // TODO: Make sure constant fits?
    return r._curr->_result;
  }
  if ( r._curr->_result._sym_type == SYM_CONSTANT ) {
    // TODO: Make sure constant fits?
    return l._curr->_result;
  }
  ASSERT( l._curr->_result._sym_type == r._curr->_result._sym_type );
  primitive_type ret = l._curr->_result;
  ret._bits = l._curr->_result._bits > r._curr->_result._bits ? l._curr->_result._bits : r._curr->_result._bits;
  return ret;
}

static void
ir_visitor( struct ir_visit_data& ivd ) {
  ir_context* ctx = ivd._ctx;
  while ( ivd._curr ) {
    ASSERT( ivd._curr );
    ir_data& id = get_ir_data( ctx, ivd );

    ASSERT( id._val == nullptr );
    switch ( ivd._curr->_type ) {
    case AST_BINARY_OP:
      {
        ir_visit_data l = ivd;
        l._curr = ivd._curr->_binary._l;
        ir_visitor( l );
        ir_data& ld = get_ir_data( ctx, l );

        ir_visit_data r = ivd;
        r._curr = ivd._curr->_binary._r;
        ir_visitor( r );
        ir_data& rd = get_ir_data( ctx, r );

        ASSERT( ld._val && rd._val );

        ivd._curr->_result = check_valid( l, r );

        if ( ivd._curr->_result._sym_type == SYM_INT ) {
          switch ( ivd._curr->_binary._op ) {
          case '+':
            id._val = ctx->_ir.CreateAdd( ld._val, rd._val );
            break;
          case '-':
            id._val = ctx->_ir.CreateSub( ld._val, rd._val );
            break;
          case '*':
            id._val = ctx->_ir.CreateMul( ld._val, rd._val );
            break;
          case '/':
            id._val = ctx->_ir.CreateSDiv( ld._val, rd._val );
            break;
          case '%':
            id._val = ctx->_ir.CreateSRem( ld._val, rd._val );
            break;
          default:
            ASSERT( !"Implement" );
          }
        }
        else if ( ivd._curr->_result._sym_type == SYM_UINT ) {
          switch ( ivd._curr->_binary._op ) {
          case '+':
            id._val = ctx->_ir.CreateAdd( ld._val, rd._val );
            break;
          case '-':
            id._val = ctx->_ir.CreateSub( ld._val, rd._val );
            break;
          case '*':
            id._val = ctx->_ir.CreateMul( ld._val, rd._val );
            break;
          case '/':
            id._val = ctx->_ir.CreateUDiv( ld._val, rd._val );
            break;
          case '%':
            id._val = ctx->_ir.CreateURem( ld._val, rd._val );
            break;
          default:
            ASSERT( !"Implement" );
          }
        }
        else if ( ivd._curr->_result._sym_type == SYM_FLOAT ) {
          switch ( ivd._curr->_binary._op ) {
          case '+':
            id._val = ctx->_ir.CreateFAdd( ld._val, rd._val );
            break;
          case '-':
            id._val = ctx->_ir.CreateFSub( ld._val, rd._val );
            break;
          case '*':
            id._val = ctx->_ir.CreateFMul( ld._val, rd._val );
            break;
          case '/':
            id._val = ctx->_ir.CreateFDiv( ld._val, rd._val );
            break;
          case '%':
            id._val = ctx->_ir.CreateFRem( ld._val, rd._val );
            break;
          default:
            ASSERT( !"Implement" );
          }
        }
      }
      break;
    case AST_ASSIGNMENT:
      //fprintf( stderr, "%*sAST_ASSIGNMENT: %s\n", depth, "", ivd._curr->_assignment._name );
      break;
    case AST_FUNCTION:
      {
        ast* ret = ivd._curr->_function._return_parameters;
        ASSERT( ret == nullptr || ret->_next == nullptr );
        Type* ret_type = get_llvm_type( ctx, ret );

        Type* params[256];
        unsigned int curr = 0;
        ast* curr_p = ivd._curr->_function._parameters;

        while ( curr < sizeof(params)/sizeof(params[0]) && curr_p ) {
          params[curr++] = get_llvm_type( ctx, curr_p );
          curr_p = curr_p->_next;
        }

        ASSERT( curr < sizeof(params)/sizeof(params[0]) );

        ArrayRef<Type*> actual_params( params, params + curr );

        FunctionType* ft = FunctionType::get( ret_type, actual_params, false );
        Function* f = Function::Create( ft, Function::InternalLinkage, ivd._curr->_function._name, &ctx->_module );

        id._val = f;
      }
      break;
    case AST_SYMBOL:
      //fprintf( stderr, "%*sAST_SYMBOL: %s\n", depth, "", ivd._curr->_symbol._name );
      break;
    case AST_REF:
      //fprintf( stderr, "%*sAST_REF: %s\n", depth, "", ivd._curr->_ref._name );
      break;
    case AST_CONSTANT:
      //fprintf( stderr, "%*sAST_CONSTANT: %s\n", depth, "", ivd._curr->_constant._value );
      break;
    }

    ivd._curr = ivd._curr->_next;
  }
}

struct ir_context*
new_ir_context( const char* name, struct ast_context* ast_ctx ) {
  return new ir_context( name, ast_ctx );
}

void
delete_ir_context( struct ir_context* ctx ) {
  delete ctx;
}

void
convert_ast_to_ir( struct ir_context* ctx ) {
  ir_visit_data ivd;
  ivd._ctx = ctx;
  ivd._curr = get_ast_start( ctx->_ast_ctx );
  ir_visitor( ivd );
  ctx->_module.print( errs(), nullptr );
}

