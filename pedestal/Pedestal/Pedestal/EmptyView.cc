/*
	EmptyView.cc
	------------
*/

#include "Pedestal/EmptyView.hh"

// Pedestal
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	View* EmptyView::Get()
	{
		static boost::intrusive_ptr< View > empty = new View;
		
		return empty.get();
	}
	
}
