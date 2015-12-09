/*
	table-utils.cc
	--------------
*/

#include "vlib/table-utils.hh"

// plus
#include "plus/integer.hh"

// vlib
#include "vlib/error.hh"
#include "vlib/list-utils.hh"
#include "vlib/symbol.hh"
#include "vlib/throw.hh"
#include "vlib/iterators/array_iterator.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/byte.hh"


namespace vlib
{
	
	Value make_table( const Value& key_type, const Value& array )
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
		
		return Value( key_type, Op_empower, array );
	}
	
	bool equal_keys( const Value& a, const Value& b )
	{
		if ( a.type() != b.type() )
		{
			return false;
		}
		
		switch ( a.type() )
		{
			case Value_boolean:
				return a.boolean() == b.boolean();
			
			case Value_byte:
			case Value_number:
				return a.number() == b.number();
			
			case Value_data:
			case Value_string:
				return a.string() == b.string();
			
			case Value_function:
				return &a.proc() == &b.proc();
			
			case Value_base_type:
				return &a.typeinfo() == &b.typeinfo();
			
			case Value_pair:
				{
					Expr& ax = *a.expr();
					Expr& bx = *b.expr();
					
					if ( ax.op != bx.op )
					{
						return false;
					}
					
					const bool compare_left = (ax.op & 0xFF) != Op_scope;
					
					if ( compare_left  &&  ! equal_keys( ax.left, bx.left ) )
					{
						return false;
					}
					
					return equal_keys( ax.right, bx.right );
				}
			
			default:
				INTERNAL_ERROR( "unsupported type in equal_keys()" );
				break;
		}
		
		return false;
	}
	
	static
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
