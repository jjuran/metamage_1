/*
	hash_long_name.hh
	-----------------
*/

#ifndef RELIXLONGFILENAMEHASH_HASHLONGNAME_HH
#define RELIXLONGFILENAMEHASH_HASHLONGNAME_HH

// more-libc
#include "more/size.h"


namespace relix
{
	
	typedef unsigned char Byte;
	
	void hash_long_name( Byte* hashed, const char* name, size_t length );
	
}

#endif
