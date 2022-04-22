/*
	print.hh
	--------
*/

#ifndef V68K_PRINT_HH
#define V68K_PRINT_HH

// log-of-war
#include "logofwar/print.hh"

// v68k
#include "v68k/memory.hh"


namespace v68k
{
	
	using ::logofwar::hex32_t;
	
	extern char const* const function_code_names[];
	extern char const* const mem_access_names[];
	
	inline
	void print( fc_t x )
	{
		logofwar::print( function_code_names[ x ] );
	}
	
	inline
	void print( mem_t x )
	{
		logofwar::print( mem_access_names[ x ] );
	}
	
}

#endif
