/*
	throw.hh
	--------
*/

#ifndef VLIB_THROW_HH
#define VLIB_THROW_HH

// iota
#include "iota/class.hh"

// plus
#include "plus/string.hh"

	
namespace vlib
{
	
	#define THROW( msg ) throw_exception( msg )
	
	struct exception
	{
		const plus::string  message;
		
		exception( const plus::string& msg )
		:
			message( msg )
		{
		}
		
		NO_NEW_DELETE
	};
	
	void throw_exception( const plus::string&  msg );
	void throw_exception( const char*          msg );
	
}

#endif
