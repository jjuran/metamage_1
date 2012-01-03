/*
	sync.c
	------
*/

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// POSIX
#include <unistd.h>


void sync( void )
{
	int i = 0;
	
	OSErr err;
	
	do
	{
		HParamBlockRec pb;
		
		pb.volumeParam.ioNamePtr  = NULL;
		pb.volumeParam.ioVRefNum  = 0;
		pb.volumeParam.ioVolIndex = ++i;
		
		err = PBHGetVInfoSync( &pb );
		
		if ( err == noErr )
		{
			err = FlushVol( NULL, pb.volumeParam.ioVRefNum );
		}
	}
	while ( err != nsvErr );
}

