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
#include "mac_config/upp-macros.hh"

// mac-sys-utils
#include "mac_sys/has/ADB.hh"


static inline
bool has_ADB()
{
	return CONFIG_ADB  &&  (CONFIG_ADB_GRANTED  ||  mac::sys::has_ADB());
}

#if TARGET_CPU_68K

static
asm pascal void ADBCompletion()
{
	/*
		buffer  in A0
		refCon  in A2 (we always pass a non-NULL pointer)
		command in D0 (byte, assured to be non-zero)
	*/
	
	MOVE.B   D0,(A2)  // The command is a byte, assured to be non-zero
	RTS
}

#else

static
pascal void ADBCompletion( Ptr buffer, Ptr refCon, long command )
{
	*refCon = true;
}

#endif

#if ! TARGET_API_MAC_CARBON

static
OSErr SendADBCommandSync( Ptr buffer, short command )
{
	DEFINE_UPP( ADBCompletion, ADBCompletion )
	
	char refCon = false;
	
	OSErr err = ADBOp( &refCon, UPP_ARG( ADBCompletion ), buffer, command );
	
	if ( err == noErr )
	{
		volatile const char& done = refCon;
		
		while ( ! done )
		{
			continue;
		}
	}
	
	return err;
}

#endif

int main( int argc, char** argv )
{
	if ( ! has_ADB() )
	{
		return 1;
	}
	
	const uint8_t kADBTalkCommand = 12;
	
#if ! TARGET_API_MAC_CARBON
	
	short n = CountADBs();
	
	for ( short i = 1;  i <= n;  ++i )
	{
		ADBDataBlock data;
		
		short address = GetIndADB( &data, i );
		
		uint8_t registers[ 9 ][ 4 ];
		
		short command = address << 4 | kADBTalkCommand | 0;
		
		for ( short r = 0;  r < 4;  ++r, ++command )
		{
			SendADBCommandSync( (Ptr) registers[ r ], command );
		}
		
		const uint8_t id = registers[ 3 ][ 2 ];
		
		printf( "Type:   %.2x -> %.2x\n", data.devType, id );
		printf( "Origin: %c\n",     '0' + data.origADBAddr );
		
		printf( "Registers:\n" );
		
		for ( short r = 0;  r < 4;  ++r )
		{
			const uint8_t* reg = registers[ r ];
			
			const int8_t n = reg[ 0 ];
			
			printf( "    %c: ", '0' + r );
			
			for ( int8_t i = 1;  i <= n;  ++i )
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
