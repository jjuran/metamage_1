/*
	
	GeniusBar
	
	Joshua Juran
	
*/

// Mac OS
#ifndef __CONTROLS__
#include <Controls.h>
#endif
#ifndef __CONTROLDEFINITIONS__
#include <ControlDefinitions.h>
#endif
#ifndef __EVENTS__
#include <Events.h>
#endif
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#ifndef __PROCESSES__
#include <Processes.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __TRAPS__
#include <Traps.h>
#endif

// mac-sys-utils
#include "mac_sys/trap_address.hh"
#include "mac_sys/trap_available.hh"


#pragma exceptions off


GrafPtr WMgrPort : 0x09DE;


static const char* gLastString;


static bool IsFinderInForeground()
{
	ProcessSerialNumber psn;
	ProcessInfoRec procInfo;
	
	procInfo.processInfoLength = sizeof procInfo;
	procInfo.processName       = NULL;
	procInfo.processAppSpec    = NULL;
	
	OSErr err = GetFrontProcess(&psn);
	if (err) return false;
	
	err = GetProcessInformation(&psn, &procInfo);
	if (err) return false;
	
	return (procInfo.processSignature == 'MACS');
}

static bool IsPointingToTrash(Point pt)
{
	Rect rect = WMgrPort->portRect;
	
	rect.bottom -= 32;
	rect.right  -= 32;
	rect.top  = rect.bottom - 32;
	rect.left = rect.right  - 32;
	
	return PtInRect( pt, &rect );
}

static const char* GetGeniusString( EventRecord* theEvent )
{
	WindowRef wptr;
	const char* geniusMsg = "";
	
	short locationCode = FindWindow( theEvent->where, &wptr );
	
	switch ( locationCode )
	{
		case inDesk:
			if ( !IsFinderInForeground() )
			{
				geniusMsg = "Click here to switch to the Finder.";
			}
			else
			{
				if ( IsPointingToTrash( theEvent->where ) )
				{
					geniusMsg = "This is GeniusBar, a hack by Josh Juran.";
				}
				else
				{
					return NULL;
				}
			}
			break;
		
		case inMenuBar:
			geniusMsg = "Click here to choose from a menu.";
			break;
		
		case inDrag:
			geniusMsg = "Click here to move this window.";
			break;
		
		case inGrow:
			geniusMsg = "Click here to resize this window.";
			break;
		
		case inGoAway:
			geniusMsg = "Click here to close this window.";
			break;
		
		case inZoomIn:
			geniusMsg = "Click here to un-zoom this window.";
			break;
		
		case inZoomOut:
			geniusMsg = "Click here to zoom this window.";
			break;
		
		case inCollapseBox:
			geniusMsg = "Click here to collapse this window.";
			break;
		
		case inSysWindow:
			geniusMsg = "This is a system window.";
		
		case inContent:
			{
				ControlHandle control;
				Point pt = theEvent->where;
				GrafPtr savePort;
				
				GetPort( &savePort );
				SetPort( wptr );
				GlobalToLocal( &pt );
				short part = FindControl( pt, wptr, &control );
				SetPort( savePort );
				
				Rect rect = control[0]->contrlRect;
				
				bool vertical = rect.bottom - rect.top > rect.right - rect.left;
				
				switch ( part )
				{
					case kControlButtonPart:
						geniusMsg = "Click here to push this button.";
						break;
					
					case kControlCheckBoxPart:
						geniusMsg = "Click here to enable this option.";
						break;
					
					case kControlUpButtonPart:
						geniusMsg = vertical ? "Click here to scroll up."
						                     : "Click here to scroll left.";
						break;
					
					case kControlDownButtonPart:
						geniusMsg = vertical ? "Click here to scroll down."
						                     : "Click here to scroll right.";
						break;
					
					case kControlPageUpPart:
						geniusMsg = vertical ? "Click here to scroll up by a page."
						                     : "Click here to scroll left by a page.";
						break;
					
					case kControlPageDownPart:
						geniusMsg = vertical ? "Click here to scroll down by a page."
						                     : "Click here to scroll right by a page.";
						break;
					
					case kControlIndicatorPart:
						geniusMsg = "Click here to scroll to an arbitrary position.";
						break;
					
					default:
						return NULL;
						break;
				}
			}
			break;
		
		default:
			return NULL;
			break;
	}
	
	return geniusMsg;
}

static void DrawGeniusMessage( const char* str )
{
	GrafPtr savePort;
	GrafPtr wMgrPort = WMgrPort;
	Rect rect;
	RGBColor saveColor, menuColor;
	long len = std::strlen( str );
	
	const bool color = mac::sys::trap_available( _GetCWMgrPort );
	
	if ( color )
	{
		GetCWMgrPort( (CGrafPtr*) &wMgrPort );
	}
	
	GetPort( &savePort );
	short mbarHeight = GetMBarHeight();
	
	rect.top = 1;
	rect.bottom = mbarHeight - 2;
	rect.left = 8;
	rect.right = wMgrPort->portRect.right - 8;
	
	SetPort( wMgrPort );
	
	if ( color )
	{
		GetBackColor( &saveColor );
		
		GetCPixel( 5, 5, &menuColor );
		RGBBackColor( &menuColor );
	}
	
	EraseRect( &rect );
	
	MoveTo( 8, mbarHeight - 6 );
	DrawText( str, 0, len );
	
	if ( color )
	{
		RGBBackColor( &saveColor );
	}
	
	SetPort( savePort );
}

static void Payload( EventRecord* theEvent )
{
	if ( (theEvent->modifiers & btnState) == 0 )
	{
		InvalMenuBar();
	}
	else
	{
		const char* str = GetGeniusString( theEvent );
		
		if ( str != gLastString )
		{
			if ( str == NULL )
			{
				InvalMenuBar();
			}
			else
			{
				DrawGeniusMessage( str );
			}
			
			gLastString = str;
		}
	}
}

static UniversalProcPtr old_GetNextEvent;

typedef pascal short (*GetNextEventProcPtr)( EventMask, EventRecord* );

static
pascal
short GetNextEvent_patch( EventMask eventMask, EventRecord* theEvent )
{
	GetNextEventProcPtr nextHandler = (GetNextEventProcPtr) old_GetNextEvent;
	
	short result = nextHandler( eventMask, theEvent );
	
	Payload( theEvent );
	
	return result;
}

int main()
{
	Handle self = Get1Resource( 'INIT', 0 );
	
	DetachResource( self );
	
	old_GetNextEvent = mac::sys::get_trap_address( _GetNextEvent );
	
	mac::sys::set_trap_address( (ProcPtr) GetNextEvent_patch, _GetNextEvent );
	
	return 0;
}
