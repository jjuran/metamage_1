/*
	segments_box.hh
	---------------
*/

#ifndef SEGMENTSBOX_HH
#define SEGMENTSBOX_HH

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

// quickdraw
#include "qd/segments.hh"


extern Handle segments_storage_handle;

inline
short* segments_storage( size_t capacity )
{
	SetHandleSize( segments_storage_handle, capacity );
	
	return (short*) *segments_storage_handle;
}

class segments_box : public quickdraw::segments_box
{
	public:
		explicit segments_box( size_t capacity );  // bytes
};

inline
segments_box::segments_box( size_t capacity )  // bytes
:
	quickdraw::segments_box( segments_storage( capacity ) )
{
}

#endif
