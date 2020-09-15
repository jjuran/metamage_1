/*
	assert.hh
	---------
*/

#ifndef VLIB_ASSERT_HH
#define VLIB_ASSERT_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class source_spec;
	
	void check_assertion_result( const Value&        test,
	                             const Value&        v,
	                             const source_spec&  source );
	
}

#endif
