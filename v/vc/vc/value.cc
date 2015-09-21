/*
	value.cc
	--------
*/

#include "vc/value.hh"

// iota
#include "iota/swap.hh"


namespace vc
{
	
	void swap( Value& a, Value& b )
	{
		using iota::swap;
		
		swap( a.type,   b.type   );
		swap( a.number, b.number );
	}
	
}
