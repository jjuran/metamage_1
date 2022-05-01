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

// Standard C/C++
#include <cstring>

// Silver
#include "Silver/Install.hh"
#include "Silver/Patch.hh"
#include "Silver/Procs.hh"
#include "Silver/Traps.hh"


#pragma exceptions off


namespace Ag = Silver;

using namespace Ag::Trap_ProcPtrs;


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
	GrafPtr wMgrPort;
	
	GetWMgrPort( &wMgrPort );
	Rect rect = wMgrPort->portRect;
	
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
	CGrafPtr cWMgrPort;
	Rect rect;
	RGBColor saveColor, menuColor;
	long len = std::strlen( str );
	
	GetPort( &savePort );
	GetCWMgrPort( &cWMgrPort );
	short mbarHeight = GetMBarHeight();
	
	rect.top = 1;
	rect.bottom = mbarHeight - 2;
	rect.left = 8;
	rect.right = cWMgrPort->portRect.right - 8;
	
	SetPort( (GrafPtr) cWMgrPort );
	GetBackColor( &saveColor );
	
	GetCPixel( 5, 5, &menuColor );
	RGBBackColor( &menuColor );
	
	EraseRect( &rect );
	
	MoveTo( 8, mbarHeight - 6 );
	DrawText( str, 0, len );
	
	RGBBackColor( &saveColor );
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

namespace
{
	
	short PatchedGetNextEvent( EventMask eventMask, EventRecord* theEvent, GetNextEventProcPtr nextHandler )
	{
		bool result = nextHandler( eventMask, theEvent );
		
		Payload( theEvent );
		
		return result;
	}
	
}

static OSErr Installer()
{
	Ag::TrapPatch< _GetNextEvent, PatchedGetNextEvent >::Install();
	
	return noErr;
}

int main()
{
	return Ag::Install( Installer );
}
