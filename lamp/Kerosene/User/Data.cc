/*	=======
 *	Data.cc
 *	=======
 */

#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

#if TARGET_RT_MAC_CFM

// POSIX
#include "errno.h"
#include "unistd.h"


#pragma export on
	
	int errno;
	
	char** environ;
	
#pragma export reset

#endif

