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
#ifndef NUCLEUS_FLAGTYPE_H
#include "Nucleus/FlagType.h"
#endif

namespace Nitrogen
{
	
#ifdef JOSHUA_JURAN_EXPERIMENTAL
	
	class EventKind_Tag {};
	typedef Nucleus::SelectorType< EventKind_Tag, ::EventKind, nullEvent > EventKind;
	
#endif
	
	class EventMask_Tag {};
	typedef Nucleus::FlagType< EventMask_Tag, ::EventMask > EventMask;
	
	class EventModifiers_Tag {};
	typedef Nucleus::FlagType< EventModifiers_Tag, ::EventModifiers > EventModifiers;
	
	class KeyModifiers_Tag {};
	typedef Nucleus::FlagType< KeyModifiers_Tag, ::UInt32 > KeyModifiers;
	
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
		return event.what == static_cast < ::EventKind > ( nullEvent );
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

