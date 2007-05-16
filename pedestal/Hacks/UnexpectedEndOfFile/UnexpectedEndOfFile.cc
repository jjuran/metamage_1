/*
	
	Unexpected End Of File
	
	Joshua Juran
	
*/

// Universal Interfaces
#include <Events.h>
#include <MacWindows.h>
#include <Menus.h>
#include <Quickdraw.h>
#include <Resources.h>
#include <Sound.h>

// Silver
#include "Silver/Patches.hh"

// UnexpectedEndOfFile
#include "UEOFUtils.hh"


using namespace Silver;


static PatchApplied<MenuSelectPatch> gMenuSelectPatch;
static PatchApplied<MenuKeyPatch>    gMenuKeyPatch;
static PatchApplied<InsertMenuPatch> gInsertMenuPatch;

static StillDownProcPtr gOriginalStillDown;

static Point gLastMouseLoc;

struct QuitInfo
{
	ProcessSerialNumber  psn;
	unsigned             menuCode;
};

enum { stashSize = 20 };

static unsigned stashCount;
static QuitInfo gStash[ stashSize ];


static MenuHandle GetAppMenuHandle()
{
	const UInt16 appMenuID = 0xbf97;
	
	MenuHandle appMenu = GetMenuHandle( appMenuID );
	
	return appMenu;
}

static bool StashFull()
{
	return stashCount >= stashSize;
}

static QuitInfo* NextStashSlot()
{
	if ( StashFull() )
	{
		return NULL;
	}
	
	return &gStash[ stashCount++ ];
}

static QuitInfo* FindPSNInStash(const ProcessSerialNumber& psn)
{
	for ( int i = 0; i < stashCount; ++i )
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

static pascal Boolean PatchedStillDown()
{
	MyA4 a4;
	
	StillDownProcPtr original = gOriginalStillDown;
	
	bool stillDown = original();
	
	if ( !stillDown )
	{
		GetMouse( &gLastMouseLoc );
	}
	
	return stillDown;
}


static pascal long PatchedMenuKey( short c )
{
	MyA4 a4;
	
	MenuKeyProcPtr original = gMenuKeyPatch.Original();
	
	if ( (c | ' ') == 'q' )
	{
		return GetQuitInfo();
	}
	
	return original( c );
}

static pascal long PatchedMenuSelect( Point startPt )
{
	MyA4 a4;
	
	MenuSelectProcPtr original = gMenuSelectPatch.Original();
	
	if ( IsFinderInForeground() )
	{
		return original( startPt );
	}
	
	MenuHandle appMenu = GetAppMenuHandle();
	
	short count = CountMenuItems( appMenu );
	
	::AppendMenu( appMenu, "\p" "Quit/Q" );
	
	TemporaryPatchApplied< StillDownPatch > stillDownPatch( PatchedStillDown );
	gOriginalStillDown = stillDownPatch.Original();
	
	long result = original( startPt );
	
	Handle menuProcH = appMenu[0]->menuProc;
	short state = ::HGetState( menuProcH );
	::HLock( menuProcH );
	
	MDEFProcPtr mdefProc = reinterpret_cast< MDEFProcPtr >( *menuProcH );
	
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

static pascal void PatchedInsertMenu( MenuHandle menu, short beforeID )
{
	MyA4 a4;
	
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
	
	InsertMenuProcPtr original = gInsertMenuPatch.Original();
	
	original( menu, beforeID );
}


static bool Install()
{
	bool locked = LoadAndLock();
	
	if ( !locked )
	{
		return false;
	}
	
	MyA4 a4;
	
	stashCount = 0;
	
	gMenuSelectPatch = MenuSelectPatch(PatchedMenuSelect);
	gMenuKeyPatch    = MenuKeyPatch(PatchedMenuKey);
	gInsertMenuPatch = InsertMenuPatch(PatchedInsertMenu);
	
	return true;
}

void main()
{
	bool installed = Install();
}

