/*
	rounds.hh
	---------
*/

#ifndef SHA1_ROUNDS_HH
#define SHA1_ROUNDS_HH

// sha1
#include "sha1/state.hh"


namespace crypto
{
	
	void sha1_extend_block( u32* block );
	
	void sha1_rounds( sha1_digest& digest, u32* const block );
	
}

#endif
