/*
	segments_box.cc
	---------------
*/

#include "segments_box.hh"

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

// ams-common
#include "scoped_zone.hh"


#pragma exceptions off


segments_box::segments_box( size_t capacity )  // bytes
:
	quickdraw::segments_box( (scoped_zone(), (short*) NewPtr( capacity )) )
{
}

segments_box::~segments_box()
{
	DisposePtr( (Ptr) begin() );
}
