/*
	dock_tile.cc
	------------
*/

#include "dock_tile.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// CGTictactoe
#include "CGTictactoe.hh"


/*
	In Mac OS X 10.5 and 10.6, the CGContext returned from
	BeginCGContextForApplicationDockTile() is persistent,
	and changes to its state are preserved across begin/end pairs.
	
	If this were the norm, we might take advantage of it by setting up
	the coordinate system only once instead of each time.  But since
	we have to do that in all other versions of OS X, it wouldn't be
	worth the added complexity.  So we save and restore the state in
	10.5 and 10.6 to match the behavior seen everywhere else.
*/

#ifdef MAC_OS_X_VERSION_10_5
#ifndef MAC_OS_X_VERSION_10_7
#define CONFIG_DOCKTILE_NEEDS_SAVED_GSTATE  1
#endif
#endif

#ifndef CONFIG_DOCKTILE_NEEDS_SAVED_GSTATE
#define CONFIG_DOCKTILE_NEEDS_SAVED_GSTATE  0
#endif


void draw_dock_tile( signed char board[ 9 ] )
{
#ifdef __APPLE__
	
//	const float dock_tile_edge = 128;
//	const float game_grid_edge =  32;
	
	CGContextRef context = BeginCGContextForApplicationDockTile();
	
	if ( CONFIG_DOCKTILE_NEEDS_SAVED_GSTATE )
	{
		CGContextSaveGState( context );
	}
	
	/*
		V-flip the coordinate system (so Y increases downward) and
		scale from the dock tile's 128x128 to our grid's 32x32.
		
		These operations involve CTM-scales of 1,-1 and 4,4 respectively,
		but these can be combined into a single CGContextScaleCTM() call.
	*/
	
	CGContextTranslateCTM( context, 0, 128 );
	CGContextScaleCTM    ( context, 4,  -4 );
	
	// Erase the canvas.
	
	const CGRect rect = CGRectMake( 0, 0, 32, 32 );
	
	CGContextSetGrayFillColor( context, 1, 1 );  // white
	CGContextFillRect( context, rect );
	
	draw_board( context, board );
	
	if ( CONFIG_DOCKTILE_NEEDS_SAVED_GSTATE )
	{
		CGContextRestoreGState( context );
	}
	
	CGContextFlush( context );
	
	EndCGContextForApplicationDockTile( context );
	
#endif
}
