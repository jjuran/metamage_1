/*	============
 *	MainEntry.hh
 *	============
 */

#ifndef GENIE_MAINENTRY_HH
#define GENIE_MAINENTRY_HH

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>


namespace Genie
{
	
	class MainEntryPoint;
	
	typedef boost::intrusive_ptr< MainEntryPoint > MainEntry;
	
}

#endif

