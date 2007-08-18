/*	=======
 *	View.hh
 *	=======
 */

#ifndef PEDESTAL_VIEW_HH
#define PEDESTAL_VIEW_HH

// Mac OS
#ifndef __EVENTS__
	#include <Events.h>
#endif

// Pedestal
#include "Pedestal/MenuItemCode.hh"


namespace Pedestal
{
	
	class View
	{
		public:
			struct Initializer {};
			
			void Idle     ( const EventRecord& /*event*/ )  {}
			void MouseDown( const EventRecord& /*event*/ )  {}
			bool KeyDown  ( const EventRecord& /*event*/ )  { return false; }
			
			bool EnterShiftSpaceQuasiMode()  { return false; }
			
			void ExitShiftSpaceQuasiMode()  {}
			
			void Resize( const Rect& /*newBounds*/ )  {}
			
			void Update()  {}
			
			void Activate( bool /*activating*/ )  {}
			
			void UpdateScrollPosition( Point /*scrollPosition*/ )  {}
			
			bool SetCursor( Point      /*location*/,
			                RgnHandle  /*mouseRgn*/  )  { return false; }
			
			bool UserCommand( MenuItemCode /*code*/  )  { return false; }
	};
	
}

#endif

