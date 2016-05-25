/*
	throw.cc
	--------
*/

#include "vlib/throw.hh"


namespace vlib
{
	
	void throw_exception( const plus::string& msg )
	{
		throw exception( msg );
	}
	
}
