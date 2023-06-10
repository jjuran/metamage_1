/*	========
 *	Stack.hh
 *	========
 */

#ifndef PEDESTAL_STACK_HH
#define PEDESTAL_STACK_HH

#ifndef PEDESTAL_VIEW_HH
#include "Pedestal/View.hh"
#endif


namespace Pedestal
{
	
	class Stack : public View
	{
		public:
			virtual unsigned ViewCount() const = 0;
			
			virtual View& GetNthView( unsigned n ) = 0;
			
			void Install( const Rect& bounds );
			
			void Uninstall();
			
			void SetBounds( const Rect& bounds );
			
			void Idle( const EventRecord& event );
			
			bool MouseDown( const EventRecord& event );
			
			bool KeyDown( const EventRecord& event );
			void KeyUp  ( const EventRecord& event );
			
			bool HitTest( const EventRecord& event );
			
			TextEdit*
			//
			EnterShiftSpaceQuasimode( const EventRecord& event );
			
			void Activate( bool activating );
			
			View* AdvanceFocus( View* current, bool backward );
			
			void Draw( const Rect& bounds, bool erasing );
			
			void DrawInContext( CGContextRef context, CGRect bounds );
			
			bool SetCursor( const EventRecord& event );
			
			bool UserCommand( CommandCode code );
	};
	
}

#endif
