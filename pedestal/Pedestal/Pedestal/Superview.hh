/*	============
 *	Superview.hh
 *	============
 */

#ifndef PEDESTAL_SUPERVIEW_HH
#define PEDESTAL_SUPERVIEW_HH

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// Pedestal
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	class Superview : public View
	{
		public:
			virtual View& Subview() = 0;
			
			virtual Rect ApertureFromBounds( const Rect& bounds )  { return bounds; }
			
			void Install( const Rect& bounds );
			
			void Uninstall();
			
			void SetBounds( const Rect& bounds )
			{
				Subview().SetBounds( ApertureFromBounds( bounds ) );
			}
			void Idle( const EventRecord& event )
			{
				Subview().Idle( event );
			}
			
			bool MouseDown( const EventRecord& event )
			{
				return HitTest( event ) && Subview().MouseDown( event );
			}
			
			bool KeyDown( const EventRecord& event )
			{
				return Subview().KeyDown( event );
			}
			
			bool HitTest( const EventRecord& event )
			{
				return Subview().HitTest( event );
			}
			
			boost::shared_ptr< Quasimode > EnterShiftSpaceQuasimode( const EventRecord& event )
			{
				return Subview().EnterShiftSpaceQuasimode( event );
			}
			
			void Activate( bool activating )  { Subview().Activate( activating ); }
			
			View* AdvanceFocus( View* current, bool backward )
			{
				return Subview().AdvanceFocus( current, backward );
			}
			
			void Draw( const Rect& bounds, bool erasing )
			{
				return Subview().Draw( ApertureFromBounds( bounds ), erasing );
			}
			
			bool SetCursor( const EventRecord& event, RgnHandle mouseRgn )
			{
				if ( HitTest( event ) )
				{
					return Subview().SetCursor( event, mouseRgn );
				}
				
				return false;
			}
			
			bool UserCommand( MenuItemCode code )  { return Subview().UserCommand( code ); }
	};
	
}

#endif

