/*
	sys/mac/time/dls+gmt-delta.cc
	-----------------------------
*/

#include "Genie/FS/sys/mac/time/dls+gmt-delta.hh"

// Mac OS
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

// iota
#include "iota/decimal.hh"
#include "iota/hexidecimal.hh"

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
	
	void sys_mac_time_dlsgmtdelta::get( plus::var_string& result, const FSTree* that, bool binary )
	{
		const long field = GetGMTDeltaField();
		
		if ( binary )
		{
			result.assign( (const char*) &field, sizeof field );
		}
		
		plus::encode_32_bit_hex( result, field );
	}
	
	void sys_mac_time_dls::get( plus::var_string& out, const FSTree* that, bool binary )
	{
		const unsigned long raw_value = GetGMTDeltaField();
		
		const unsigned char dls = raw_value >> 24;
		
		if ( binary )
		{
			out.assign( (const char*) &dls, sizeof dls );
		}
		
		out += iota::encoded_hex_char( dls >> 4 );
		out += iota::encoded_hex_char( dls >> 0 );
	}
	
	void sys_mac_time_gmtdelta::get( plus::var_string& out, const FSTree* that, bool binary )
	{
		const long raw_value = GetGMTDeltaField();
		
		// Mask off DLS byte, and sign extend if negative
		const long delta = (raw_value & 0x00FFFFFF) | (raw_value & 0x00800000) * 0xFF << 1;
		
		if ( binary )
		{
			out.assign( (const char*) &delta, sizeof delta );
		}
		
		out = iota::inscribe_decimal( delta );
	}
	
}

