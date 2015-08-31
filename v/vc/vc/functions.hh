/*
	functions.hh
	------------
*/

#ifndef VC_FUNCTIONS_HH
#define VC_FUNCTIONS_HH

// plus
#include "plus/string.hh"

// vc
#include "vc/function_id.hh"


namespace vc
{
	
	function_id function_from_name( const plus::string& name );
	
}

#endif
