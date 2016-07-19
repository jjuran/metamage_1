/*
	array-utils.cc
	--------------
*/

#include "vlib/array-utils.hh"

// plus
#include "plus/integer.hh"

// vlib
#include "vlib/list-utils.hh"
#include "vlib/symbol.hh"
#include "vlib/throw.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/types/byte.hh"


namespace vlib
{
	
	static
	unsigned subscript_integer( const Value& index )
	{
		if ( index.type() != Value_number )
		{
			THROW( "non-integer array subscript" );
		}
		
		const plus::integer& i = index.number();
		
		if ( i.is_negative() )
		{
			THROW( "negative array subscript" );
		}
		
		if ( i > 0xFFFFFFFFu )
		{
			THROW( "Array subscript is too large" );
		}
		
		return i.clipped();
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
		else if ( array.type() != V_str  &&  array.type() != V_data )
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
			else if ( expr->op == Op_gamut  ||  expr->op == Op_delta )
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
			
			if ( expr->op == Op_gamut  ||  expr->op == Op_delta )
			{
				return sliced_subscript( array, index );
			}
		}
		
		THROW( "unsupported subscript type" );
		
		return Value();
	}
	
	void push( const Value& array_target, const Value& list )
	{
		if ( ! is_symbol( array_target ) )
		{
			THROW( "push() destination must be a symbol" );
		}
		
		Symbol& sym = *array_target.sym();
		
		Value& array = sym.deref();
		
		if ( is_empty_array( array ) )
		{
			array = make_array( list );
			return;
		}
		
		Expr* expr = array.expr();
		
		if ( expr == NULL  ||  expr->op != Op_array )
		{
			THROW( "push() destination must be an array" );
		}
		
		if ( sym.vtype().type() )
		{
			Value assigned = as_assigned( sym.vtype(), make_array( list ) );
			
			if ( assigned.type() == Value_nothing )
			{
				THROW( "push() elements are the wrong type" );
			}
		}
		
		splice_lists( expr->right, list );
	}
	
}
