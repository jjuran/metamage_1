/*
	
	Classic Service
	
	Joshua Juran
	
*/

// Universal Interfaces
#include <Events.h>
#include <MacWindows.h>
#include <Menus.h>
#include <Quickdraw.h>
#include <Resources.h>
#include <Sound.h>
#include <ToolUtils.h>

// Standard C++
#include <string>
#include <vector>

// Silver
#include "Patches.hh"

// ClassicService
#include "FileIO.hh"


namespace ClassicService
{
	
	//namespace N = Nitrogen;
	
	using namespace Silver;
	
	using std::string;
	
	
	MenuRef gAppleMenu;
	short gServicesMenuItem;
	short gLastCheckedMenuID;
	short gServicesMenuID;
	TEHandle gTE;
	
	static PatchApplied< AppendResMenuPatch > gAppendResMenuPatch;
	static PatchApplied< MenuSelectPatch    > gMenuSelectPatch;
	static PatchApplied< MenuKeyPatch       > gMenuKeyPatch;
	static PatchApplied< TEActivatePatch    > gTEActivatePatch;
	static PatchApplied< SystemMenuPatch    > gSystemMenuPatch;
	
	static char rot13( char c )
	{
		char lower = c | ' ';
		if ( lower >= 'a'  ||  lower <= 'z' )
		{
			if ( lower >= 'n' )
			{
				c = c - 13;
			}
			else
			{
				c = c + 13;
			}
		}
		return c;
	}
	
	static void Payload()
	{
		::FlashMenuBar( 0 );
		::FlashMenuBar( 0 );
		::FlashMenuBar( 0 );
		::FlashMenuBar( 0 );
		short start = (**gTE).selStart;
		short end   = (**gTE).selEnd;
		Handle text = (**gTE).hText;
		
		long size = GetHandleSize( text );
		Handle temp = NewHandle( end - start );
		BlockMoveData( *text + start, *temp, end - start );
		
		TEKey( 8, gTE );
		HLock( temp );
		
		for ( short i = 0;  i < end - start;  ++i )
		{
			//char& c = (*text)[i];
			TEKey( rot13( (*temp)[i] ), gTE );
		}
		
		DisposeHandle( temp );
		
	}
	
	static pascal void PatchedSystemMenu( long menuResult )
	{
		MyA4 a4;
		
		if ( HiWord( menuResult ) == gServicesMenuID )
		{
			if ( LoWord( menuResult ) == 1  &&  gTE != NULL )
			{
				Payload();
			}
		}
		else
		{
			gSystemMenuPatch.Original()( menuResult );
		}
		
	}
	
