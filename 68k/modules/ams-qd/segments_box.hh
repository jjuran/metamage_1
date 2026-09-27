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

// iota
#include "iota/class.hh"

// quickdraw
#include "qd/segments.hh"


extern Handle segments_storage_handle;

inline
short* segments_storage( size_t capacity )
{
	return (short*) *segments_storage_handle;
}

class segments_box : public quickdraw::segments_box
{
	NON_COPYABLE( segments_box )
	NO_NEW_DELETE
	
	public:
		explicit segments_box( size_t capacity );  // bytes
		~segments_box();
};

#endif
