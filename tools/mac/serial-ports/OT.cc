/*
	OT.cc
	-----
*/

#include "OT.hh"

// Mac OS
#ifdef __MACOS__
#ifndef __OPENTRANSPORT__
#include <OpenTransport.h>
#endif
#endif

// Standard C
#include <stdio.h>


int list_OT_serial_ports( int verbosity )
{
#ifdef __MACOS__
	
	OTPortRecord port;
	
	OTItemCount i = 0;
	
	while ( OTGetIndexedPort( &port, i ) )
	{
		OTDeviceType type = OTGetDeviceTypeFromPortRef( port.fRef );
		
		if ( type == kOTSerialDevice )
		{
			printf( "%s\n", port.fPortName );
			
			if ( verbosity )
			{
				printf( "    port flags:   %lx\n", port.fPortFlags    );
				printf( "    info flags:   %lx\n", port.fInfoFlags    );
				printf( "    capabilities: %lx\n", port.fCapabilities );
				
				const char* module = port.fModuleName;
				const char* slot   = port.fSlotID;
				const char* rsrc   = port.fResourceInfo;
				
				ItemCount count = port.fNumChildPorts;
				
				if ( *module )  printf( "    module:   %s\n",  module );
				if ( *slot   )  printf( "    slot ID:  %s\n",  slot   );
				if ( *rsrc   )  printf( "    res info: %s\n",  rsrc   );
				if ( count   )  printf( "    children: %lu\n", count  );
				
				printf( "\n" );
			}
		}
		
		++i;
	}
	
	return 0;
	
#endif
	
	fprintf( stderr, "%s\n", "Open Transport unavailable" );
	
	return 1;
}
