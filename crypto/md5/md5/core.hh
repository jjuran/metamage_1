/*
	core.hh
	-------
*/

#ifndef MD5_CORE_HH
#define MD5_CORE_HH

// md5
#include "md5/state.hh"


namespace crypto
{
	
	void md5_rounds( md5_digest& digest, u32* const block );
	
}

#endif
