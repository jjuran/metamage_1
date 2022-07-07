/*
	drvr.cc
	-------
*/

// Standard C
#include <stdio.h>

// mac-types
#include "mac_types/AuxDCE.hh"

// mac-sys-utils
#include "mac_sys/unit_table.hh"

// chars
#include "conv/mac_utf8.hh"


int main( int argc, char** argv )
{
#if ! TARGET_API_MAC_CARBON
	
	using mac::types::AuxDCEHandle;
	
	AuxDCEHandle* base = mac::sys::get_unit_table_base();
	
	const short count = mac::sys::get_unit_table_entry_count();
	
	for ( short i = 0;  i < count;  ++i )
	{
		const unsigned char* p = mac::sys::get_driver_name( base[ i ] );
		
		if ( unsigned len = *p++ )
		{
			if ( *p == '\0' )
			{
				++p;
				--len;
			}
			
			char utf[ 255 * 3 + 1 ];  // worst case
			
			size_t n = conv::utf8_from_mac( utf, sizeof utf, (char*) p, len );
			
			utf[ n ] = '\0';
			
			printf( "%d: %s\n", i, utf );
		}
		
	}
	
#endif
	
	return 0;
}
