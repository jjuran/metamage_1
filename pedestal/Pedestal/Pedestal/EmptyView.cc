/*	============
 *	EmptyView.cc
 *	============
 */

#include "Pedestal/EmptyView.hh"


namespace Pedestal
{
	
	const boost::shared_ptr< View >& EmptyView::Get()
	{
		static boost::shared_ptr< View > empty = boost::shared_ptr< View >( new EmptyView );
		
		return empty;
	}
	
}

