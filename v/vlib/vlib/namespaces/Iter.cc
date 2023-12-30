/*
	Iter.cc
	-------
*/

#include "vlib/namespaces/Iter.hh"

// vlib
#include "vlib/namespace_info.hh"
#include "vlib/order.hh"
#include "vlib/throw.hh"
#include "vlib/iterators/generic_iterator.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/namespace.hh"
#include "vlib/types/proc.hh"


#define QUINE( name )  #name, &name


namespace vlib
{
	
	static
	Value popcount( const Value& container )
	{
		generic_iterator it( container );
		
		long n = 0;
		
		while ( it )
		{
			n += it.use().to< Boolean >();
		}
		
		return Integer( n );
	}
	
	static const proc_info proc_popcount = { QUINE(popcount), NULL, Proc_pure };
	
	static
	Value getter( const plus::string& name )
	{
		if ( name == "max" )
		{
			return Proc( proc_It_max );
		}
		
		if ( name == "min" )
		{
			return Proc( proc_It_min );
		}
		
		if ( name == "popcount" )
		{
			return Proc( proc_popcount );
		}
		
		if ( name == "sorted" )
		{
			return Proc( proc_sorted );
		}
		
		THROW( "nonexistent member of namespace Iter" );
		
		return Value();
	}
	
	const namespace_info namespace_Iter =
	{
		"Iter",
		&getter,
	};
	
}
