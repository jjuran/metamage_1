/*	=======
 *	View.hh
 *	=======
 */

#ifndef PEDESTAL_VIEW_HH
#define PEDESTAL_VIEW_HH

// Mac OS
#include <Quickdraw.h>

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/shared_ptr.hpp>

// Pedestal
#include "Pedestal/MenuItemCode.hh"


struct EventRecord;


namespace Pedestal
{
	
	class Quasimode;
	
	class View
	{
		public:
			struct Initializer {};
			
			virtual ~View()  {}
			
			virtual void Install  ( const Rect& bounds )  {}
			virtual void Uninstall(                    );
			
			virtual void SetBounds( const Rect& bounds )  {}
			
			virtual void Idle     ( const EventRecord& event )  {}
			virtual bool MouseDown( const EventRecord& event )  { return true;  }
			virtual bool KeyDown  ( const EventRecord& event )  { return false; }
			virtual bool HitTest  ( const EventRecord& event )  { return true;  }
			
			virtual boost::shared_ptr< Quasimode > EnterShiftSpaceQuasimode( const EventRecord& );
			
			virtual void Draw( const Rect& bounds, bool erasing );
			
			virtual void Activate( bool activating )  {}
			
			virtual void Focus()  {}
			virtual void Blur ()  {}
			virtual void Cue  ()  {}
			
			virtual View* AdvanceFocus( View* current, bool backward = false )
			{
				return current;
			}
			
			virtual bool SetCursor( const EventRecord&  event,
			                        RgnHandle           mouseRgn  )  { return false; }
			
			virtual bool UserCommand( MenuItemCode code  )  { return false; }
	};
	
}

#endif

