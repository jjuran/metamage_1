/*
	common.hh
	---------
*/

#ifndef DES_COMMON_HH
#define DES_COMMON_HH

// Standard C
#include <stdint.h>


namespace des
{
	
	void make_subkeys( uint64_t* subkeys, const uint64_t& key );
	
	void initial_permutation( uint64_t& result, const uint64_t& data );
	void final_permutation  ( uint64_t& result, const uint64_t& data );
	
	void crypt_block_core( const uint64_t subkeys[ 16 ], uint64_t& data );
	
}

#endif
