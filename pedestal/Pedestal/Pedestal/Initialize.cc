/*
	Initialize.cc
	-------------
*/

#include "Pedestal/Initialize.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif


#pragma exceptions off


namespace Pedestal
{
	
	void Init_Memory( unsigned moreMasters )
	{
	#if !TARGET_API_MAC_CARBON
		
		::MaxApplZone();
		
	#endif
		
		for ( unsigned i = 0;  i < moreMasters;  ++i )
		{
			::MoreMasters();
		}
	}
	
}
