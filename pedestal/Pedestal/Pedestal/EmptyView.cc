/*	============
 *	EmptyView.cc
 *	============
 */

#include "Pedestal/EmptyView.hh"

// Nitrogen
#include "Nitrogen/QuickDraw.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	void EmptyView::Draw( const Rect& bounds, bool erasing )
	{
		if ( erasing )
		{
			N::EraseRect( bounds );
		}
	}
	
	const boost::shared_ptr< View >& EmptyView::Get()
	{
		static boost::shared_ptr< View > empty = boost::shared_ptr< View >( new EmptyView );
		
		return empty;
	}
	
}

