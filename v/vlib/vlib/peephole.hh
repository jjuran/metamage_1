/*
	peephole.hh
	-----------
*/

#ifndef VLIB_PEEPHOLE_HH
#define VLIB_PEEPHOLE_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	void optimize_lambda_body( Value& body );
	
}

#endif
