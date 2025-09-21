/*
	files.cc
	--------
*/

#include "files.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif


short find_appl( Str255 name )
{
	OSErr err;
	
	ParamBlockRec pb;
	
	VolumeParam& vol  = pb.volumeParam;
	FileParam&   file = pb.fileParam;
	
	file.ioNamePtr  = name;
	file.ioFVersNum = 0;
	
	short vindex = 0;
	
	do
	{
		vol.ioNamePtr = name;
		
		vol.ioVolIndex = ++vindex;
		
		err = PBGetVInfoSync( &pb );
		
		if ( err )
		{
			break;
		}
		
		short findex = 0;
		
		while ( true )
		{
			file.ioFDirIndex = ++findex;
			
			err = PBGetFInfoSync( &pb );
			
			if ( err )
			{
				break;
			}
			
			if ( file.ioFlFndrInfo.fdType == 'APPL' )
			{
				return -vol.ioVRefNum;
			}
		}
	}
	while ( err == fnfErr );
	
	name[ 0 ] = 0;
	
	if ( err != nsvErr )
	{
		return err;
	}
	
	return 0;

}
