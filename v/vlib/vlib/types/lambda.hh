/*
	lambda.hh
	---------
*/

#ifndef VLIB_TYPES_LAMBDA_HH
#define VLIB_TYPES_LAMBDA_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class Lambda : public Value
	{
		public:
			Lambda( const Value& body );
	};
	
}

#endif
