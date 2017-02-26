/*
	dispatch.hh
	-----------
*/

#ifndef VLIB_DISPATCH_DISPATCH_HH
#define VLIB_DISPATCH_DISPATCH_HH


namespace vlib
{
	
	struct stringifiers;
	
	struct dispatch
	{
		stringifiers const*  to_string;
	};
	
}

#endif
