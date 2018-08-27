/*
	table-utils.cc
	--------------
*/

#include "vlib/table-utils.hh"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/equal.hh"
#include "vlib/throw.hh"
#include "vlib/iterators/array_iterator.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/iterators/list_iterator.hh"


namespace vlib
{
	
	static
	bool equal_keys( const Value& a, const Value& b )
	{
		if ( a.type() != b.type() )
		{
			return false;
		}
		
		switch ( a.type() )
		{
			case Value_boolean:
			case Value_byte:
			case Value_number:
			case Value_mb32:
			case Value_packed:
				break;
			
			case Value_string:
				return a.string() == b.string();
			
			default:
				THROW( "unsupported key type" );
		}
		
		return equal( a, b );
	}
	
	Value keyed_subscript( const Value& array, const Value& key )
	{
		array_iterator it( array );
		
		while ( it )
		{
			const Value& mapping = it.use();
			
			if ( Expr* expr = mapping.expr() )
			{
				if ( expr->op == Op_mapping )
				{
					if ( equal_keys( key, expr->left ) )
					{
						return expr->right;
					}
					
					continue;
				}
			}
			
			THROW( "non-mapping in table" );
		}
		
		THROW( "no such key in table" );
		return Value();
	}
	
	static
	Value sliced_subscript( const Value& table, const Value& key )
	{
		if ( is_empty_array( key ) )
		{
			return key;
		}
		
		list_builder results;
		
		if ( Expr* expr = key.expr() )
		{
			if ( expr->op == Op_array )
			{
				const Value& list = expr->right;
				
				list_iterator next_item( list );
				
				do
				{
					results.append( keyed_subscript( table, next_item.use() ) );
				}
				while ( next_item );
			}
		}
		
		return make_array( results );
	}
	
	Value associative_subscript( const Value& table, const Value& key )
	{
		if ( is_empty_array( key ) )
		{
			return key;
		}
		
		const Value& array = table.expr()->right;
		
		if ( is_array( key ) )
		{
			return sliced_subscript( array, key );
		}
		
		return keyed_subscript( array, key );
	}
	
	static
	Value& find_key_mutable( Value& list, const Value& key )
	{
		Value* it = &list;
		
		while ( true )
		{
			Value& next = first_mutable( *it );
			
			if ( equal_keys( next.expr()->left, key ) )
			{
				return next.unshare().expr()->right;
			}
			
			if ( it->listexpr() == 0 )
			{
				// Key not found; create a new mapping.
				
				*it = Value( *it, empty_list );
				
				return it->expr()->right;
			}
			
			it = &rest_mutable( *it );
		}
	}
	
	Value* get_table_subscript_addr( Expr* array_expr, const Value& key )
	{
		Value& array = array_expr->right.unshare();
		
		Value* it;
		
		if ( ! is_empty_array( array ) )
		{
			Value& list  = array.expr()->right;
			
			it = &find_key_mutable( list, key );
			
			if ( ! is_empty_list( *it ) )
			{
				return it;
			}
		}
		else
		{
			array = make_array( Value() );
			
			it = &array.expr()->right;
		}
		
		*it = Value( key, Op_mapping, Value() );
		
		return &it->expr()->right;
	}
	
}
