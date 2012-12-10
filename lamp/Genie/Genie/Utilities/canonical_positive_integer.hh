/*
	canonical_positive_integer.hh
	-----------------------------
*/

#ifndef GENIE_UTILITIES_CANINICALPOSITIVEINTEGER_HH
#define GENIE_UTILITIES_CANINICALPOSITIVEINTEGER_HH

// plus
#include "plus/string_fwd.hh"


namespace Genie
{
	
	struct canonical_positive_integer
	{
		static bool applies( const plus::string& name );
	};
	
}

#endif

