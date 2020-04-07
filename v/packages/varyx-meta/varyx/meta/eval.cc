/*
	eval.cc
	-------
*/

#include "varyx/meta/eval.hh"

// debug
#include "debug/assert.hh"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/exceptions.hh"
#include "vlib/interpret.hh"
#include "vlib/symbol.hh"
#include "vlib/types.hh"
#include "vlib/iterators/array_iterator.hh"
#include "vlib/iterators/safe_list_builder.hh"
#include "vlib/proc_info.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"
#include "vlib/value.hh"


#ifndef NULL
#define NULL 0L
#endif


namespace varyx
{
namespace meta
{

using namespace vlib;

static
void eval_error_handler( const plus::string& msg, const source_spec& src )
{
	throw user_exception( String( msg ), src );
}

static
Value v_eval( const Value& v )
{
	Expr* expr = v.expr();
	
	const char* code = expr->left .string().c_str();
	const char* file = expr->right.string().c_str();
	
	const Value result = interpret( code, file, NULL, &eval_error_handler );
	
	if ( Expr* expr = result.expr() )
	{
		if ( expr->op == Op_export )
		{
			const Value& exported = expr->right;
			
			if ( const Symbol* sym = exported.sym() )
			{
				return sym->get();
			}
			
			safe_list_builder  mappings;
			array_iterator     it( exported );
			
			while ( it )
			{
				const Symbol* sym = it.use().sym();
				
				ASSERT( sym );
				
				mappings.append( mapping( sym->name(), sym->get() ) );
			}
			
			return Value( Op_module, make_array( mappings ) );
		}
	}
	
	return result;
}

static const Type c_str = c_str_vtype;

static const Value cstr_eval( c_str,
                              Op_duplicate,
                              String( "<eval>" ) );

static const Value eval( c_str, cstr_eval );

const proc_info proc_eval = { "eval", &v_eval, &eval };

}
}
