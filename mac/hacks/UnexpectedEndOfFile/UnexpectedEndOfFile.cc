/*
	
	Unexpected End Of File
	
	Joshua Juran
	
*/

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#ifndef __MENUS__
#include <Menus.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// Silver
#include "Silver/Install.hh"
#include "Silver/Patch.hh"
#include "Silver/Procs.hh"
#include "Silver/Traps.hh"

// UnexpectedEndOfFile
#include "UEOFUtils.hh"


#pragma exceptions off


namespace Ag = Silver;

using namespace Ag::Trap_ProcPtrs;


static Point gLastMouseLoc;

struct QuitInfo
{
	ProcessSerialNumber  psn;
	unsigned             menuCode;
};

enum { stashSize = 20 };

static unsigned stashCount;
static QuitInfo gStash[ stashSize ];


static inline MenuHandle GetAppMenuHandle()
{
	const UInt16 appMenuID = 0xbf97;
	
	MenuHandle appMenu = GetMenuHandle( appMenuID );
	
	return appMenu;
}

static inline bool StashIsFull()
{
	return stashCount >= stashSize;
}

static inline QuitInfo* NextStashSlot()
{
	if ( StashIsFull() )
	{
		return NULL;
	}
	
	return &gStash[ stashCount++ ];
}

static QuitInfo* FindPSNInStash( const ProcessSerialNumber& psn )
{
	for ( unsigned i = 0; i < stashCount; ++i )
	{
		if ( gStash[i].psn == psn )
		{
			return &gStash[i];
		}
	}
	
	return NULL;
}

static void StoreQuitInfo( MenuHandle menu, short item )
{
	UInt32 code = menu[0]->menuID << 16  |  item;
	
	ProcessSerialNumber psn = CurrentPSN();
	
	QuitInfo* info = FindPSNInStash( psn );
	
	if ( info == NULL )
	{
		info = NextStashSlot();
	}
	
	if ( info != NULL )
	{
		info->psn      = psn;
		info->menuCode = code;
	}
}

static unsigned GetQuitInfo()
{
	ProcessSerialNumber psn = CurrentPSN();
	
	QuitInfo* info = FindPSNInStash( psn );
	
	if ( info == NULL )
	{
		return 0;
	}
	
	return info->menuCode;
}

namespace
{
	
	Boolean PatchedStillDown( StillDownProcPtr nextHandler )
	{
		bool stillDown = nextHandler();
		
		if ( !stillDown )
		{
			GetMouse( &gLastMouseLoc );
		}
		
		return stillDown;
	}
	
	
	long PatchedMenuKey( short c, MenuKeyProcPtr nextHandler )
	{
		if ( (c | ' ') == 'q' )
		{
			return GetQuitInfo();
		}
		
		return nextHandler( c );
	}
	
	long PatchedMenuSelect( Point startPt, MenuSelectProcPtr nextHandler )
	{
		if ( IsFinderInForeground() )
		{
			return nextHandler( startPt );
		}
		
		MenuHandle appMenu = GetAppMenuHandle();
		
		short count = CountMenuItems( appMenu );
		
		::AppendMenu( appMenu, "\p" "Quit/Q" );
		
		//TemporaryPatchApplied< StillDownPatch > stillDownPatch( PatchedStillDown );
		//gOriginalStillDown = stillDownPatch.Original();
		
		typedef Ag::TrapPatch< _StillDown, PatchedStillDown > StillDownPatch;
		
		StillDownPatch::Install();
		
		long result = nextHandler( startPt );
		
		StillDownPatch::Remove();
		
		Handle menuProcH = appMenu[0]->menuProc;
		short state = ::HGetState( menuProcH );
		::HLock( menuProcH );
		
		MenuDefProcPtr mdefProc = reinterpret_cast< MenuDefProcPtr >( *menuProcH );
		
		GrafPtr wMgrPort;
		GetWMgrPort( &wMgrPort );
		
		Rect rect;
		rect.top = 20;
		rect.right = wMgrPort->portRect.right - 11;
		rect.bottom = rect.top + appMenu[0]->menuHeight;
		rect.left = rect.right - appMenu[0]->menuWidth;
		
		short whichItem;
		
		{
			ThingWhichPreventsMenuItemDrawing thing;
			
			mdefProc( mChooseMsg, appMenu, &rect, gLastMouseLoc, &whichItem );
		}
		
		::HSetState( menuProcH, state );
		
		::DeleteMenuItem( appMenu, count + 1 );
		
		if ( whichItem > count )
		{
			return GetQuitInfo();
		}
		
		return result;
	}
	
	void PatchedInsertMenu( MenuHandle menu, short beforeID, InsertMenuProcPtr nextHandler )
	{
		if ( EqualPstrings( menu[0]->menuData, "\p" "File" ) )
		{
			short count = CountMenuItems( menu );
			
			Str255 itemName;
			::GetMenuItemText( menu, count, itemName );
			
			if ( EqualPstrings( itemName, "\p" "Quit" ) )
			{
				StoreQuitInfo( menu, count );
				
				::DeleteMenuItem( menu, count );
				--count;
				
				::GetMenuItemText( menu, count, itemName );
				
				if ( itemName[1] == '-' )
				{
					::DeleteMenuItem( menu, count );
				}
			}
		}
		
		nextHandler( menu, beforeID );
	}
	
}


static OSErr Installer()
{
	stashCount = 0;
	
	Ag::TrapPatch< _MenuSelect, PatchedMenuSelect >::Install();
	Ag::TrapPatch< _InsertMenu, PatchedInsertMenu >::Install();
	Ag::TrapPatch< _MenuKey,    PatchedMenuKey    >::Install();
	
	return noErr;
}

int main()
{
	return Ag::Install( Installer );
}
