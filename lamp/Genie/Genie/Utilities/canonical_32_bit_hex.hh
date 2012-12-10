/*
	canonical_32_bit_hex.hh
	-----------------------
*/

#ifndef GENIE_UTILITIES_CANINICAL32BITHEX_HH
#define GENIE_UTILITIES_CANINICAL32BITHEX_HH

// plus
#include "plus/string_fwd.hh"


namespace Genie
{
	
	struct canonical_32_bit_hex
	{
		static bool applies( const plus::string& name );
	};
	
}

#endif

