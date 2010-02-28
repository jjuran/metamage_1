// Nitrogen/Events.cc
// ------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/Events.hh"


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
		Point result;
		::GetGlobalMouse( &result );
		
		return result;
	}
	
	KeyModifiers GetCurrentKeyModifiers()
	{
		return KeyModifiers( ::GetCurrentKeyModifiers() );
	}
	
	bool CheckEventQueueForUserCancel()
	{
		return ::CheckEventQueueForUserCancel();
	}
	
}

