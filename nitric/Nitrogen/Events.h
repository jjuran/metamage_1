// Events.h

#ifndef NITROGEN_EVENTS_H
#define NITROGEN_EVENTS_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __EVENTS__
#include FRAMEWORK_HEADER(HIToolbox,Events.h)
#endif
#ifndef NITROGEN_OSUTILS_H
#include "Nitrogen/OSUtils.h"
#endif
#ifndef NITROGEN_QUICKDRAW_H
#include "Nitrogen/QuickDraw.h"
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NUCLEUS_FLAG_H
#include "Nucleus/Flag.h"
#endif

namespace Nitrogen
{
	
#ifdef JOSHUA_JURAN_EXPERIMENTAL
	
	typedef Nucleus::Selector< class EventKind_Tag, ::EventKind >::Type EventKind;
	
	static const EventKind nullEvent       = EventKind( ::nullEvent       );
	static const EventKind mouseDown       = EventKind( ::mouseDown       );
	static const EventKind mouseUp         = EventKind( ::mouseUp         );
	static const EventKind keyDown         = EventKind( ::keyDown         );
	static const EventKind keyUp           = EventKind( ::keyUp           );
	static const EventKind autoKey         = EventKind( ::autoKey         );
	static const EventKind updateEvt       = EventKind( ::updateEvt       );
	static const EventKind diskEvt         = EventKind( ::diskEvt         );
	static const EventKind activateEvt     = EventKind( ::activateEvt     );
	static const EventKind osEvt           = EventKind( ::osEvt           );
	static const EventKind kHighLevelEvent = EventKind( ::kHighLevelEvent );
	
	
#endif
	
	typedef Nucleus::Flag< class EventMask_Tag, ::EventMask >::Type EventMask;
	
	static const EventMask mDownMask          = EventMask( ::mDownMask          );
	static const EventMask mUpMask            = EventMask( ::mUpMask            );
	static const EventMask keyDownMask        = EventMask( ::keyDownMask        );
	static const EventMask keyUpMask          = EventMask( ::keyUpMask          );
	static const EventMask autoKeyMask        = EventMask( ::autoKeyMask        );
	static const EventMask updateMask         = EventMask( ::updateMask         );
	static const EventMask diskMask           = EventMask( ::diskMask           );
	static const EventMask activMask          = EventMask( ::activMask          );
	static const EventMask highLevelEventMask = EventMask( ::highLevelEventMask );
	static const EventMask osMask             = EventMask( ::osMask             );
	static const EventMask everyEvent         = EventMask( ::everyEvent         );
	
	
	typedef Nucleus::Flag< class EventModifiers_Tag, ::EventModifiers >::Type EventModifiers;
	
	typedef Nucleus::Flag< class KeyModifiers_Tag, UInt32 >::Type KeyModifiers;
	
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

