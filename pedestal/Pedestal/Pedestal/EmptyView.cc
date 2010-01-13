/*	============
 *	EmptyView.cc
 *	============
 */

#include "Pedestal/EmptyView.hh"


namespace Pedestal
{
	
	const boost::shared_ptr< View >& EmptyView::Get()
	{
		static boost::shared_ptr< View > empty = seize_ptr( new EmptyView );
		
		return empty;
	}
	
}

