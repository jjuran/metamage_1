/*
	copyfile.cc
	-----------
*/

// POSIX
#include "fcntl.h"

// Relix
#include "relix/copyfile.h"

// Exceptions are off here
#pragma exceptions off


int copyfile( const char* src, const char* dest )
{
	return copyfileat( AT_FDCWD, src, AT_FDCWD, dest, 0 );
}
