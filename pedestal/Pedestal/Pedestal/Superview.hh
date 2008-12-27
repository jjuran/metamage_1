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
			
			void Install  ()  { Subview().Install  (); }
			void Uninstall()  { Subview().Uninstall(); }
			
			void Idle     ( const EventRecord& event )  {        Subview().Idle     ( event ); }
			void MouseDown( const EventRecord& event )  {        Subview().MouseDown( event ); }
			bool KeyDown  ( const EventRecord& event )  { return Subview().KeyDown  ( event ); }
			
			boost::shared_ptr< Quasimode > EnterShiftSpaceQuasimode( const EventRecord& event )
			{
				return Subview().EnterShiftSpaceQuasimode( event );
			}
			
			void Activate( bool activating )  { Subview().Activate( activating ); }
			
			bool SetCursor( Point location, RgnHandle mouseRgn )
			{
				return Subview().SetCursor( location, mouseRgn );
			}
			
			bool UserCommand( MenuItemCode code )  { return Subview().UserCommand( code ); }
	};
	
}

#endif

