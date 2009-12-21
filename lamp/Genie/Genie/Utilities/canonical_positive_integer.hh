/*
	canonical_positive_integer.hh
	-----------------------------
	
	Copyright 2009, Joshua Juran
*/

#ifndef GENIE_UTILITIES_CANINICALPOSITIVEINTEGER_HH
#define GENIE_UTILITIES_CANINICALPOSITIVEINTEGER_HH

// Standard C++
#include <string>


namespace Genie
{
	
	struct canonical_positive_integer
	{
		static bool applies( const std::string& name );
	};
	
}

#endif

