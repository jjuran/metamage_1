/*
	
	Classic Service
	
	Joshua Juran
	
*/

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif
#ifndef __LOWMEM__
#include <LowMem.h>
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
#ifndef __TOOLUTILS__
#include <ToolUtils.h>
#endif

// Silver
#include "Silver/Install.hh"
#include "Silver/Patch.hh"
#include "Silver/Procs.hh"
#include "Silver/Traps.hh"


#pragma exceptions off


namespace ClassicService
{
	
	namespace Ag = Silver;
	
	using namespace Ag::Trap_ProcPtrs;
	
	
	struct ServicesMenuItem
	{
		long     key;
		MenuRef  parentMenu;  // Apple menu
		short    index;    // index into parent menu
	};
	
	enum
	{
		itemCount = 4
	};
	
	ServicesMenuItem gServicesMenuItems[ itemCount ];
	
	TEHandle gTE;
	
	
	static ServicesMenuItem* FindServicesMenuItem( long key )
	{
		ServicesMenuItem* end = gServicesMenuItems + itemCount;
		
		for ( ServicesMenuItem* result = gServicesMenuItems;  result != end;  ++result )
		{
			if ( result->key == key )  return result;
		}
		
		return NULL;
	}
	
	static inline ServicesMenuItem* NewServicesMenuItem()
	{
		return FindServicesMenuItem( 0 );
	}
	
	static char rot13( char c )
	{
		char lower = c | ' ';
		
		if ( lower >= 'a'  &&  lower <= 'z' )
		{
			char rotation = lower < 'n' ? 13 : -13;
			
			c += rotation;
		}
		
		return c;
	}
	
	
	static inline long GetUniqueKeyForApp()
	{
		return (long) LMGetApplZone();
	}
	
	static void PerformService( short index )
	{
		if ( index != 1 )  return;
		
		if ( gTE == NULL )  return;
		
		::FlashMenuBar( 0 );
		::FlashMenuBar( 0 );
		::FlashMenuBar( 0 );
		::FlashMenuBar( 0 );
		
		short start = gTE[0]->selStart;
		short end   = gTE[0]->selEnd;
		Handle text = gTE[0]->hText;
		
		short selectionLength = end - start;
		
		Ptr tempPtr = NewPtr( selectionLength );
		
		if ( tempPtr == NULL )
		{
			return;
		}
		
		BlockMoveData( *text + start, tempPtr, selectionLength );
		
		for ( unsigned i = 0;  i < selectionLength;  ++i )
		{
			tempPtr[i] = rot13( tempPtr[i] );
		}
		
		TEDelete( gTE );
		
		TEInsert( tempPtr, selectionLength, gTE );
		
		DisposePtr( tempPtr );
		
	}
	
	static void InstallServicesMenuItem( long key, MenuRef parentMenu )
	{
		ServicesMenuItem* servicesMenuItem = NewServicesMenuItem();
		
		if ( servicesMenuItem == NULL )  return;
		
		servicesMenuItem->key        = key;
		servicesMenuItem->parentMenu = parentMenu;
		servicesMenuItem->index      = ::CountMenuItems( parentMenu ) + 1;
		
		::AppendMenu( parentMenu, "\p  Services" );
		
		::AppendMenu( parentMenu, "\p  -" );
		// set key to hierarchicalMenu
		::SetItemCmd( parentMenu, servicesMenuItem->index, hMenuCmd );
	}
	
	static void RemoveServicesMenuItem( long key )
	{
		ServicesMenuItem* it = FindServicesMenuItem( key );
		
		if ( it != NULL )
		{
			it->key = 0;
		}
	}
	
	static short GetFreeMenuID()
	{
		for ( short menuID = 1;  menuID <= 255;  ++menuID )
		{
			if ( ::GetMenuHandle( menuID ) == NULL )
			{
				return menuID;
			}
		}
		return 0;
	}
	
	static MenuRef InstallServicesMenu()
	{
		ServicesMenuItem* servicesMenuItem = FindServicesMenuItem( GetUniqueKeyForApp() );
		
		if ( servicesMenuItem == NULL )  return NULL;
		
		short servicesMenuID = GetFreeMenuID();
		
		if ( servicesMenuID == 0 )  return NULL;
		
		MenuRef servicesMenu = ::NewMenu( servicesMenuID, "\pServices" );
		
		::InsertMenu( servicesMenu, kInsertHierarchicalMenu );
		
		MenuRef parentMenu = servicesMenuItem->parentMenu;
		
		short itemIndex = servicesMenuItem->index;
		
		// set key to hierarchicalMenu
		::SetItemCmd( parentMenu, itemIndex, hMenuCmd );
		
		// set mark to the menu ID
		::SetItemMark( parentMenu, itemIndex, servicesMenuID );
		
		::SetMenuItemText( parentMenu, itemIndex,     "\p" "Services" );
		::SetMenuItemText( parentMenu, itemIndex + 1, "\p" "-"        );
		
		::AppendMenu( servicesMenu, "\pRot13" );
		
		return servicesMenu;
	}
	
	static void RemoveServicesMenu( MenuRef servicesMenu )
	{
		::DeleteMenu ( servicesMenu[0]->menuID );
		::DisposeMenu( servicesMenu            );
	}
	
	namespace
	{
		
		void PatchedAppendResMenu( MenuRef menu, ResType type, AppendResMenuProcPtr nextHandler )
		{
			if ( type == 'DRVR' )
			{
				InstallServicesMenuItem( GetUniqueKeyForApp(), menu );
			}
			
			nextHandler( menu, type );
		}
		
		void PatchedExitToShell( ExitToShellProcPtr nextHandler )
		{
			RemoveServicesMenuItem( GetUniqueKeyForApp() );
			
			nextHandler();
		}
		
		
		long PatchedMenuSelect( Point startPt, MenuSelectProcPtr nextHandler )
		{
			MenuRef servicesMenu = InstallServicesMenu();
			
			long result = nextHandler( startPt );
			
			if ( servicesMenu != NULL )
			{
				if ( HiWord( result ) == servicesMenu[0]->menuID )
				{
					PerformService( LoWord( result ) );
				}
				
				RemoveServicesMenu( servicesMenu );
			}
			
			return result;
			
			return result;
		}
		
		void PatchedTEActivate( TEHandle hTE, TEActivateProcPtr nextHandler )
		{
			gTE = hTE;
			
			nextHandler( hTE );
		}
		
		long PatchedMenuKey( short c, MenuKeyProcPtr nextHandler )
		{
			return nextHandler( c );
		}
		
	}
	
	static inline bool WindowManagerInitialized()
	{
		const UInt8& WWExist = *(UInt8*) 0x08f2;
		
		return WWExist == 0;
	}
	
	static OSErr Installer()
	{
		if ( WindowManagerInitialized() )
		{
			return 0x00ff;
		}
		
		gServicesMenuItems[0].key = 0;
		gServicesMenuItems[1].key = 0;
		
		gTE = NULL;
		
		Ag::TrapPatch< _AppendResMenu, PatchedAppendResMenu >::Install();
		Ag::TrapPatch< _MenuSelect,    PatchedMenuSelect    >::Install();
		//Ag::TrapPatch< _MenuKey,       PatchedMenuKey       >::Install();
		Ag::TrapPatch< _TEActivate,    PatchedTEActivate    >::Install();
		Ag::TrapPatch< _ExitToShell,   PatchedExitToShell   >::Install();
		
		return noErr;
	}
	
}

int main()
{
	return Silver::Install( ClassicService::Installer );
}
