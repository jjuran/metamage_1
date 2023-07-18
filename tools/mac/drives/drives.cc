/*
	drives.cc
	---------
*/

// Mac OS
#ifndef __APPLE__
#ifndef __FILES__
#include <Files.h>
#endif
#endif

// Standard C
#include <stdio.h>

// iota
#include "iota/char_types.hh"


int main( int argc, char** argv )
{
#if ! TARGET_API_MAC_CARBON
	
	const QHdr& DrvQHdr = *(QHdr*) 0x0308;
	
	DrvQEl* drv = (DrvQEl*) DrvQHdr.qHead;
	
	while ( drv != NULL )
	{
		const int qType = drv->qType;
		
		const UInt32 flags = ((UInt32*) drv)[ -1 ];
		
		const UInt16 fsid = drv->dQFSID;
		const UInt32 size = drv->dQDrvSz + (qType ? drv->dQDrvSz2 << 16 : 0);
		
		printf( "Drive:   %d\n",   drv->dQDrive  );
		printf( "Flags:   %.8x\n", flags );
		printf( "RefNum:  %d\n",   drv->dQRefNum );
		printf( "FSID:    %x ('%c%c')\n",
		        fsid,
		        iota::is_print( fsid >> 8 ) ? fsid >> 8 : '.',
		        iota::is_print( fsid      ) ? fsid      : '.' );
		printf( "qType:   %d\n",   qType );
		printf( "Size:    %lu\n",  size  );
		
		printf( "\n" );
		
		drv = (DrvQEl*) drv->qLink;
	}
	
#endif
	
	return 0;
}
