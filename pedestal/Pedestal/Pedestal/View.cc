/*	=======
 *	View.cc
 *	=======
 */

#include "Pedestal/View.hh"


namespace Pedestal
{
	
	boost::shared_ptr< Quasimode > View::EnterShiftSpaceQuasimode( const EventRecord& )
	{
		return boost::shared_ptr< Quasimode >();
	}
	
}

