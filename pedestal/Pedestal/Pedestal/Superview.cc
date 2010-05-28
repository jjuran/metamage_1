/*	============
 *	Superview.cc
 *	============
 */

#include "Pedestal/Superview.hh"

// Pedestal
#include "Pedestal/Quasimode.hh"


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
	
	boost::intrusive_ptr< Quasimode >
	//
	Superview::EnterShiftSpaceQuasimode( const EventRecord& event )
	{
		return Subview().EnterShiftSpaceQuasimode( event );
	}
	
}

