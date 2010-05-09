/*
	canonical_positive_integer.hh
	-----------------------------
	
	Copyright 2009, Joshua Juran
*/

#ifndef GENIE_UTILITIES_CANINICALPOSITIVEINTEGER_HH
#define GENIE_UTILITIES_CANINICALPOSITIVEINTEGER_HH


namespace plus
{
	
	class string;
	
}

namespace Genie
{
	
	struct canonical_positive_integer
	{
		static bool applies( const plus::string& name );
	};
	
}

#endif

