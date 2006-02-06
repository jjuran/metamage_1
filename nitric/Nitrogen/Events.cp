// Events.cp

#ifndef NITROGEN_EVENTS_H
#include "Nitrogen/Events.h"
#endif


namespace Nitrogen
{
	
	Point GetMouse()
	{
		Point pt;
		::GetMouse( &pt );
		return pt;
	}
	
	bool Button()
	{
		return ::Button();
	}
	
	bool StillDown()
	{
		return ::StillDown();
	}
	
	bool WaitMouseUp()
	{
		return ::WaitMouseUp();
	}
	
	GetKeys_Result GetKeys()
	{
		GetKeys_Result result;
		::GetKeys( result.keyMap );
		
		return result;
	}
	
	EventRecord GetNextEvent( EventMask eventMask )
	{
		EventRecord event;
		::GetNextEvent( eventMask, &event );
		return event;
	}
	
	EventRecord WaitNextEvent( EventMask eventMask, UInt32 sleep, RgnHandle mouseRgn )
	{
		EventRecord event;
		::WaitNextEvent( eventMask, &event, sleep, mouseRgn );
		return event;
	}
	
	EventRecord EventAvail( EventMask eventMask )
	{
		EventRecord event;
		::EventAvail( eventMask, &event );
		return event;
	}
	
	void FlushEvents( EventMask whichMask, EventMask stopMask )
	{
		::FlushEvents( whichMask, stopMask );
	}
	
	Point GetGlobalMouse()
	{
	#if TARGET_API_MAC_CARBON
		
		Point result;
		::GetGlobalMouse( &result );
		return result;
		
	#else
		
		return LocalToGlobal( GetMouse() );
		
	#endif
	}
	
	KeyModifiers GetCurrentKeyModifiers()
	{
	#if TARGET_API_MAC_CARBON
		
		return KeyModifiers( ::GetCurrentKeyModifiers() );
		
	#else
		
		return KeyModifiers( *(UInt16 *)0x017A << 9 & 0xFF00 );
		
	#endif
	}
	
	bool CheckEventQueueForUserCancel()
	{
	#if TARGET_API_MAC_CARBON
		
		return ::CheckEventQueueForUserCancel();
		
	#else
		
		bool canceled = false;
		
		QHdrPtr eventQueueHeader = ::GetEvQHdr();
		
		for ( EvQElPtr eventQueuePtr =  reinterpret_cast< EvQElPtr >( eventQueueHeader->qHead );
		      eventQueuePtr          != NULL  &&  !canceled;
		      eventQueuePtr          =  reinterpret_cast< EvQElPtr >( eventQueuePtr->qLink ) )
		{
			if ( eventQueuePtr->evtQWhat == keyDown )
			{
				bool command = eventQueuePtr->evtQModifiers & cmdKey;
				char keyChar = eventQueuePtr->evtQMessage   & charCodeMask;
				
				if ( command && keyChar == '.' )
				{
					canceled = true;
				}
				else if ( keyChar == kEscapeCharCode )
				{
					char keyCode = ( eventQueuePtr->evtQMessage & keyCodeMask ) >> 8;
					
					if ( keyCode == 0x35 )
					{
						canceled = true;
					}
				}
			}
		}
		
		if ( canceled )
		{
			// Remove mouse and key events
		}
		
		return canceled;
		
	#endif
	}
	
}

