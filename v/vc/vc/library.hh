/*
	library.hh
	----------
*/

#ifndef VC_LIBRARY_HH
#define VC_LIBRARY_HH

// vc
#include "vc/value.hh"


namespace vc
{
	
	void define( const char* name, function_type f );
	
	Value v_getenv( const Value& );
	Value v_time  ( const Value& );
	
}

#endif
