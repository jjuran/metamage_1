/*	=======
 *	Data.cc
 *	=======
 */

// POSIX
#include "errno.h"
#include "unistd.h"


#pragma export on
	
	int errno;
	
#if TARGET_RT_MAC_CFM
	
	char** environ;
	
#endif
	
#pragma export reset

