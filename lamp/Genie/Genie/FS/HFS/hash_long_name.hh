/*
	hash_long_name.hh
	-----------------
*/

#ifndef GENIE_FS_HFS_HASHLONGNAME_HH
#define GENIE_FS_HFS_HASHLONGNAME_HH

// more-libc
#include "more/size.h"


namespace Genie
{
	
	void hash_long_name( unsigned char* hashed, const char* name, size_t len );
	
}

#endif
