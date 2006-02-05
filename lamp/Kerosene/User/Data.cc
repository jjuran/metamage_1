/*	=======
 *	Data.cc
 *	=======
 */

// POSIX
#include "errno.h"
#include "unistd.h"


#pragma export on
	
	int errno;
	
	char** environ;
	
#pragma export reset

