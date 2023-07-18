/*
	adbs.cc
	-------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#if ! TARGET_API_MAC_CARBON
#ifndef __DESKBUS__
#include <DeskBus.h>
#endif
#endif
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// Standard C
#include <stdio.h>

// mac-config
#include "mac_config/adb.hh"

// mac-sys-utils
#include "mac_sys/has/ADB.hh"

// mac-adb-utils
#include "mac_adb/command.hh"


static inline
bool has_ADB()
{
	return CONFIG_ADB  &&  (CONFIG_ADB_GRANTED  ||  mac::sys::has_ADB());
}

int main( int argc, char** argv )
{
	if ( ! has_ADB() )
	{
		return 1;
	}
	
	const UInt8 kADBTalkCommand = 12;
	
#if ! TARGET_API_MAC_CARBON
	
	short n = CountADBs();
	
	for ( short i = 1;  i <= n;  ++i )
	{
		ADBDataBlock data;
		
		short address = GetIndADB( &data, i );
		
		Byte registers[ 9 ][ 4 ];
		
		short command = address << 4 | kADBTalkCommand | 0;
		
		for ( short r = 0;  r < 4;  ++r, ++command )
		{
			mac::adb::send_command( registers[ r ], command );
		}
		
		const UInt8 id = registers[ 3 ][ 2 ];
		
		printf( "Type:   %.2x -> %.2x\n", data.devType, id );
		printf( "Origin: %c\n",     '0' + data.origADBAddr );
		
		printf( "Registers:\n" );
		
		for ( short r = 0;  r < 4;  ++r )
		{
			const Byte* reg = registers[ r ];
			
			const short n = reg[ 0 ];
			
			printf( "    %c: ", '0' + r );
			
			for ( short i = 1;  i <= n;  ++i )
			{
				printf( " %.2x", reg[ i ] );
			}
			
			printf( "\n" );
		}
		
		printf( "\n" );
	}
	
	return 0;
	
#endif
}
