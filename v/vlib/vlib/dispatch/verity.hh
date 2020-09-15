/*
	verity.hh
	---------
*/

#ifndef VLIB_DISPATCH_VERITY_HH
#define VLIB_DISPATCH_VERITY_HH


namespace vlib
{
	
	class Value;
	
	typedef bool (*truth_test)( const Value& v );
	
	struct veritization
	{
		const truth_test verity;
	};
	
}

#endif
