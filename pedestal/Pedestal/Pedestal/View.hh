/*	=======
 *	View.hh
 *	=======
 */

#ifndef PEDESTAL_VIEW_HH
#define PEDESTAL_VIEW_HH

// Mac OS
#include <Quickdraw.h>

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
			virtual void Uninstall(                    )  {}
			
			virtual void SetBounds( const Rect& bounds )  {}
			
			virtual void Idle     ( const EventRecord& event )  {}
			virtual void MouseDown( const EventRecord& event )  {}
			virtual bool KeyDown  ( const EventRecord& event )  { return false; }
			virtual bool HitTest  ( const EventRecord& event )  { return true;  }
			
			virtual boost::shared_ptr< Quasimode > EnterShiftSpaceQuasimode( const EventRecord& );
			
			virtual void Resize( short width, short height )  {}
			
			virtual void Draw( const Rect& bounds )  {}
			
			virtual void Activate( bool activating )  {}
			
			virtual bool SetCursor( const EventRecord&  event,
			                        RgnHandle           mouseRgn  )  { return false; }
			
			virtual bool UserCommand( MenuItemCode code  )  { return false; }
	};
	
}

#endif

