/*
	errno.hh
	--------
*/

#ifndef RELIX_ERRNO_HH
#define RELIX_ERRNO_HH


namespace relix
{
	
	int set_errno( int errnum );
	
	int set_errno_from_exception();
	
}

#endif

