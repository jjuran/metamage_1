/*
	library.cc
	----------
*/

#include "vlib/library.hh"

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>
#include <time.h>

// must
#include "must/write.h"

// plus
#include "plus/string/concat.hh"

// vlib
#include "vlib/error.hh"
#include "vlib/string-utils.hh"


namespace vlib
{
	
	Value v_getenv( const Value& v )
	{
		if ( v.type != Value_string )
		{
			TYPE_ERROR( "getenv() argument must be a string" );
		}
		
		if ( strlen( v.string.c_str() ) != v.string.size() )
		{
			TYPE_ERROR( "getenv() argument must not contain NUL bytes" );
		}
		
		if ( const char* var = getenv( v.string.c_str() ) )
		{
			return var;
		}
		
		return Value_empty_list;
	}
	
	Value v_print( const Value& v )
	{
		const plus::string s = make_string( v, Stringified_to_print ) + "\n";
		
		must_write( STDOUT_FILENO, s.data(), s.size() );
		
		return Value();
	}
	
	Value v_time( const Value& v )
	{
		if ( ! is_empty( v ) )
		{
			TYPE_ERROR( "time takes no arguments" );
		}
		
		const time_t t = time( NULL );
		
		return t;
	}
	
}
