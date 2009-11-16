/*	============
 *	Superview.cc
 *	============
 */

#include "Pedestal/Superview.hh"


namespace Pedestal
{
	
	void Superview::Install( const Rect& bounds )
	{
		View::Install( bounds );
		
		Subview().Install( ApertureFromBounds( bounds ) );
	}
	
	void Superview::Uninstall()
	{
		Subview().Uninstall();
		
		View::Uninstall();
	}
	
}

