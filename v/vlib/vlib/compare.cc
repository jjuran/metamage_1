/*
	compare.cc
	----------
*/

#include "vlib/compare.hh"

// vlib
#include "vlib/throw.hh"
#include "vlib/value.hh"
#include "vlib/dispatch/compare.hh"
#include "vlib/dispatch/dispatch.hh"


namespace vlib
{
	
	using math::integer::cmp_t;
	
	
	cmp_t compare( const Value& a, const Value& b )
	{
		if ( a.type() != b.type() )
		{
			THROW( "mismatched types in compare()" );
		}
		
		if ( a.dispatch_methods() != b.dispatch_methods() )
		{
			THROW( "mismatched types in compare()" );
		}
		
		if ( const dispatch* methods = a.dispatch_methods() )
		{
			if ( const comparison* cmp = methods->compare )
			{
				if ( const ordering ord = cmp->order )
				{
					return ord( a, b );
				}
			}
		}
		
		THROW( "unsupported type in compare()" );
		
		return 0;
	}
	
}
