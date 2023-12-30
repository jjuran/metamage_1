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


#define LENGTH(array)  (sizeof (array) / sizeof *(array))

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
	
	static const proc_info* procs[] =
	{
		&proc_It_max,
		&proc_It_min,
		&proc_popcount,
		&proc_sorted,
	};
	
	static
	const proc_info* find_proc( const plus::string& name )
	{
		for ( int i = 0;  i < LENGTH( procs );  ++i )
		{
			const proc_info* proc = procs[ i ];
			
			if ( proc->name == name )
			{
				return proc;
			}
		}
		
		return NULL;
	}
	
	static
	Value getter( const plus::string& name )
	{
		if ( const proc_info* proc = find_proc( name ) )
		{
			return Proc( *proc );
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
