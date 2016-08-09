/*
	exception.cc
	------------
*/

#include "exception.hh"

// vlib
#include "vlib/exceptions.hh"


namespace vlib
{
	
	void throw_exception_object( const Value& mappings )
	{
		const Value object( Op_module, make_array( mappings ) );
		
		throw user_exception( object, source_spec() );
	}
	
}
