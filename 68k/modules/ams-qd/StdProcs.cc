/*
	StdProcs.cc
	-----------
*/

#include "StdProcs.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-common
#include "callouts.hh"


#pragma mark -
#pragma mark Miscellaneous Routines
#pragma mark -

pascal void SetStdProcs_patch( QDProcs* procs )
{
	fast_memset( procs, '\0', sizeof (QDProcs) );
}
