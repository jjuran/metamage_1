/*
	fetcher.hh
	----------
*/

#ifndef V68K_FETCHER_HH
#define V68K_FETCHER_HH

// C99
#include <stdint.h>


namespace v68k
{
	
	struct registers;
	
	class memory;
	
	
	typedef uint32_t (*fetcher)( registers&, const memory& );
	
}

#endif

