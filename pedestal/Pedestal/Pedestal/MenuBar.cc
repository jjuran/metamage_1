/*	==========
 *	MenuBar.cc
 *	==========
 */

#include "Pedestal/MenuBar.hh"

// Standard C++
#include <map>
#include <vector>

// iota
#include "iota/quad.hh"

// Nitrogen
#include "Mac/Resources/Types/ResType.hh"

#include "Nitrogen/Menus.hh"

// ClassicToolbox
#if CALL_NOT_IN_CARBON
#include "ClassicToolbox/Devices.hh"
#endif


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	typedef std::map< N::MenuID, std::vector< MenuItemCode > > Menus;
	
	
	static N::MenuID gAppleMenuID = N::MenuID();
	
	static Menus gMenus;
	
	
	static MenuItemCode TakeCodeFromItemText( Str255 ioItemText )
	{
		int len = ioItemText[ 0 ];
		
		if ( len < 7 )  return 0;
		
		if ( ioItemText[ len     ] != ']' )  return 0;
		if ( ioItemText[ len - 5 ] != '[' )  return 0;
		
		MenuItemCode code = iota::decode_quad( &ioItemText[ len - 4 ] );
		
		ioItemText[ 0 ] -= 7;
		
		return code;
	}
	
	static MenuItemCode ExtractItemCmdCode( MenuRef menu, short item )
	{
		N::Str255 itemText = N::GetMenuItemText( menu, item );
		MenuItemCode code = TakeCodeFromItemText( itemText );
		if ( code != 0 )
		{
			N::SetMenuItemText( menu, item, itemText );
		}
		
		return code;
	}
	
	static void ExtractCmdCodes( MenuRef menu, std::vector< MenuItemCode >& outCodes )
	{
		short count = N::CountMenuItems( menu );
		outCodes.resize( 1 + count );  // slot 0 is unused
		for ( short i = count;  i > 0;  i-- )
		{
			outCodes[ i ] = ExtractItemCmdCode( menu, i );
		}
	}
	
	void AddMenu( N::MenuID menuID )
	{
		MenuRef menu = N::GetMenuRef( menuID );
		
		ExtractCmdCodes( menu, gMenus[ menuID ] );
	}
	
	
	static const Mac::ResType kDeskAccessoryResourceType = Mac::ResType( 'DRVR' );
	
	void PopulateAppleMenu( N::MenuID menuID )
	{
		gAppleMenuID = menuID;
		
		N::AppendResMenu( N::GetMenuRef( menuID ), kDeskAccessoryResourceType );
	}
	
	MenuItemCode HandleMenuItem( N::MenuID menuID, SInt16 item )
	{
		Menus::const_iterator it = gMenus.find( menuID );
		
		if ( it != gMenus.end() )
		{
			const std::vector< MenuItemCode >& commands = it->second;
			
			if ( item < commands.size() )
			{
				MenuItemCode code = commands[ item ];
				
				return code;
			}
			
		#if CALL_NOT_IN_CARBON
			
			else if ( menuID == gAppleMenuID )
			{
				N::OpenDeskAcc( N::GetMenuItemText( N::GetMenuRef( menuID ), item ) );
			}
			
		#endif
		}
		
		return 0;
	}
	
}

