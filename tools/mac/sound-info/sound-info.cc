/*
	sound-info.cc
	-------------
*/

// Mac OS
#ifndef __APPLE__
#ifndef __DEVICES__
#include <Devices.h>
#endif
#endif

// Standard C
#include <stdio.h>


#pragma exceptions off


#define QUINE( foo )  #foo, foo

#define UTableBase  (*(Ptr*) 0x011C)
#define SoundBase   (*(Ptr*) 0x0266)
#define SoundDCE    (*(Ptr*) 0x027A)


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

int main( int argc, char** argv )
{
	Sound_Driver_info();
	
	return 0;
}
