/*
	sound-info.cc
	-------------
*/

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __APPLE__
#ifndef __DEVICES__
#include <Devices.h>
#endif
#endif
#if ! __LP64__
#ifndef __SOUND__
#include <Sound.h>
#endif
#endif

// Standard C
#include <stdio.h>

// mac-sys-utils
#include "mac_sys/gestalt.hh"
#include "mac_sys/trap_available.hh"


#pragma exceptions off


using mac::sys::gestalt;
using mac::sys::trap_available;


#define QUINE( foo )  #foo, foo

#define UTableBase  (*(Ptr*) 0x011C)
#define SoundBase   (*(Ptr*) 0x0266)
#define SoundDCE    (*(Ptr*) 0x027A)

#define ROM85  (*(short*) 0x028E)


static const char* dce_flag_names[] =
{
	"dVMImmune",
	"<bit 1 undefined>",
	"<bit 2 undefined>",
	"<bit 3 undefined>",
	"<bit 4 undefined>",
	"dOpened",
	"dRAMBased",
	"drvrActive",
	
	"dReadEnable",
	"dWriteEnable",
	"dControlEnable",
	"dStatusEnable",
	"dNeedGoodBye",
	"dNeedTime",
	"dNeedLock",
	"<bit 15 undefined>",
};

#if ! TARGET_API_MAC_CARBON

static
void print_DCE( DCtlEntry* dce )
{
	printf( "        %s: %d\n",  "dCtlRefNum",  dce->dCtlRefNum  );
	printf( "        %s: %p\n",  "dCtlDriver",  dce->dCtlDriver  );
	printf( "        %s: %p\n",  "dCtlStorage", dce->dCtlStorage );
	printf( "        %s: $%x\n", "dCtlFlags",   dce->dCtlFlags   );
	
	short flags = dce->dCtlFlags;
	
	short mask = 0x1;
	
	for ( int i = 0;  i < 16;  ++i )
	{
		if ( flags & mask )
		{
			printf( "            %s\n", dce_flag_names[ i ] );
		}
		
		mask <<= 1;
	}
}

#endif

static
void Sound_Driver_info()
{
#if ! TARGET_API_MAC_CARBON
	
	DCtlEntry*** unit_table = (DCtlEntry***) UTableBase;
	
	DCtlEntry** UTable3 = unit_table[ 3 ];
	
	DCtlEntry* UTable3_ = UTable3 ? *UTable3 : NULL;
	
	printf( "%s\n", "Sound Driver" );
	printf( "    %s: %p\n", QUINE( UTableBase ) );
	printf( "    %s: %p\n", QUINE( SoundBase  ) );
	printf( "    %s: %p\n", QUINE( SoundDCE   ) );
	
	DCtlEntry* dce = (DCtlEntry*) SoundDCE;
	
	/*
		Don't print the info for SoundDCE if we're going to print the
		same thing for the DCE in the unit table immediately afterward.
		
		StripAddress() is always available in MacRelix.
	*/
	
	if ( (long) dce > 0  &&  StripAddress( dce ) != StripAddress( UTable3_ ) )
	{
		print_DCE( dce );
	}
	
	if ( UTable3 )
	{
		printf( "    %s: %p\n", "*unit[3]", *UTable3 );
		
		print_DCE( *UTable3 );
	}
	
	OSErr err;
	
	const SInt16 empty_square_wave[] =
	{
		-1,       // swMode
		0, 0, 0,  // terminating zero Tone
	};
	
	SInt32 size = sizeof empty_square_wave;
	
	err = FSWrite( -4, &size, empty_square_wave );
	
	const char* error = NULL;
	
	switch ( err )
	{
		case    0:  error = "noErr";          break;
		case  -22:  error = "unitEmptyErr";   break;
		case  -28:  error = "notOpenErr";     break;
		case -200:  error = "noHardwareErr";  break;
	}
	
	if ( error )
	{
		printf( "    %s: %s\n", "Square wave test", error );
	}
	else
	{
		printf( "    %s: OSErr %d\n", "Square wave test", err );
	}
	
	printf( "%s\n", "" );
	
#endif
}

static const char* snd_flag_names[] =
{
	"gestaltStereoCapability",
	"gestaltStereoMixing",
	"gestaltQuickTimeUsesNewSoundMgrCalls",
	"gestaltSoundIOMgrPresent",
	"gestaltBuiltInSoundInput",
	"gestaltHasSoundInputDevice",
	"gestaltPlayAndRecord",
	"gestalt16BitSoundIO",
	
	"gestaltStereoInput",
	"gestaltLineLevelInput",
	"gestaltSndPlayDoubleBuffer",
	"gestaltMultiChannels",
	"gestalt16BitAudioSupport",
	"<bit 13 undefined>",
	"<bit 14 undefined>",
	"<bit 15 undefined>",
	
	"<bit 16 undefined>",
	"<bit 17 undefined>",
	"<bit 18 undefined>",
	"<bit 19 undefined>",
	"<bit 20 undefined>",
	"<bit 21 undefined>",
	"<bit 22 undefined>",
	"<bit 23 undefined>",
	
	"<bit 24 undefined>",
	"<bit 25 undefined>",
	"<bit 26 undefined>",
	"<bit 27 undefined>",
	"<bit 28 undefined>",
	"<bit 29 undefined>",
	"<bit 30 undefined>",
	"<bit 31 undefined>",
};

static
bool has_SoundManager()
{
	enum
	{
		_SoundDispatch = 0xA800,
	};
	
#if ! __LP64__
	
	if ( ! TARGET_CPU_68K )
	{
		return true;
	}
	
	return ROM85 > 0  &&  trap_available( _SoundDispatch );
	
#endif
	
	return false;
}

static
void Sound_Manager_info()
{
	if ( ! has_SoundManager() )
	{
		return;
	}
	
	printf( "%s\n", "Sound Manager" );
	
	NumVersion version = SndSoundManagerVersion();
	
	Byte point = '0' | (version.minorAndBugRev      & 0xF);
	Byte minor = '0' | (version.minorAndBugRev >> 4 & 0xF);
	Byte major = '0' |  version.majorRev;
	
	printf( "    %s: %c.%c.%c\n", "version", major, minor, point );
	
	UInt32 snd  = gestalt( 'snd ' );
	UInt32 snhw = gestalt( 'snhw' );
	
	printf( "    %s: $%x\n", "'snd '", (int) snd );
	
	if ( snd )
	{
		long mask = 0x1;
		
		for ( int i = 0;  i < 32;  ++i )
		{
			const char* name = snd_flag_names[ i ];
			
			char got = snd & mask       ? '*'
			         : name[ 0 ] != '<' ? ' '
			         :                    '\0';
			
			if ( got )
			{
				printf( "        %c %s\n", got, name );
			}
			
			mask <<= 1;
		}
	}
	
	if ( snhw )
	{
		Byte a = snhw >> 24;
		Byte b = snhw >> 16;
		Byte c = snhw >>  8;
		Byte d = snhw;
		
		printf( "    %s: '%c%c%c%c'\n", "'snhw'", a, b, c, d );
	}
	
	printf( "%s\n", "" );
}

int main( int argc, char** argv )
{
	Sound_Driver_info();
	Sound_Manager_info();
	
	return 0;
}
