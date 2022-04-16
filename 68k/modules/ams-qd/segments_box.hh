/*
	segments_box.hh
	---------------
*/

#ifndef SEGMENTSBOX_HH
#define SEGMENTSBOX_HH

// quickdraw
#include "qd/segments.hh"


class segments_box : public quickdraw::segments_box
{
	private:
		// non-copyable
		segments_box           ( const segments_box& );
		segments_box& operator=( const segments_box& );
	
	public:
		explicit segments_box( size_t capacity );  // bytes
		~segments_box();
};

#endif
