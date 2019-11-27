/*
	debug/boost_assert.hh
	---------------------
*/

#ifndef DEBUG_BOOSTASSERT_HH
#define DEBUG_BOOSTASSERT_HH

#define BOOST_ENABLE_ASSERT_HANDLER
#define BOOST_NO_IOSTREAM

// Debug
#include "debug/assert.hh"

// Boost
#include <boost/assert.hpp>


namespace boost
{
	
	inline void assertion_failed( const char* expr, const char* func, const char* file, long line )
	{
		::debug::handle_failed_assertion( expr, func, file, line, true );
	}
	
}

#endif
