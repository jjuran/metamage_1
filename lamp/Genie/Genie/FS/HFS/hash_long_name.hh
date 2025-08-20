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
	
	typedef unsigned char Byte;
	
	void hash_long_name( Byte* hashed, const char* name, size_t length );
	
}

#endif
