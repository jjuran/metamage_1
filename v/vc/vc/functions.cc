/*
	functions.cc
	------------
*/

#include "vc/functions.hh"


#define ARRAY_LEN( a ) (sizeof (a) / sizeof (a)[0])
#define ARRAY_END( a ) ((a) + ARRAY_LEN(a))


namespace vc
{
	
	struct function_mapping
	{
		const char*  name;
		function_id  f;
	};
	
	static
	const function_mapping functions[] =
	{
		{ "abs",  Function_abs  },
		{ "half", Function_half },
	};
	
	static
	const function_mapping* find( const plus::string& name )
	{
		const function_mapping* begin = functions;
		const function_mapping* end   = ARRAY_END( functions );
		
		const function_mapping* it = begin;
		
		while ( it < end )
		{
			if ( it->name == name )
			{
				return it;
			}
			
			++it;
		}
		
		return 0;  // NULL
	}
	
	function_id function_from_name( const plus::string& name )
	{
		if ( const function_mapping* it = find( name ) )
		{
			return it->f;
		}
		
		return Function_none;
	}
	
}
