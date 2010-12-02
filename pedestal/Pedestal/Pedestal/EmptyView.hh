/*	============
 *	EmptyView.hh
 *	============
 */

#ifndef PEDESTAL_EMPTYVIEW_HH
#define PEDESTAL_EMPTYVIEW_HH

// Pedestal
#ifndef PEDESTAL_VIEW_HH
#include "Pedestal/View.hh"
#endif


namespace Pedestal
{
	
	class EmptyView : public View
	{
		public:
			static const boost::intrusive_ptr< View >& Get();
	};
	
}

#endif

