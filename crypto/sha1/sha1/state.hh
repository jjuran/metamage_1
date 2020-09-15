/*
	state.hh
	--------
*/

#ifndef SHA1_STATE_HH
#define SHA1_STATE_HH

// Standard C
#include <stdint.h>


namespace crypto
{
	
	typedef uint32_t u32;
	typedef uint64_t u64;
	
	struct sha1_digest
	{
		u32 h[5];
	};
	
	struct sha1_state
	{
		u64          n_blocks;
		sha1_digest  digest;
	};
	
}

#endif
