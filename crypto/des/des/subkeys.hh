/*
	subkeys.hh
	----------
*/

#ifndef DES_SUBKEYS_HH
#define DES_SUBKEYS_HH

// Standard C
#include <stdint.h>

// des
#include "des/common.hh"


namespace des
{
	
	void reverse_subkeys( uint64_t* out, const uint64_t* in );
	
	inline
	void make_subkeys_straight( uint64_t* subkeys, const uint64_t& key )
	{
		make_subkeys( subkeys, key );
	}
	
	inline
	void make_subkeys_reversed( uint64_t* subkeys, const uint64_t& key )
	{
		uint64_t straight[ 16 ];
		
		make_subkeys( straight, key );
		
		reverse_subkeys( subkeys, straight );
	}
	
}

#endif
