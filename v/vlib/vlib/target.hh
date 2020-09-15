/*
	target.hh
	---------
*/

#ifndef VLIB_TARGET_HH
#define VLIB_TARGET_HH

// vlib
//#include "vlib/value.hh"


namespace vlib
{
	
	class Value;
	
	struct Target
	{
		Value*        addr;
		Value const*  type;
	};
	
}

#endif
