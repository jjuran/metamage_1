/*
	byterange_iterator.cc
	---------------------
*/

#include "vlib/iterators/byterange_iterator.hh"

// vlib
#include "vlib/types/byte.hh"
#include "vlib/types/byterange.hh"


namespace vlib
{
	
	byterange_iterator::byterange_iterator( const Value& range )
	{
		iota::byte_range br = range.as< ByteRange >().get();
		
		its_next = begin( br );
		its_high = end  ( br ) - 1;
	}
	
	const Value& byterange_iterator::get() const
	{
		its_value = Byte( its_next );
		
		return its_value;
	}
	
}
