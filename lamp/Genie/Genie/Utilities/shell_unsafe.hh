/*
	shell_unsafe.hh
	---------------
*/

#ifndef GENIE_UTILITIES_SHELLUNSAFE_HH
#define GENIE_UTILITIES_SHELLUNSAFE_HH

// plus
#include "plus/string.hh"


namespace Genie
{
	
	long shell_unsafe_index( const plus::string& s );
	
	plus::string safe_prefix( const plus::string& s );
	
}

#endif