	static pascal void PatchedTEActivate( TEHandle hTE )
	{
		MyA4 a4;
		
		gTEActivatePatch.Original()( hTE );
		
		gTE = hTE;
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
	
	static pascal void PatchedAppendResMenu( MenuRef menu, ResType type )
	{
		MyA4 a4;
		
		AppendResMenuProcPtr original = gAppendResMenuPatch.Original();
		
		if ( type == 'DRVR' )
		{
			::AppendMenu( menu, "\pServices" );
			gAppleMenu = menu;
			gServicesMenuItem = ::CountMenuItems( menu );
			::AppendMenu( menu, "\p-" );
			// set key to hierarchicalMenu
			::SetItemCmd( menu, gServicesMenuItem, hMenuCmd );
			
			//GetServiceInfo();
			
		}
		return original( menu, type );
	}
	
	static pascal long PatchedMenuKey(short c)
	{
		MyA4 a4;
		
		MenuKeyProcPtr original = gMenuKeyPatch.Original();
		
		return original(c);
	}
	
	static pascal long PatchedMenuSelect(Point startPt)
	{
		MyA4 a4;
		
		MenuSelectProcPtr original = gMenuSelectPatch.Original();
		
		gLastCheckedMenuID = 0;
		
		gServicesMenuID = GetFreeMenuID();
		MenuRef servicesMenu = ::NewMenu( gServicesMenuID, "\pServices" );
		::InsertMenu( servicesMenu, kInsertHierarchicalMenu );
		
		// set key to hierarchicalMenu
		::SetItemCmd( gAppleMenu, gServicesMenuItem, hMenuCmd );
		// set mark to the menu ID
		::SetItemMark( gAppleMenu, gServicesMenuItem, gServicesMenuID );
		
		/*
		for ( int i = 0;  i < gMenuItemNames->size();  ++i )
		{
			short menuID = GetFreeMenuID();
			
			const string& s = (*gMenuItemNames)[ i ];
			
			::AppendMenu( servicesMenu, N::Str255( s.substr( 0, 255 ) ) );
		}
		*/
		
		::AppendMenu( servicesMenu, "\pRot13" );
		
		long result = original( startPt );
		
		if ( HiWord( result ) == gServicesMenuID )
		{
			if ( LoWord( result ) == 1  &&  gTE != NULL )
			{
				Payload();
			}
		}
		
		::DeleteMenu( gServicesMenuID );
		::DisposeMenu( servicesMenu );
		
		/*
		if ( HiWord( result ) == gServicesMenuID )
		{
			if ( LoWord( result ) == 1  &&  gTE != NULL )
			{
				short start = (**gTE).selStart;
				short end   = (**gTE).selEnd;
				Handle text = (**gTE).hText;
				for ( short i = start;  i < end;  ++i )
				{
					//char& c = (*text)[i];
					(*text)[i] = rot13( (*text)[i] );
				}
				(*text)[2] = '¥';
			}
			result = 0;
		}
		*/
		
		return result;
		
		/*
		
		MenuHandle appMenu = GetAppMenuHandle();
		short count = CountMenuItems(appMenu);
		short iServicesItem = count + 1;
		AppendMenu(appMenu, "\pServices");
		MenuRef servicesMenu = ::NewMenu( kServicesMenuID, "\pServices" );
		::InsertMenu( servicesMenu, kInsertHierarchicalMenu );
		// set key to hierarchicalMenu
		::SetItemCmd( appMenu, iServicesItem, hMenuCmd );
		// set mark to kServicesMenuID
		::SetItemMark( appMenu, iServicesItem, kServicesMenuID );
		
		::AppendMenu( servicesMenu, "\pDo Script");
		
		TemporaryPatchApplied<StillDownPatch> stillDownPatch(PatchedStillDown);
		gOriginalStillDown = stillDownPatch.Original();
		
		long result = original( startPt );
		
		Handle menuProcH = (*appMenu)->menuProc;
		short state = HGetState(menuProcH);
		HLock(menuProcH);
		
		MDEFProcPtr mdefProc = reinterpret_cast<MDEFProcPtr>(*menuProcH);
		GrafPtr wMgrPort;
		GetWMgrPort(&wMgrPort);
		Rect rect;
		rect.top = 20;
		rect.right = wMgrPort->portRect.right - 11;
		rect.bottom = rect.top + (*appMenu)->menuHeight;
		rect.left = rect.right - (*appMenu)->menuWidth;
		short whichItem;
		
		{
			ThingWhichPreventsMenuItemDrawing thing;
			
			mdefProc(mChooseMsg, appMenu, &rect, gLastMouseLoc, &whichItem);
		}
		
		HSetState(menuProcH, state);
		
		::DeleteMenuItem( appMenu, iServicesItem );
		::DisposeMenu( servicesMenu );
		
		*/
		
		return result;
	}
	
	
	static void StoreServiceMenuItem( const string& menuItemName )
	{
		gMenuItemNames->push_back( menuItemName );
	}
	
	/*
	static string ReadFileData( const FSSpec& file )
	{
		N::Owned< V::FileHandle > fileH( V::OpenFileReadOnly( file ) );
		unsigned fileSize = IO::GetEOF(fileH);
		string data;
		data.resize( fileSize );
		int bytes = IO::Read(fileH, &data[0], fileSize);
		//Assert_(bytes == fileSize);
		return data;
	}
	
	static void DoSomethingWithServiceFile( const FSSpec& file )
	{
		FSSpec infoPListFile = file + "Contents" + "Info.plist";
		string infoPList = ReadFileData( infoPListFile );
		string::size_type iNSMenuItem = infoPList.find( "<key>NSMenuItem</key>" );
		if ( iNSMenuItem == string::npos )
		{
			return;
		}
		string::size_type iLast = iNSMenuItem;
		while ( true )
		{
			string::size_type iDefault = infoPList.find( "<key>default</key>", iLast );
			if ( iDefault == string::npos )
			{
				break;
			}
			string stringElement = "<string>";
			string::size_type iString = infoPList.find( stringElement, iDefault );
			string::size_type iValue = iString + stringElement.size();
			string::size_type iEndString = infoPList.find( "</string>", iValue );
			string value = infoPList.substr( iValue, iEndString - iValue );
			
			StoreServiceMenuItem( value );
			
			iLast = iEndString;
		}
	}
	
	static void GetServiceInfo()
	{
		gMenuItemNames = new std::vector< string >;
		
		std::vector< FSSpec > services = V::GetContentsAsFSSpecs
		(
			V::DirSpec( V::SystemVolume(), fsRtDirID ) + "System" + "Library" + "Services"
		);
		
		std::for_each
		(
			services.begin(), 
			services.end(), 
			std::ptr_fun( DoSomethingWithServiceFile )
		);
	}
	*/
	
	static bool Install()
	{
		bool locked = LoadAndLock();
		if ( !locked )
		{
			return false;
		}
		
		MyA4 a4;
		
		//gMenuItemNames = new std::vector< string >;
		
		gServicesMenuItem = 0;
		gTE = NULL;
		
		gAppendResMenuPatch = AppendResMenuPatch( PatchedAppendResMenu );
		gMenuSelectPatch    = MenuSelectPatch   ( PatchedMenuSelect    );
		gMenuKeyPatch       = MenuKeyPatch      ( PatchedMenuKey       );
		gTEActivatePatch    = TEActivatePatch   ( PatchedTEActivate    );
		gSystemMenuPatch    = SystemMenuPatch   ( PatchedSystemMenu    );
		
		return true;
	}
	
}

void main()
{
	bool installed = ClassicService::Install();
}

