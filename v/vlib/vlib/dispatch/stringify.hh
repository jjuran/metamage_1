/*
	stringify.hh
	------------
*/

#ifndef VLIB_DISPATCH_STRINGIFY_HH
#define VLIB_DISPATCH_STRINGIFY_HH

// Standard C
#include <stddef.h>

// plus
#include "plus/string_fwd.hh"


namespace vlib
{
	
	class Value;
	
	typedef const char* (*string_data)( const Value& v );
	typedef size_t      (*string_size)( const Value& v );
	
	typedef char* (*string_copy)( char* p, const Value& v );
	
	typedef plus::string (*string_make)( const Value& v );
	
	/*
		Valid combinations:
		
			data (returning address of NUL-terminated buffer)
			data && size
			size && copy
			make
	*/
	
	class dynamic_data_exception {};
	
	/*
		A g++ bug prevents us from initializing const members with elided
		zero values:  "error: uninitialized const member"
		
		It's present in Debian's g++ 4.7.2-5 and absent in Apple's 4.0.1.
		TODO:  Limit workaround to affected versions.
	*/
	
	#ifdef __GNUC__
	#define CONST /**/
	#else
	#define CONST  const
	#endif
	
	struct stringify
	{
		const string_data data;  // gets address of fixed data
		const string_size size;  // gets size of data
		const string_copy copy;  // copies data
		CONST string_make make;  // computes data
	};
	
	#undef CONST
	
	struct stringifiers
	{
		const stringify* str;  // human-readable stringification
		const stringify* rep;  // reproductive format
		const stringify* bin;  // binary serialization format
	};
	
	size_t get_stringified_size( const stringify& methods, const Value& v );
	
	char* copy_stringified( char* p, const stringify& methods, const Value& v );
	
}

#endif
