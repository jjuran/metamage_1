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

// chars
#include "conv/mac_utf8.hh"


int main( int argc, char** argv )
{
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
		
		Size size = GetHandleSize( (Handle) dch );
		
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
		
		printf( "Name:     %s\n",   utf              );
		printf( "RefNum:   %d\n",   dce.dCtlRefNum   );
		printf( "Flags:    %.4x\n", dce.dCtlFlags    );
		printf( "Position: %ld\n",  dce.dCtlPosition );
		printf( "Storage:  %p\n",   dce.dCtlStorage  );
		printf( "CurTicks: %ld\n",  dce.dCtlCurTicks );
		printf( "Window:   %p\n",   dce.dCtlWindow   );
		printf( "Delay:    %d\n",   dce.dCtlDelay    );
		printf( "EMask:    %d\n",   dce.dCtlEMask    );
		printf( "Menu:     %d\n",   dce.dCtlMenu     );
		
		if ( size >= sizeof (AuxDCE) )
		{
			printf( "Slot:     %d\n",   dce.dCtlSlot     );
			printf( "SlotId:   %d\n",   dce.dCtlSlotId   );
			printf( "DevBase:  %lx\n",  dce.dCtlDevBase  );
			printf( "Owner:    %p\n",   dce.dCtlOwner    );
			printf( "ExtDev:   %d\n",   dce.dCtlExtDev   );
			printf( "NodeID:   %lu\n",  dce.dCtlNodeID   );
		}
		
		printf( "\n" );
	}
	
#endif
	
	return 0;
}
