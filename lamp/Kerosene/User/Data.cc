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


#pragma export on

int errno;

int h_errno;

#if TARGET_RT_MAC_CFM
	
	iota::environ_t environ;
	
#endif

#pragma export reset

