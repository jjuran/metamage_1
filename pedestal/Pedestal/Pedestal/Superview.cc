/*	============
 *	Superview.cc
 *	============
 */

#include "Pedestal/Superview.hh"


namespace Pedestal
{
	
	Rect Superview::ApertureFromBounds( const Rect& bounds )
	{
		return bounds;
	}
	
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
	
	TextEdit*
	//
	Superview::EnterShiftSpaceQuasimode( const EventRecord& event )
	{
		return Subview().EnterShiftSpaceQuasimode( event );
	}
	
	void Superview::DrawInContext( CGContextRef context, CGRect bounds )
	{
		Rect rect =
		{
			short( bounds.origin.y ),
			short( bounds.origin.x ),
			short( bounds.origin.y + bounds.size.height ),
			short( bounds.origin.x + bounds.size.width  ),
		};
		
		rect = ApertureFromBounds( rect );
		
		bounds.origin.x = rect.left;
		bounds.origin.y = rect.right;
		bounds.size.width  = rect.right - rect.left;
		bounds.size.height = rect.bottom - rect.top;
		
		Subview().DrawInContext( context, bounds );
	}
	
}
