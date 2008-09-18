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

#if PRAGMA_ONCE
#pragma once
#endif


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
			
			boost::shared_ptr< Quasimode > EnterShiftSpaceQuasimode( const EventRecord& );
			
			void Resize( short width, short height )  {}
			
			void Update()  {}
			
			void Activate( bool activating )  {}
			
			bool SetCursor( Point      location,
			                RgnHandle  mouseRgn  )  { return false; }
			
			bool UserCommand( MenuItemCode code  )  { return false; }
	};
	
}

#endif

