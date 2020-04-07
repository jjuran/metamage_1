/*
	gestalt.cc
	----------
*/

#include "mac_sys/gestalt.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __GESTALT__
#include <Gestalt.h>
#endif

// mac-sys-utils
#include "mac_sys/trap_available.hh"


namespace mac {
namespace sys {
	
	static inline
	bool has_Gestalt_trap()
	{
		enum { _Gestalt = 0xA1AD };
		
		return ! TARGET_CPU_68K  ||  trap_available( _Gestalt );
	}
	
	long gestalt( unsigned long selector, long default_value )
	{
		if ( ! has_Gestalt_trap() )
		{
			return default_value;
		}
		
		SInt32 result;
		
		const OSErr err = ::Gestalt( selector, &result );
		
		return err == noErr ? result : default_value;
	}
	
	bool gestalt_defined( unsigned long selector )
	{
		if ( ! has_Gestalt_trap() )
		{
			return false;
		}
		
		SInt32 unused_Gestalt_result;
		
		const OSErr err = ::Gestalt( selector, &unused_Gestalt_result );
		
		return err == noErr;
	}
	
}
}
