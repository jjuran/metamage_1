/*
	range.hh
	--------
*/

#ifndef VLIB_TYPES_RANGE_HH
#define VLIB_TYPES_RANGE_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class Range : public Value
	{
		public:
			Range( const Value& left, op_type op, const Value& right );
	};
	
}

#endif
