/*
	paint_screen.cc
	---------------
*/

#include "paint_screen.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-common
#include "QDGlobals.hh"


#pragma exceptions off


OSErr paint_screen( const Byte* filename )
{
	enum
	{
		image_file_size = 21888,
		
		image_width  = 512,
		image_height = 342,
	};
	
	OSErr err;
	short refnum;
	
	err = FSOpen( filename, fsRdPerm, &refnum );
	
	if ( err == noErr )
	{
		SInt32 len;
		
		err = GetEOF( refnum, &len );
		
		if ( err == noErr  &&  len >= image_file_size )
		{
			err = memFullErr;
			
			if ( Ptr ptr = NewPtr( image_file_size ) )
			{
				err = noErr;
				
				SInt32 n = image_file_size;
				
				FSRead( refnum, &n, ptr );
				
				BitMap bitmap = { ptr, 512 / 8, { 0, 0, 342, 512 } };
				
				const QDGlobals& qd = get_QDGlobals();
				
				const Rect& bounds = qd.screenBits.bounds;
				
				short display_width  = bounds.right - bounds.left;
				short display_height = bounds.bottom - bounds.top;
				
				Rect dstRect = bitmap.bounds;
				
				short dh = display_width  / 2u - image_width  / 2;
				short dv = display_height / 2u - image_height / 2;
				
				OffsetRect( &dstRect, dh, dv );
				
				ClipRect ( &bounds );
				PaintRect( &bounds );
				
				CopyBits( &bitmap,
				          &qd.screenBits,
				          &bitmap.bounds,
				          &dstRect,
				          srcCopy,
				          NULL );
				
				DisposePtr( ptr );
			}
		}
		
		FSClose( refnum );
	}
	
	return err;
}
