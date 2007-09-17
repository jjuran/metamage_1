/*	=======
 *	Data.cc
 *	=======
 */

#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

// POSIX
#include "errno.h"

// Iota
#include "iota/environ.hh"

#if TARGET_RT_MAC_CFM

#pragma export on
	
	int errno;
	
	iota::environ_t environ;
	
#pragma export reset

#endif

