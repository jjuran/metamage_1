/*
	types.hh
	--------
*/

#ifndef DES_TYPES_HH
#define DES_TYPES_HH

// Standard C
#include <stdint.h>

// iota
#include "iota/endian.hh"


namespace des
{
	
	struct Encryption {};
	struct Decryption {};
	
	extern Encryption encryption;
	extern Decryption decryption;
	
	typedef iota::big_endian_conversion< uint64_t >::type U_64;
	
	typedef U_64 key_t;
	typedef U_64 block_t;
	
}

#endif
