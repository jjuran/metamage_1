/*
	documents.hh
	------------
*/

#include "mac_app/documents.hh"

// Mac OS
#if ! TARGET_API_MAC_CARBON
#ifndef __SEGLOAD__
#include <SegLoad.h>
#endif
#endif


#pragma exceptions off


namespace mac {
namespace app {

void open_AppFiles_with( HFS_callback callback )
{
#if TARGET_CPU_68K  &&  ! TARGET_RT_MAC_CFM
	
	short action;
	short count;
	CountAppFiles( &action, &count );
	
	for ( int i = 1;  i <= count;  ++i )
	{
		AppFile file;
		GetAppFiles( i, &file );
		
		OSStatus err = callback( file.vRefNum, 0, file.fName );
		
		if ( err == noErr )
		{
			ClrAppFiles( i );
		}
	}
	
#endif
}

}
}
