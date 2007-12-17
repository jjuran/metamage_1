/*	=======
 *	View.hh
 *	=======
 */

#ifndef PEDESTAL_VIEW_HH
#define PEDESTAL_VIEW_HH

// Mac OS
#ifndef __QUICKDRAW__
	#include <Quickdraw.h>
#endif

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
			
			void Idle     ( const EventRecord& event )  {}
			void MouseDown( const EventRecord& event )  {}
			bool KeyDown  ( const EventRecord& event )  { return false; }
			
			boost::shared_ptr< Quasimode > EnterShiftSpaceQuasimode( const EventRecord& )  { return boost::shared_ptr< Quasimode >(); }
			
			void Resize( const Rect& newBounds )  {}
			
			void Update()  {}
			
			void Activate( bool activating )  {}
			
			void UpdateScrollPosition( Point scrollPosition )  {}
			
			bool SetCursor( Point      location,
			                RgnHandle  mouseRgn  )  { return false; }
			
			bool UserCommand( MenuItemCode code  )  { return false; }
	};
	
}

#endif

