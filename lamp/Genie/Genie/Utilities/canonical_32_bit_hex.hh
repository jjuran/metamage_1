/*
	canonical_32_bit_hex.hh
	-----------------------
	
	Copyright 2009, Joshua Juran
*/

#ifndef GENIE_UTILITIES_CANINICAL32BITHEX_HH
#define GENIE_UTILITIES_CANINICAL32BITHEX_HH


namespace plus
{
	
	class string;
	
}

namespace Genie
{
	
	struct canonical_32_bit_hex
	{
		static bool applies( const plus::string& name );
	};
	
}

#endif

