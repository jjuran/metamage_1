/*
	state.hh
	--------
*/

#ifndef MD5_STATE_HH
#define MD5_STATE_HH

// Standard C
#include <stdint.h>


namespace crypto
{
	
	typedef uint32_t u32;
	typedef uint64_t u64;
	
	struct md5_digest
	{
		u32 a, b, c, d;
	};
	
	struct md5_state
	{
		u64         n_blocks;
		md5_digest  digest;
	};
	
}

#endif
