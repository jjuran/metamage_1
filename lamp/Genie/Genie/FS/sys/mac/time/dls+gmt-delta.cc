/*
	sys/mac/time/dls+gmt-delta.cc
	-----------------------------
*/

#include "Genie/FS/sys/mac/time/dls+gmt-delta.hh"

// mac-sys-utils
#include "mac_sys/gmt_delta.hh"

// gear
#include "gear/hexadecimal.hh"
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/hexadecimal.hh"
#include "plus/var_string.hh"


namespace Genie
{
	
	void sys_mac_time_dlsgmtdelta::get( plus::var_string& result, const vfs::node* that, bool binary )
	{
		const long field = mac::sys::gmtDelta_field();
		
		if ( binary )
		{
			result.assign( (const char*) &field, sizeof field );
		}
		
		plus::encode_32_bit_hex( result, field );
	}
	
	void sys_mac_time_dls::get( plus::var_string& out, const vfs::node* that, bool binary )
	{
		const unsigned long raw_value = mac::sys::gmtDelta_field();
		
		const unsigned char dls = raw_value >> 24;
		
		if ( binary )
		{
			out.assign( (const char*) &dls, sizeof dls );
		}
		
		out += gear::encoded_hex_char( dls >> 4 );
		out += gear::encoded_hex_char( dls >> 0 );
	}
	
	void sys_mac_time_gmtdelta::get( plus::var_string& out, const vfs::node* that, bool binary )
	{
		const long delta = mac::sys::gmt_delta();
		
		if ( binary )
		{
			out.assign( (const char*) &delta, sizeof delta );
		}
		
		out = gear::inscribe_decimal( delta );
	}
	
}
