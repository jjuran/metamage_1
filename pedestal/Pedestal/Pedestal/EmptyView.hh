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
			static const boost::intrusive_ptr< View >& Get();
	};
	
}

#endif

