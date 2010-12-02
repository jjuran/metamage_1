/*	=======
 *	View.hh
 *	=======
 */

#ifndef PEDESTAL_VIEW_HH
#define PEDESTAL_VIEW_HH

// Mac OS
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

// Debug
#include "debug/boost_assert.hh"

// plus
#include "plus/ref_count.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// Pedestal
#ifndef PEDESTAL_COMMANDCODE_HH
#include "Pedestal/CommandCode.hh"
#endif


struct EventRecord;      // Events.h
struct MacRegion;        // Quickdraw.h
struct OpaqueRgnHandle;  // Quickdraw.h
struct Rect;             // Quickdraw.h


namespace Pedestal
{
	
	// same as ::RgnHandle but without the include
	
#if !OPAQUE_TOOLBOX_STRUCTS
	
	typedef MacRegion** RgnHandle;
	
#else
	
	typedef OpaqueRgnHandle* RgnHandle;
	
#endif
	
	class Quasimode;
	
	class View : public plus::ref_count< View >
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
			
			virtual boost::intrusive_ptr< Quasimode > EnterShiftSpaceQuasimode( const EventRecord& );
			
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
			
			virtual bool UserCommand( CommandCode code  )  { return false; }
	};
	
	
	inline boost::intrusive_ptr< View > seize_ptr( View* view )
	{
		return boost::intrusive_ptr< View >( view );
	}
	
}

#endif

