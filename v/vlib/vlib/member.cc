/*
	member.cc
	---------
*/

#include "vlib/member.hh"

// debug
#include "debug/assert.hh"

// plus
#include "plus/var_string.hh"

// vlib
#include "vlib/exceptions.hh"
#include "vlib/list-utils.hh"
#include "vlib/throw.hh"
#include "vlib/table-utils.hh"
#include "vlib/types.hh"
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/record.hh"
#include "vlib/types/string.hh"


namespace vlib
{
	
	static
	Value array_member( const Value& array, const plus::string& name )
	{
		if ( is_empty_array( array ) )
		{
			if ( name == "length" )
			{
				return Integer();
			}
			
			THROW( "nonexistent array member" );
		}
		
		Expr* expr = array.expr();
		
		ASSERT( expr != NULL );
		ASSERT( expr->op == Op_array );
		
		if ( name == "length" )
		{
			return Integer( count( expr->right ) );
		}
		
		THROW( "nonexistent array member" );
		
		return Value();
	}
	
	static
	Value mapping_member( const Value& mapping, const plus::string& name )
	{
		Expr* expr = mapping.expr();
		
		ASSERT( expr != NULL );
		ASSERT( expr->op == Op_mapping );
		
		if ( name == "key" )
		{
			return expr->left;
		}
		
		if ( name == "value" )
		{
			return expr->right;
		}
		
		THROW( "nonexistent mapping member" );
		
		return Value();
	}
	
	Value calc_member( const Value& left, const Value& right )
	{
		if ( right.type() != Value_string )
		{
			THROW( "non-string member name" );
		}
		
		if ( const dispatch* methods = left.dispatch_methods() )
		{
			if ( const operators* ops = methods->ops )
			{
				if ( handler_2arg handler = ops->binary )
				{
					const Value result = handler( Op_member, left, right );
					
					if ( result )
					{
						return result;
					}
				}
			}
		}
		
		Value vtype = proc_typeof.addr( left );
		
		if ( Expr* expr = vtype.expr() )
		{
			if ( expr->op == Op_subscript )
			{
				return array_member( left, right.string() );
			}
			
			if ( expr->op == Op_mapping )
			{
				return mapping_member( left, right.string() );
			}
			
			if ( expr->op == Op_module )
			{
				// Pretend the module is a table.
				return associative_subscript( left, right );
			}
		}
		
		if ( vtype.type() == Value_base_type )
		{
			const type_info& typeinfo = vtype.typeinfo();
			
			if ( member_proc member_hook = typeinfo.member )
			{
				if ( const Value member = member_hook( left, right.string() ) )
				{
					return member;
				}
				
				Value desc = mapping( "desc", String( "no such field" ) );
				Value name = mapping( "name", right );
				Value type = mapping( "type", String( typeinfo.name ) );
				
				Value exception = Record( Value( desc, Value( name, type ) ) );
				
				throw user_exception( exception, source_spec() );
			}
		}
		
		THROW( "unsupported type for member access" );
		
		return Value();
	}
	
}
