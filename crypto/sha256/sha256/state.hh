/*
	state.hh
	--------
*/

#ifndef SHA256_STATE_HH
#define SHA256_STATE_HH

// Standard C
#include <stdint.h>


namespace crypto
{
	
	typedef uint32_t u32;
	typedef uint64_t u64;
	
	struct sha256_hash
	{
		u32 h[8];
	};
	
	struct sha256_state
	{
		u64          n_blocks;
		sha256_hash  digest;
	};
	
}

#endif
