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


namespace Genie
{
	
	static long GetGMTDeltaField()
	{
		MachineLocation location;
		
		::ReadLocation( &location );
		
		const long raw_value = location.u.gmtDelta;
		
		return raw_value;
	}
	
	plus::string sys_mac_time_dlsgmtdelta::Read( const FSTree* that, bool binary )
	{
		const long field = GetGMTDeltaField();
		
		if ( binary )
		{
			return plus::string( (const char*) &field, sizeof field );
		}
		
		return plus::encode_32_bit_hex( field );
	}
	
}

