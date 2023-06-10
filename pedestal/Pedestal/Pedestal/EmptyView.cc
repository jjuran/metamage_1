/*
	EmptyView.cc
	------------
*/

#include "Pedestal/EmptyView.hh"

// debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// Pedestal
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	View* EmptyView()
	{
		static boost::intrusive_ptr< View > empty = new View;
		
		return empty.get();
	}
	
}
