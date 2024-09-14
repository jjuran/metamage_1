/*
	Math.cc
	-------
*/

#include "vlib/namespaces/Math.hh"

// vlib
#include "vlib/functions.hh"
#include "vlib/lib/math.hh"
#include "vlib/namespace_info.hh"
#include "vlib/reduce.hh"
#include "vlib/throw.hh"
#include "vlib/value.hh"
#include "vlib/types/namespace.hh"
#include "vlib/types/proc.hh"


#define LENGTH(array)  (sizeof (array) / sizeof *(array))

#ifndef NULL
#define NULL  0L
#endif


namespace vlib
{
	
	static const proc_info* procs[] =
	{
		&proc_half,
		&proc_max,
		&proc_min,
		&proc_product,
		&proc_sincospi,
		&proc_sum,
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
		
		THROW( "nonexistent member of namespace Math" );
		
		return Value();
	}
	
	const namespace_info namespace_Math =
	{
		"Math",
		&getter,
	};
	
}
