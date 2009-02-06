/*	============
 *	Superview.hh
 *	============
 */

#ifndef PEDESTAL_SUPERVIEW_HH
#define PEDESTAL_SUPERVIEW_HH

#include "Pedestal/View.hh"


namespace Pedestal
{
	
	class Superview : public View
	{
		public:
			virtual View& Subview() = 0;
			
			virtual Rect ApertureFromBounds( const Rect& bounds )  { return bounds; }
			
			void Install( const Rect& bounds )  { Subview().Install( bounds); }
			
			void Uninstall()  { Subview().Uninstall(); }
			
			void Idle( const EventRecord& event )
			{
				Subview().Idle( event );
			}
			
			void MouseDown( const EventRecord& event )
			{
				if ( HitTest( event ) )
				{
					Subview().MouseDown( event );
				}
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
			
			void Draw( const Rect& bounds )
			{
				return Subview().Draw( ApertureFromBounds( bounds ) );
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
			
			void Resize( short width, short height )
			{
				Subview().Resize( width, height );
			}
	};
	
}

#endif

