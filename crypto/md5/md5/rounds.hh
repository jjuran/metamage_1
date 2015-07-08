/*
	rounds.hh
	---------
*/

#ifndef MD5_ROUNDS_HH
#define MD5_ROUNDS_HH

// md5
#include "md5/state.hh"


namespace crypto
{
	
	void md5_rounds( md5_digest& digest, u32* const block );
	
}

#endif
