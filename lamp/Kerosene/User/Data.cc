/*	=======
 *	Data.cc
 *	=======
 */

#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

// Iota
#include "iota/environ.hh"


#pragma export on

#if TARGET_RT_MAC_CFM
	
	iota::environ_t environ;
	
#endif

#pragma export reset

