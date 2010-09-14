/*
	sys/mac/time/dls+gmt-delta.cc
	-----------------------------
*/

#include "Genie/FS/sys/mac/time/dls+gmt-delta.hh"

// Mac OS
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

// plus
#include "plus/hexidecimal.hh"
#include "plus/var_string.hh"


namespace Genie
{
	
	static long GetGMTDeltaField()
	{
		MachineLocation location;
		
		::ReadLocation( &location );
		
		const long raw_value = location.u.gmtDelta;
		
		return raw_value;
	}
	
	void sys_mac_time_dlsgmtdelta::Read( plus::var_string& result, const FSTree* that, bool binary )
	{
		const long field = GetGMTDeltaField();
		
		if ( binary )
		{
			result.assign( (const char*) &field, sizeof field );
		}
		
		plus::encode_32_bit_hex( result, field );
	}
	
}

