/*
	assert.cc
	---------
*/

#include "vlib/assert.hh"

// plus
#include "plus/var_string.hh"

// vlib
#include "vlib/exceptions.hh"
#include "vlib/string-utils.hh"
#include "vlib/types/boolean.hh"


namespace vlib
{
	
	static
	language_error assertion_result_not_boolean( const source_spec& source )
	{
		return language_error( "assertion result not boolean", source );
	}
	
	void check_assertion_result( const Value&        test,
	                             const Value&        v,
	                             const source_spec&  source )
	{
		bool passed;
		
		if ( const Boolean* value = v.is< Boolean >() )
		{
			passed = *value;
		}
		else
		{
			throw assertion_result_not_boolean( source );
		}
		
		if ( ! passed )
		{
			plus::var_string s = "assertion failed: ";
			
			s += rep( test );
			
			throw language_error( s, source );
		}
	}
	
}
