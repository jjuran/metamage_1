/*
	proc.cc
	-------
*/

#include "vlib/types/proc.hh"

// debug
#include "debug/assert.hh"

// vlib
#include "vlib/symbol.hh"
#include "vlib/throw.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/verity.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/any.hh"
#include "vlib/types/builtin.hh"


namespace vlib
{
	
	static
	bool always_true( const Value& v )
	{
		return true;
	}
	
	static const veritization proc_veritization =
	{
		&always_true,
	};
	
	static
	bool is_etc( const Value& type )
	{
		if ( type.type() == Value_base_type )
		{
			const type_info& typeinfo = type.typeinfo();
			
			return &typeinfo == &etc_vtype;
		}
		
		return false;
	}
	
	static
	Value as_assigned_or_default( const Value& type, const Value& v )
	{
		if ( Expr* expr = type.expr() )
		{
			if ( expr->op == Op_duplicate )
			{
				if ( is_empty_list( v ) )
				{
					ASSERT( as_assigned( expr->left, expr->right ).type() );
					
					return expr->right;
				}
				
				return as_assigned( expr->left, v );
			}
		}
		
		return as_assigned( type, v );
	}
	
	static
	Value apply_prototype( const Value& prototype, const Value& arguments )
	{
		list_builder result;
		
		list_iterator defs( prototype );
		list_iterator args( arguments );
		
		while ( defs )
		{
			const Value& type = defs.use();
			
			if ( is_etc( type ) )
			{
				if ( defs )
				{
					THROW( "`...` must be last in a prototype" );
				}
				
				result.append( args.rest() );
				
				return result;
			}
			
			const Value& arg = args.use();
			
			const Value r = as_assigned_or_default( type, arg );
			
			if ( r.type() == Value_NIL )
			{
				THROW( "arguments don't match function prototype" );
			}
			
			result.append( r );
		}
		
		if ( args )
		{
			THROW( "too many arguments" );
		}
		
		return result;
	}
	
	static
	Value call_function( const Value& f, const Value& arguments )
	{
		const proc_info& proc = f.dereference< proc_info >();
		
		if ( const Value* prototype = proc.prototype )
		{
			const Value args = apply_prototype( *prototype, arguments );
			
			return proc.addr( args );
		}
		
		return proc.addr( arguments );
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		switch ( op )
		{
			case Op_function:
			case Op_named_unary:
				return call_function( a, b );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		0,  // NULL
		&binary_op_handler,
	};
	
	const dispatch proc_dispatch =
	{
		&builtin_stringifiers,
		&proc_veritization,
		&builtin_comparison,
		&ops,
	};
	
}
