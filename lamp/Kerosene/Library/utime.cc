/*	========
 *	utime.cc
 *	========
 */

// POSIX
#include "sys/stat.h"
#include "utime.h"


// Exceptions are off here
#pragma exceptions off


int utime( const char* path, const struct utimbuf *time_buffer )
{
	struct timespec times[2] = { { 0, 0 }, { 0, 0 } };
	
	const struct timespec* t = NULL;
	
	if ( time_buffer )
	{
		times[0].tv_sec = time_buffer->actime;
		times[1].tv_sec = time_buffer->modtime;
		
		t = times;
	}
	
	return utimensat( AT_FDCWD, path, t, 0 );
}
