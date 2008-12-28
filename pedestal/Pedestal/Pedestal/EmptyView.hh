/*	============
 *	EmptyView.hh
 *	============
 */

#ifndef PEDESTAL_EMPTYVIEW_HH
#define PEDESTAL_EMPTYVIEW_HH

// Pedestal
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	class EmptyView : public View
	{
		public:
			void Draw( const Rect& bounds );
			
			static const boost::shared_ptr< View >& Get();
	};
	
}

#endif

