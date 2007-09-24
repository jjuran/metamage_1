/*	==============
 *	MemoryLimit.cc
 *	==============
 */

#include "Backtrace/MemoryLimit.hh"

#ifdef __MACOS__
#ifndef __GESTALT__
#include <Gestalt.h>
#endif
#endif


namespace Backtrace
{
	
#ifdef __MACOS__
	
	static bool InOSX()
	{
		long response = 0;
		
		OSErr err = ::Gestalt( gestaltSystemVersion, &response );
		
		return err == noErr  &&  response >= 0x1000;
	}
	
	static UInt32 LogicalRAMSize()
	{
		long response = 0;
		
		OSErr err = ::Gestalt( gestaltLogicalRAMSize, &response );
		
		return err == noErr ? response : 0;
	}
	
	const void* MemoryLimit()
	{
		if ( TARGET_API_MAC_CARBON )
		{
			static bool inOSX = InOSX();
			
			if ( inOSX )
			{
				return reinterpret_cast< const void* >( 0xffffffff );
			}
		}
		
		static UInt32 logicalRAMSize = LogicalRAMSize();
		
		return reinterpret_cast< const void* >( logicalRAMSize );
	}
	
#endif
	
}

