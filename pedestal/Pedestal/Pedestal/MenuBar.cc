/*	==========
 *	MenuBar.cc
 *	==========
 */

#include "Pedestal/MenuBar.hh"

// iota
#include "iota/quad.hh"

// Nitrogen
#include "Mac/Resources/Types/ResType.hh"

#include "Nitrogen/Menus.hh"

// ClassicToolbox
#if CALL_NOT_IN_CARBON
#include "ClassicToolbox/Devices.hh"
#endif

// Pedestal
#include "Pedestal/MenuItemCommands.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	static N::MenuID gAppleMenuID = N::MenuID();
	
	
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
	
	void AddMenu( MenuRef menu )
	{
		const Mac::MenuID menuID = N::GetMenuID( menu );
		
		const UInt16 count = N::CountMenuItems( menu );
		
		for ( int i = count;  i > 0;  i-- )
		{
			const CommandCode code = ExtractItemCmdCode( menu, i );
			
			SetMenuItemCommandCode( menuID, i, code );
		}
	}
	
	
	static const Mac::ResType kDeskAccessoryResourceType = Mac::ResType( 'DRVR' );
	
	void PopulateAppleMenu( MenuRef menu )
	{
		gAppleMenuID = N::GetMenuID( menu );
		
		N::AppendResMenu( menu, kDeskAccessoryResourceType );
	}
	
	CommandCode HandleMenuItem( N::MenuID menuID, SInt16 item )
	{
		if ( CommandCode code = GetMenuItemCommandCode( menuID, item ) )
		{
			return code;
		}
		
	#if CALL_NOT_IN_CARBON
		
		if ( menuID == gAppleMenuID )
		{
			N::OpenDeskAcc( N::GetMenuItemText( N::GetMenuRef( menuID ), item ) );
		}
		
	#endif
		
		return kCmdNone;
	}
	
}

