/*
	segments_box.hh
	---------------
*/

#ifndef SEGMENTSBOX_HH
#define SEGMENTSBOX_HH

// iota
#include "iota/class.hh"

// quickdraw
#include "qd/segments.hh"


class segments_box : public quickdraw::segments_box
{
	NON_COPYABLE( segments_box )
	NO_NEW_DELETE
	
	public:
		explicit segments_box( size_t capacity );  // bytes
		~segments_box();
};

#endif
