/*
	generic_iterator.cc
	-------------------
*/

#include "vlib/iterators/generic_iterator.hh"

// vlib
#include "vlib/throw.hh"
#include "vlib/iterators/byterange_iterator.hh"
#include "vlib/iterators/iterator_template.hh"
#include "vlib/iterators/list_iterator.hh"
#include "vlib/iterators/range_iterator.hh"
#include "vlib/iterators/string_iterator.hh"
#include "vlib/types/byterange.hh"


namespace vlib
{
	
	template < class Iterator >
	static inline
	iterator_base* new_iterator( const Value& container )
	{
		return new iterator_template< Iterator >( container );
	}
	
	generic_iterator::generic_iterator( const Value& container )
	{
		if ( is_empty_array( container ) )
		{
			its_impl = new_iterator< list_iterator >( empty_list );
			return;
		}
		
		switch ( container.type() )
		{
			case V_str:
			case V_pack:
				its_impl = new_iterator< string_iterator >( container );
				return;
			
			default:
				break;
		}
		
		if ( container.is< ByteRange >() )
		{
			its_impl = new_iterator< byterange_iterator >( container );
			return;
		}
		
		if ( Expr* expr = container.expr() )
		{
			switch ( expr->op )
			{
				case Op_empower:
					if ( is_empty_array( expr->right ) )
					{
						its_impl = new_iterator< list_iterator >( empty_list );
						return;
					}
					
					expr = expr->right.expr();
					// fall through
				
				case Op_array:
					its_impl = new_iterator< list_iterator >( expr->right );
					return;
				
				case Op_gamut:
				case Op_delta:
					its_impl = new_iterator< range_iterator >( container );
					return;
				
				default:
					break;
			}
		}
		
		THROW( "not an iterable container" );
	}
	
	generic_iterator::~generic_iterator()
	{
		delete its_impl;
	}
	
}
