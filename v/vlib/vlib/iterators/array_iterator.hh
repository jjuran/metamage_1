/*
	array_iterator.hh
	-----------------
*/

#ifndef VLIB_ITERATORS_ARRAYITERATOR_HH
#define VLIB_ITERATORS_ARRAYITERATOR_HH

// vlib
#include "vlib/iterators/list_iterator.hh"


namespace vlib
{
	
	class array_iterator : public list_iterator
	{
		public:
			array_iterator( const Value& array );
			
			array_iterator& operator++()  { step(); return *this; }
	};
	
}

#endif
