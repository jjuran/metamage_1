/*
	library.cc
	----------
*/

#include "vc/library.hh"

// Standard C
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
	
	Value v_time( const Value& v )
	{
		if ( v.type != Value_empty_list )
		{
			TYPE_ERROR( "time takes no arguments" );
		}
		
		const time_t t = time( NULL );
		
		return plus::integer( t );
	}
	
}
