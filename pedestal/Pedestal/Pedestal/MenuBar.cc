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
	
	
	typedef std::map< N::MenuID, std::vector< CommandCode > > Menus;
	
	
	static N::MenuID gAppleMenuID = N::MenuID();
	
	static Menus gMenus;
	
	
	static CommandCode TakeCodeFromItemText( Str255 ioItemText )
	{
		int len = ioItemText[ 0 ];
		
		if ( len < 7 )  return kCmdNone;
		
		if ( ioItemText[ len     ] != ']' )  return kCmdNone;
		if ( ioItemText[ len - 5 ] != '[' )  return kCmdNone;
		
		ioItemText[ 0 ] -= 7;
		
		return CommandCode( iota::decode_quad( &ioItemText[ len - 4 ] ) );
	}
	
	static CommandCode ExtractItemCmdCode( MenuRef menu, short item )
	{
		N::Str255 itemText = N::GetMenuItemText( menu, item );
		
		CommandCode code = TakeCodeFromItemText( itemText );
		
		if ( code != kCmdNone )
		{
			N::SetMenuItemText( menu, item, itemText );
		}
		
		return code;
	}
	
	static void ExtractCmdCodes( MenuRef menu, std::vector< CommandCode >& outCodes )
	{
		short count = N::CountMenuItems( menu );
		outCodes.resize( 1 + count );  // slot 0 is unused
		for ( short i = count;  i > 0;  i-- )
		{
			outCodes[ i ] = ExtractItemCmdCode( menu, i );
		}
	}
	
	void AddMenu( MenuRef menu )
	{
		const Mac::MenuID menuID = N::GetMenuID( menu );
		
		ExtractCmdCodes( menu, gMenus[ menuID ] );
	}
	
	
	static const Mac::ResType kDeskAccessoryResourceType = Mac::ResType( 'DRVR' );
	
	void PopulateAppleMenu( MenuRef menu )
	{
		gAppleMenuID = N::GetMenuID( menu );
		
		N::AppendResMenu( menu, kDeskAccessoryResourceType );
	}
	
	CommandCode HandleMenuItem( N::MenuID menuID, SInt16 item )
	{
		Menus::const_iterator it = gMenus.find( menuID );
		
		if ( it != gMenus.end() )
		{
			const std::vector< CommandCode >& commands = it->second;
			
			if ( item < commands.size() )
			{
				CommandCode code = commands[ item ];
				
				return code;
			}
			
		#if CALL_NOT_IN_CARBON
			
			else if ( menuID == gAppleMenuID )
			{
				N::OpenDeskAcc( N::GetMenuItemText( N::GetMenuRef( menuID ), item ) );
			}
			
		#endif
		}
		
		return kCmdNone;
	}
	
}

