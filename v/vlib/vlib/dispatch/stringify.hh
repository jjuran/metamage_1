/*
	stringify.hh
	------------
*/

#ifndef VLIB_DISPATCH_STRINGIFY_HH
#define VLIB_DISPATCH_STRINGIFY_HH

// Standard C
#include <stddef.h>


namespace vlib
{
	
	class Value;
	
	typedef const char* (*string_data)( const Value& v );
	typedef size_t      (*string_size)( const Value& v );
	
	typedef char* (*string_copy)( char* p, const Value& v );
	
	/*
		Valid combinations:
		
			data (returning address of NUL-terminated buffer)
			data && size
			size && copy
	*/
	
	struct stringify
	{
		const string_data data;  // gets address of fixed data
		const string_size size;  // gets size of data
		const string_copy copy;  // copies data
	};
	
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
