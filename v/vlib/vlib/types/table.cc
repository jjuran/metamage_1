/*
	table.cc
	--------
*/

#include "vlib/types/table.hh"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/assign.hh"
#include "vlib/equal.hh"
#include "vlib/list-utils.hh"
#include "vlib/table-utils.hh"
#include "vlib/throw.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/verity.hh"
#include "vlib/iterators/array_iterator.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/types/any.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/type.hh"


namespace vlib
{
	
	static const Type etc = etc_vtype;
	
	static
	bool table_truth( const Value& v )
	{
		// Check if the array is empty.
		return Boolean( v.expr()->right.expr() != 0 );  // NULL
	}
	
	static const veritization table_veritization =
	{
		&table_truth,
	};
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				return Value( v.expr()->left, Op_empower, etc );
			
			case Op_unary_minus:
			{
				Expr* expr = v.expr();
				
				const Value& old_list = expr->right.expr()->right;
				
				const Value new_list  = reverse_list( old_list );
				const Value new_array = make_array  ( new_list );
				
				return Value( expr->left, expr->op, new_array );
			}
			
			default:
				break;
		}
		
		return Value();
	}
	
	static
	Value table_member( const Value& table, const plus::string& name )
	{
		const Value& array = table.expr()->right;
		
		if ( name == "length" )
		{
			if ( is_empty_array( array ) )
			{
				return Integer();
			}
			
			Expr* expr = array.expr();
			
			return Integer( count( expr->right ) );
		}
		
		if ( name == "keys"  ||  name == "values" )
		{
			if ( is_empty_array( array ) )
			{
				return array;
			}
			
			const bool keys = name == "keys";
			
			list_builder results;
			
			array_iterator it( array );
			
			while ( it )
			{
				const Value& mapping = it.use();
				
				Expr* expr = mapping.expr();
				
				results.append( keys ? expr->left : expr->right );
			}
			
			return make_array( results );
		}
		
		THROW( "nonexistent table member" );
		
		return Value();
	}
	
	static
	bool in_array_mapping_keys( const Value& v, const Value& array )
	{
		array_iterator it( array );
		
		while ( it )
		{
			const Value& mapping = it.use();
			const Value& key = mapping.expr()->left;
			
			if ( equal( key, v ) )
			{
				return true;
			}
		}
		
		return false;
	}
	
	static
	Value binary_op_handler( op_type op, const Value& a, const Value& b )
	{
		switch ( op )
		{
			case Op_contains:
				return Boolean( in_array_mapping_keys( b, a.expr()->right ) );
			
			case Op_member:
				return table_member( a, b.string() );
			
			case Op_subscript:
				return associative_subscript( a, b );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static
	void push_elements( Value& table, const Value& new_elements )
	{
		if ( is_empty_list( new_elements ) )
		{
			return;
		}
		
		const Value& key_type = table.expr()->left;
		
		const Value mapping_type( key_type,     Op_mapping,   etc );
		const Value array_type  ( mapping_type, Op_subscript, etc );
		
		Value& array = table.unshare().expr()->right;
		
		const Target target = { &array, &array_type };
		
		push( target, new_elements );
	}
	
	static
	Value mutating_op_handler( op_type        op,
	                           const Target&  target,
	                           const Value&   x,
	                           const Value&   b )
	{
		switch ( op )
		{
			case Op_push:
				push_elements( *target.addr, b );
				return nothing;
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
		&binary_op_handler,
		0,  // NULL
		&mutating_op_handler,
	};
	
	const dispatch table_dispatch =
	{
		0,  // NULL
		&table_veritization,
		0,  // NULL
		&ops,
	};
	
	static
	void check_table( const Value& key_type, const Value& array )
	{
		array_iterator it( array );
		
		while ( it )
		{
			const Value& mapping = it.use();
			
			if ( Expr* expr = mapping.expr() )
			{
				if ( expr->op == Op_mapping )
				{
					if ( as_assigned( key_type, expr->left ).type() )
					{
						continue;
					}
					
					THROW( "non-conforming key type in table" );
				}
			}
			
			THROW( "non-mapping in table" );
		}
	}
	
	Table::Table( const Value& key_type, const Value& array )
	:
		Value( key_type, Op_empower, array, &table_dispatch )
	{
		check_table( key_type, array );
	}
	
}
