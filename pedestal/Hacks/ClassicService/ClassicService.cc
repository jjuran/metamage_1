/*
	
	Classic Service
	
	Joshua Juran
	
*/

// Universal Interfaces
#include <Events.h>
#include <LowMem.h>
#include <MacWindows.h>
#include <Menus.h>
#include <Quickdraw.h>
#include <Resources.h>
#include <Sound.h>
#include <ToolUtils.h>

// Silver
#include "Silver/Patches.hh"


namespace ClassicService
{
	
	using namespace Silver;
	
	
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
	
	short gLastCheckedMenuID;
	TEHandle gTE;
	
	static PatchApplied< AppendResMenuPatch > gAppendResMenuPatch;
	static PatchApplied< MenuSelectPatch    > gMenuSelectPatch;
	static PatchApplied< MenuKeyPatch       > gMenuKeyPatch;
	static PatchApplied< TEActivatePatch    > gTEActivatePatch;
	static PatchApplied< ExitToShellPatch   > gExitToShellPatch;
	
	
	static ServicesMenuItem* FindServicesMenuItem( long key )
	{
		ServicesMenuItem* end = gServicesMenuItems + itemCount;
		
		for ( ServicesMenuItem* result = gServicesMenuItems;  result != end;  ++result )
		{
			if ( result->key == key )  return result;
		}
		
		return NULL;
	}
	
	inline ServicesMenuItem* NewServicesMenuItem()
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
	
	
	inline long UniqueKey()
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
		while ( gLastCheckedMenuID <= 255 )
		{
			++gLastCheckedMenuID;
			
			if ( ::GetMenuHandle( gLastCheckedMenuID ) == NULL )
			{
				return gLastCheckedMenuID;
			}
		}
		return 0;
	}
	
	static MenuRef InstallServicesMenu()
	{
		gLastCheckedMenuID = 0;
		
		ServicesMenuItem* servicesMenuItem = FindServicesMenuItem( UniqueKey() );
		
		if ( servicesMenuItem == NULL )  return NULL;
		
		short servicesMenuID = GetFreeMenuID();
		
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
	
	
	static pascal void PatchedAppendResMenu( MenuRef menu, ResType type )
	{
		MyA4 a4;
		
		AppendResMenuProcPtr original = gAppendResMenuPatch.Original();
		
		if ( type == 'DRVR' )
		{
			InstallServicesMenuItem( UniqueKey(), menu );
		}
		
		original( menu, type );
	}
	
	static pascal void PatchedExitToShell()
	{
		MyA4 a4;
		
		RemoveServicesMenuItem( UniqueKey() );
		
		gExitToShellPatch.Original()();
	}
	
	
	static pascal long PatchedMenuSelect( Point startPt )
	{
		MyA4 a4;
		
		MenuSelectProcPtr original = gMenuSelectPatch.Original();
		
		MenuRef servicesMenu = InstallServicesMenu();
		
		long result = original( startPt );
		
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
	
	static pascal void PatchedTEActivate( TEHandle hTE )
	{
		MyA4 a4;
		
		gTE = hTE;
		
		gTEActivatePatch.Original()( hTE );
	}
	
	static pascal long PatchedMenuKey( short c )
	{
		MyA4 a4;
		
		MenuKeyProcPtr original = gMenuKeyPatch.Original();
		
		return original( c );
	}
	
	
	static bool Install()
	{
		bool locked = LoadAndLock();
		
		if ( !locked )
		{
			return false;
		}
		
		MyA4 a4;
		
		gServicesMenuItems[0].key = 0;
		gServicesMenuItems[1].key = 0;
		
		gTE = NULL;
		
		gAppendResMenuPatch = AppendResMenuPatch( PatchedAppendResMenu );
		gMenuSelectPatch    = MenuSelectPatch   ( PatchedMenuSelect    );
		//gMenuKeyPatch       = MenuKeyPatch      ( PatchedMenuKey       );
		gTEActivatePatch    = TEActivatePatch   ( PatchedTEActivate    );
		gExitToShellPatch   = ExitToShellPatch  ( PatchedExitToShell   );
		
		return true;
	}
	
}

void main()
{
	bool installed = ClassicService::Install();
}

