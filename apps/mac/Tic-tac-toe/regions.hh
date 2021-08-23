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

RgnHandle get_X_token( short unitLength );
RgnHandle get_O_token( short unitLength );

RgnHandle temporary_copy( RgnHandle rgn );

#endif
