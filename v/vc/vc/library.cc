/*
	library.cc
	----------
*/

#include "vc/library.hh"

// Standard C
#include <stdlib.h>
#include <time.h>

// vc
#include "vc/error.hh"
#include "vc/symbol_table.hh"


namespace vc
{
	
	void define( const char* name, function_type f )
	{
		const symbol_id sym = create_symbol( name, Symbol_const );
		
		assign_symbol( sym, Value( f, name ) );
	}
	
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
	
	Value v_time( const Value& v )
	{
		if ( v.type != Value_empty_list )
		{
			TYPE_ERROR( "time takes no arguments" );
		}
		
		const time_t t = time( NULL );
		
		return t;
	}
	
}
