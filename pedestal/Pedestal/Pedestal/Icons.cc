/*	========
 *	Icons.cc
 *	========
 */

#include "Pedestal/Icons.hh"

// Nitrogen
#include "Nitrogen/Icons.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	void IconID::Plot( const Rect& area )
	{
		N::PlotIconID( area,
		               kAlignNone,
		               kTransformNone,
		               resID );
	}
	
}

