/*
	array-utils.cc
	--------------
*/

#include "vlib/array-utils.hh"

// bignum
#include "bignum/integer.hh"

// vlib
#include "vlib/assign.hh"
#include "vlib/list-utils.hh"
#include "vlib/throw.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/byte.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/range.hh"
#include "vlib/types/string.hh"


namespace vlib
{
	
	unsigned subscript_integer( const Value& index )
	{
		return integer_cast< unsigned >( index );
	}
	
	static
	Value get_nth_element( const Value& array, unsigned i )
	{
		if ( Expr* expr = array.expr() )
		{
			if ( expr->op == Op_array )
			{
				return get_nth( expr->right, i );
			}
			
			THROW( "type not subscriptable" );
		}
		else if ( array.type() != V_str  &&  array.type() != V_pack )
		{
			THROW( "type not subscriptable" );
		}
		
		const plus::string& s = array.string();
		
		if ( i >= s.size() )
		{
			return Value_empty_list;
		}
		
		return Byte( s[ i ] );
	}
	
	static
	Value indexed_subscript( const Value& array, unsigned i )
	{
		const Value nth = get_nth_element( array, i );
		
		if ( is_empty_list( nth ) )
		{
			THROW( "subscript exceeds array bounds" );
		}
		
		return nth;
	}
	
	static
	Value sliced_subscript( const Value& array, const Value& index )
	{
		if ( is_empty_array( index ) )
		{
			return index;
		}
		
		list_builder results;
		
		if ( Expr* expr = index.expr() )
		{
			if ( expr->op == Op_array )
			{
				const Value& list = expr->right;
				
				list_iterator next_item( list );
				
				do
				{
					const unsigned i = subscript_integer( next_item.use() );
					
					results.append( indexed_subscript( array, i ) );
				}
				while ( next_item );
			}
			else if ( Range::test( index ) )
			{
				const unsigned a = subscript_integer( expr->left  );
				const unsigned b = subscript_integer( expr->right );
				
				unsigned i = a;
				
				while ( i < b  ||  (expr->op == Op_gamut  &&  i == b) )
				{
					results.append( indexed_subscript( array, i++ ) );
				}
			}
		}
		
		return make_array( results );
	}
	
	Value linear_subscript( const Value& array, const Value& index )
	{
		if ( index.type() == Value_number )
		{
			const unsigned i = subscript_integer( index );
			
			return indexed_subscript( array, i );
		}
		
		if ( is_empty_array( index ) )
		{
			return sliced_subscript( array, index );
		}
		
		if ( Expr* expr = index.expr() )
		{
			if ( expr->op == Op_array )
			{
				return sliced_subscript( array, index );
			}
			
			if ( Range::test( index ) )
			{
				if ( array.type() == V_str  ||  array.type() == V_pack )
				{
					const unsigned a = subscript_integer( expr->left  );
					const unsigned b = subscript_integer( expr->right );
					
					if ( a > b )
					{
						THROW( "descending range in subscript" );
					}
					
					if ( a < b )
					{
						const plus::string& s = array.string();
						
						plus::string::size_type size = s.size();
						
						if ( size == 0 )
						{
							THROW( "range subscript of empty string/pack" );
						}
						
						const bool is_gamut = expr->op == Op_gamut;
						
						if ( b > size - is_gamut )
						{
							THROW( "range subscript overruns string/pack" );
						}
						
						return String( s.substr( a, b - a + is_gamut ) );
					}
				}
				
				return sliced_subscript( array, index );
			}
		}
		
		THROW( "unsupported subscript type" );
		
		return Value();
	}
	
	static inline
	const Value& array_subtype( const Expr* type_expr )
	{
		return type_expr->left;
	}
	
	void get_array_index_type( const Value& array_type, const Value*& base_type )
	{
		if ( array_type )
		{
			Expr* expr = array_type.expr();
			
			if ( expr != NULL  &&  expr->op == Op_subscript )
			{
				base_type = &array_subtype( expr );
			}
		}
	}
	
	Value* get_array_subscript_addr( Expr* array_expr, const Value& index )
	{
		Value& list = array_expr->right;
		
		return &get_nth_mutable( list, subscript_integer( index ) );
	}
	
	void push( const Target& target, const Value& list )
	{
		Value&       array = *target.addr;
		Value const& type  = *target.type;
		
		if ( is_empty_array( array ) )
		{
			array = make_array( list );
			return;
		}
		
		Expr* expr = array.unshare().expr();
		
		if ( expr == NULL  ||  expr->op != Op_array )
		{
			THROW( "push() destination must be an array" );
		}
		
		if ( type )
		{
			if ( ! as_assigned( type, make_array( list ) ) )
			{
				THROW( "push() elements are the wrong type" );
			}
		}
		
		splice_lists( expr->right, list );
	}
	
}
