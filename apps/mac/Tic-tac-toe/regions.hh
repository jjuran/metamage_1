/*
	regions.hh
	----------
*/

#ifndef TICTACTOE_REGIONS_HH
#define TICTACTOE_REGIONS_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

extern RgnHandle allocRgns[ 10 ];
extern RgnHandle mouseRgns[ 10 ];
extern RgnHandle otherRgn;

RgnHandle alloc_mouseRgns();

RgnHandle deactivate_region( int rgn_index );
RgnHandle reactivate_region( int rgn_index );

RgnHandle get_X_token( short unitLength );
RgnHandle get_O_token( short unitLength );

#endif
