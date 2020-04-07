/*
	segments_box.cc
	---------------
*/

#include "segments_box.hh"

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif


segments_box::segments_box( size_t capacity )  // bytes
:
	quickdraw::segments_box( (short*) NewPtr( capacity ) )
{
}

segments_box::~segments_box()
{
	DisposePtr( (Ptr) begin() );
}
