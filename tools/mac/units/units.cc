/*
	units.cc
	--------
*/

// Mac OS
#ifndef __APPLE__
#ifndef __DEVICES__
#include <Devices.h>
#endif
#endif

// Standard C
#include <stdio.h>

// mac-glue-utils
#include "mac_glue/Memory.hh"

// command
#include "command/get_option.hh"

// chars
#include "conv/mac_utf8.hh"


#pragma exceptions off


enum
{
	Opt_verbose = 'v',
	
	Opt_last_byte = 255,
};

static command::option options[] =
{
	{ "verbose", Opt_verbose },
	
	{}
};

static bool verbose;

static
char* const* get_options( char** argv )
{
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		using command::global_result;
		
		switch ( opt )
		{
			case Opt_verbose:
				verbose = true;
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

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

static
void print_DCE_flags( short flags )
{
	short mask = 0x1;
	
	for ( int i = 0;  i < 16;  ++i )
	{
		if ( flags & mask )
		{
			printf( "              %s\n", dce_flag_names[ i ] );
		}
		
		mask <<= 1;
	}
}

int main( int argc, char** argv )
{
	char* const* args = get_options( argv );
	
	int argn = argc - (args - argv);
	
#if ! TARGET_API_MAC_CARBON
	
	typedef AuxDCEHandle* UTable;
	
	const UTable UTableBase = *(UTable*) 0x011c;
	const short& UnitNTryCnt = *(short*) 0x01d2;
	
	for ( short i = 0;  i < UnitNTryCnt;  ++i )
	{
		const AuxDCEHandle dch = UTableBase[ i ];
		
		if ( ! dch )
		{
			continue;
		}
		
		Size size = mac::glue::GetHandleSize_raw( (Handle) dch );
		
		const AuxDCE& dce = **dch;
		
		const bool ram_based = dce.dCtlFlags & dRAMBasedMask;
		
		void* drvr = dce.dCtlDriver;
		
		// Dereferences a handle if ram_based
		const DRVRHeaderPtr header = ram_based ? *(DRVRHeader **) drvr
		                                       :  (DRVRHeader * ) drvr;
		
		unsigned char* p = header->drvrName;
		
		unsigned len = *p++;
		
		if ( *p == '\0' )
		{
			++p;
			--len;
		}
		
		char utf[ 255 * 3 + 1 ];  // worst case
		
		size_t n = conv::utf8_from_mac( utf, sizeof utf, (char*) p, len );
		
		utf[ n ] = '\0';
		
		#define PRINT_NULLABLE( field, format )  \
			if ( verbose  ||  dce.dCtl##field )  \
				printf( #field ": " format "\n",  dce.dCtl##field )
		
		printf( "Name:     %s\n",   utf              );
		printf( "RefNum:   %d\n",   dce.dCtlRefNum   );
		printf( "Flags:    %.4x\n", dce.dCtlFlags    );
		
		print_DCE_flags( dce.dCtlFlags );
		
		PRINT_NULLABLE( Position, "%ld" );
		PRINT_NULLABLE( Storage, " %p"  );
		PRINT_NULLABLE( CurTicks, "%ld" );
		PRINT_NULLABLE( Window, "  %p"  );
		PRINT_NULLABLE( Delay, "   %d"  );
		PRINT_NULLABLE( EMask, "   %d"  );
		PRINT_NULLABLE( Menu, "    %d"  );
		
		if ( size >= sizeof (AuxDCE) )
		{
			PRINT_NULLABLE( Slot, "    %d"  );
			PRINT_NULLABLE( SlotId, "  %d"  );
			PRINT_NULLABLE( DevBase, " %lx" );
			PRINT_NULLABLE( Owner, "   %p"  );
			PRINT_NULLABLE( ExtDev, "  %d"  );
			PRINT_NULLABLE( NodeID, "  %lu" );
		}
		
		printf( "\n" );
	}
	
#endif
	
	return 0;
}
