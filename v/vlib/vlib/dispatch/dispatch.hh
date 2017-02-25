/*
	dispatch.hh
	-----------
*/

#ifndef VLIB_DISPATCH_DISPATCH_HH
#define VLIB_DISPATCH_DISPATCH_HH


namespace vlib
{
	
	struct stringifiers;
	struct veritization;
	struct comparison;
	struct operators;
	
	struct dispatch
	{
		stringifiers const*  to_string;
		veritization const*  to_boolean;
		comparison   const*  compare;
		operators    const*  ops;
	};
	
}

#endif
