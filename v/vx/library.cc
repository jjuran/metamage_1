/*
	library.cc
	----------
*/

#include "library.hh"

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>
#include <time.h>

// must
#include "must/write.h"

// plus
#include "plus/var_string.hh"

// vlib
#include "vlib/string-utils.hh"
#include "vlib/types.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	static
	Value v_getenv( const Value& v )
	{
		if ( const char* var = getenv( get_str( v ).c_str() ) )
		{
			return var;
		}
		
		return Value_empty_list;
	}
	
	static
	Value v_print( const Value& v )
	{
		plus::var_string s = make_string( v, Stringified_to_print ).move();
		
		s += "\n";
		
		must_write( STDOUT_FILENO, s.data(), s.size() );
		
		return Value();
	}
	
	static
	Value v_time( const Value& v )
	{
		const time_t t = time( NULL );
		
		return t;
	}
	
	static const Value c_str = c_str_vtype;
	
	const proc_info proc_getenv = { &v_getenv, "getenv", &c_str      };
	const proc_info proc_print  = { &v_print,  "print",  NULL        };
	const proc_info proc_time   = { &v_time,   "time",   &empty_list };
	
}
