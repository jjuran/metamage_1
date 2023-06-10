/*	============
 *	Superview.hh
 *	============
 */

#ifndef PEDESTAL_SUPERVIEW_HH
#define PEDESTAL_SUPERVIEW_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// Pedestal
#ifndef PEDESTAL_VIEW_HH
#include "Pedestal/View.hh"
#endif


namespace Pedestal
{
	
	class Superview : public View
	{
		public:
			virtual View& Subview() = 0;
			
			virtual Rect ApertureFromBounds( const Rect& bounds );
			
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
			
			void KeyUp( const EventRecord& event )
			{
				Subview().KeyUp( event );
			}
			
			bool HitTest( const EventRecord& event )
			{
				return Subview().HitTest( event );
			}
			
			TextEdit* EnterShiftSpaceQuasimode( const EventRecord& event );
			
			void Activate( bool activating )  { Subview().Activate( activating ); }
			
			View* AdvanceFocus( View* current, bool backward )
			{
				return Subview().AdvanceFocus( current, backward );
			}
			
			void Draw( const Rect& bounds, bool erasing )
			{
				return Subview().Draw( ApertureFromBounds( bounds ), erasing );
			}
			
			void DrawInContext( CGContextRef context, CGRect bounds );
			
			bool SetCursor( const EventRecord& event )
			{
				if ( HitTest( event ) )
				{
					return Subview().SetCursor( event );
				}
				
				return false;
			}
			
			bool UserCommand( CommandCode code )  { return Subview().UserCommand( code ); }
	};
	
}

#endif
