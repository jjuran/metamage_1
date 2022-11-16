/*	==========
 *	setleds.cc
 *	==========
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

// POSIX
#include <unistd.h>

// Standard C
#include <stdio.h>
#include <string.h>

// iota
#include "iota/strings.hh"

// mac-adb-utils
#include "mac_adb/LEDs.hh"


#pragma exceptions off


#if !TARGET_API_MAC_CARBON


static int gChangedBits     = 0;
static int gChangedBitsMask = 0;


static inline bool IsKeyboard( const ADBDataBlock& adbData )
{
	const SInt8 kADBKeyboardDevice = 2;
	
	return adbData.devType == kADBKeyboardDevice;
}


static void PrintLEDs( UInt8 leds )
{
	const char* status[] = { "off", "on " };
	
	bool num    = leds & 1;
	bool caps   = leds & 2;
	bool scroll = leds & 4;
	
	#define NUM    "NumLock"
	#define CAPS   "CapsLock"
	#define SCROLL "ScrollLock"
	
	#define ON_ " on "
	#define OFF " off"
	#define SUB " %s"
	
	#define FORMAT \
	"Current default flags:  " NUM ON_ "   " CAPS OFF "   " SCROLL OFF "\n" \
	"Current flags:          " NUM ON_ "   " CAPS OFF "   " SCROLL OFF "\n" \
	"Current leds:           " NUM SUB "   " CAPS SUB "   " SCROLL SUB "\n"
	
	printf( FORMAT, status[ num ], status[ caps ], status[ scroll ] );
}

static
void DoLEDs( short i )
{
	ADBDataBlock data;
	ADBAddress address = GetIndADB( &data, i );
	
	if ( IsKeyboard( data ) )
	{
		Byte leds = mac::adb::get_LEDs( address );
		
		if ( gChangedBitsMask == 0 )
		{
			PrintLEDs( leds );
		}
		else
		{
			leds &= ~gChangedBitsMask;
			leds |= gChangedBits;
			
			mac::adb::set_LEDs( address, leds );
		}
	}
}

static const char* gOptionNames[] = { "num", "caps", "scroll", NULL };

#endif


int main( int argc, char** argv )
{
#if TARGET_API_MAC_CARBON
	
	write( STDERR_FILENO, STR_LEN( "setleds: ADB unsupported in Carbon\n" ) );
	
	return 1;
	
#else
	
	for ( char** argp = argv + 1;  *argp != NULL;  ++argp )
	{
		const char* arg = *argp;
		
		bool on = false;
		
		if ( arg[0] == '+' )
		{
			on = true;
		}
		else if ( arg[0] != '-' )
		{
			fprintf( stderr, "setleds: unrecognized argument '%s'\n", arg );
			
			return 2;
		}
		
		for ( const char** pp = gOptionNames;  ;  ++pp )
		{
			if ( strcmp( *pp, arg + 1 ) == 0 )
			{
				int bit = pp - gOptionNames;
				
				gChangedBits     |= on << bit;
				gChangedBitsMask |= 1  << bit;
				
				break;
			}
			
			if ( *pp == NULL )
			{
				fprintf( stderr, "setleds: unrecognized led '%s'\n", arg );
				
				return 2;
			}
		}
	}
	
	short count = CountADBs();
	
	for ( short i = 1;  i <= count;  ++i )
	{
		DoLEDs( i );
	}
	
	return 0;
	
#endif
}
