/*
	subkeys.cc
	----------
*/

#include "des/subkeys.hh"


namespace des
{

void reverse_subkeys( uint64_t* out, const uint64_t* in )
{
	for ( int i = 0;  i < 16;  ++i )
	{
		out[ 15 - i ] = in[ i ];
	}
}

}  // namespace des
