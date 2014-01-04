/*
	yield.hh
	--------
*/

#ifndef GENIE_API_YIELD_HH
#define GENIE_API_YIELD_HH


namespace Genie
{
	
	bool yield( bool may_throw );
	
	void try_again( bool nonblocking );
	
}

#endif

