// Nitrogen/Events.hh
// ------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2007 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_EVENTS_HH
#define NITROGEN_EVENTS_HH

#ifndef __EVENTS__
#include <Events.h>
#endif

#if !TARGET_API_MAC_CARBON
#include "Carbonate/Events.hh"
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/flag_ops.hh"

#ifndef NITROGEN_OSUTILS_HH
#include "Nitrogen/OSUtils.hh"
#endif
#ifndef NITROGEN_QUICKDRAW_HH
#include "Nitrogen/Quickdraw.hh"
#endif
#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif

namespace Nitrogen
{
	
	enum EventKind
	{
		nullEvent       = ::nullEvent,
		mouseDown       = ::mouseDown,
		mouseUp         = ::mouseUp,
		keyDown         = ::keyDown,
		keyUp           = ::keyUp,
		autoKey         = ::autoKey,
		updateEvt       = ::updateEvt,
		diskEvt         = ::diskEvt,
		activateEvt     = ::activateEvt,
		osEvt           = ::osEvt,
		kHighLevelEvent = ::kHighLevelEvent,
		
		kEventKind_Max = nucleus::enumeration_traits< ::EventKind >::max
	};
	
	
	enum EventMask
	{
		mDownMask          = ::mDownMask,
		mUpMask            = ::mUpMask,
		keyDownMask        = ::keyDownMask,
		keyUpMask          = ::keyUpMask,
		autoKeyMask        = ::autoKeyMask,
		updateMask         = ::updateMask,
		diskMask           = ::diskMask,
		activMask          = ::activMask,
		highLevelEventMask = ::highLevelEventMask,
		osMask             = ::osMask,
		everyEvent         = ::everyEvent,
		
		kEventMask_Max = nucleus::enumeration_traits< ::EventMask >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( EventMask )
	
	
	enum EventModifiers
	{
		activeFlag      = ::activeFlag,
		btnState        = ::btnState,
		cmdKey          = ::cmdKey,
		shiftKey        = ::shiftKey,
		alphaLock       = ::alphaLock,
		optionKey       = ::optionKey,
		controlKey      = ::controlKey,
		rightShiftKey   = ::rightShiftKey,
		rightOptionKey  = ::rightOptionKey,
		rightControlKey = ::rightControlKey,
		
		kEventModifiers_Max = nucleus::enumeration_traits< ::EventModifiers >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( EventModifiers )
	
	
	// KeyModifiers must be distinct from EventModifiers since it's a different size.
	
	enum KeyModifiers
	{
		kKeyModifiers_Max = nucleus::enumeration_traits< ::UInt32 >::max
	};
	
	// There's no point defining operators if there are no constants.
	//NUCLEUS_DEFINE_FLAG_OPS( KeyModifiers )
	
	// ...
	
	Point GetMouse();
	
	bool Button();
	bool StillDown();
	bool WaitMouseUp();
	
	// ...
	
	union GetKeys_Result
	{
		KeyMap           keyMap;
		KeyMapByteArray  keyMapByteArray;
	};
	
	GetKeys_Result GetKeys();
	
	// ...
	
	inline bool IsNullEvent( const EventRecord& event )
	{
		return event.what == static_cast< ::EventKind >( nullEvent );
	}
	
	EventRecord GetNextEvent( EventMask eventMask );
	
	EventRecord WaitNextEvent( EventMask eventMask, UInt32 sleep, RgnHandle mouseRgn = NULL );
	
	EventRecord EventAvail( EventMask eventMask );
	
	// ...
	
	void FlushEvents( EventMask whichMask, EventMask stopMask );
	
	// ...
	
	Point GetGlobalMouse();
	
	KeyModifiers GetCurrentKeyModifiers();
	
	bool CheckEventQueueForUserCancel();
	
	// ...
	
}

#endif

