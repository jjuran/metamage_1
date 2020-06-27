/*
	documents.hh
	------------
*/

#include "mac_app/documents.hh"

// Mac OS
#ifdef __MC68K__
#ifndef __SEGLOAD__
#include <SegLoad.h>
#endif
#endif


namespace mac {
namespace app {

void open_AppFiles_with( HFS_callback callback )
{
#ifdef __MC68K__
	
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
