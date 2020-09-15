/*
	dyad.cc
	-------
*/

#include "vlib/dyad.hh"

// iota
#include "iota/swap.hh"


namespace vlib
{
	
	void dyad::swap( dyad& b )
	{
		using iota::swap;
		
		swap( v,  b.v  );
		swap( op, b.op );
	}
	
}
