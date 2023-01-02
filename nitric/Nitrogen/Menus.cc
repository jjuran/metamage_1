// Nitrogen/Menus.cc
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2006 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/Menus.hh"

// Nitrogen
#include "Nitrogen/OSStatus.hh"


namespace Nitrogen
{
	
	using Mac::ThrowOSStatus;
	
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( MenuManager )
	
	
	static void RegisterMenuManagerErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class MenuManagerErrorsRegistration
	{
		public:
			MenuManagerErrorsRegistration()  { RegisterMenuManagerErrors(); }
	};
	
	static MenuManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
#if ! __LP64__
	
   nucleus::owned<MenuRef> NewMenu( MenuID           menuID,
                           ConstStr255Param menuTitle )
     {
      MenuRef result = ::NewMenu( menuID, menuTitle );
      if ( result == 0 )
         throw NewMenu_Failed();
      return nucleus::owned<MenuRef>::seize( result );
     }
   
   void SetMenuExcludesMarkColumn( MenuRef menu, bool excludesMark )
     {
      ThrowOSStatus( ::SetMenuExcludesMarkColumn( menu, excludesMark ) );
     }

   
   nucleus::owned<MenuRef> CreateNewMenu( MenuID inMenuID, MenuAttributes inMenuAttributes )
      {
        MenuRef result;
       ThrowOSStatus( ::CreateNewMenu( inMenuID, inMenuAttributes, &result ) );
       return nucleus::owned<MenuRef>::seize ( result );
      }
   
   MenuItemIndex AppendMenuItemTextWithCFString( MenuRef            inMenu,
                                                 CFStringRef        inString,
                                                 MenuItemAttributes inAttributes,
                                                 MenuCommand        inCommandID )
     {
      MenuItemIndex result;
      ThrowOSStatus ( ::AppendMenuItemTextWithCFString( inMenu, inString, inAttributes, inCommandID, &result ) );
      return result;
     }

   void InsertMenuItemTextWithCFString( MenuRef            inMenu,
                                        CFStringRef        inString,
                                        MenuItemIndex      inAfterItem,
                                        MenuItemAttributes inAttributes,
                                        MenuCommand        inCommandID )
     {
      ThrowOSStatus ( ::InsertMenuItemTextWithCFString( inMenu, inString, inAfterItem, inAttributes, inCommandID ) );
     }

	nucleus::owned< MenuID > MacInsertMenu( MenuRef menu, MenuID beforeID )
	{
		::MacInsertMenu( menu, beforeID );
		return nucleus::owned< MenuID >::seize( Nitrogen::GetMenuID( menu ) );
	}
	
	Str255 GetMenuItemText( MenuRef menu, SInt16 item )
	{
		Str255 itemText;
		::GetMenuItemText( menu, item, itemText );
		return itemText;
	}
	
   void ChangeMenuItemAttributes( MenuRef            menu,
                                  MenuItemIndex      item,
                                  MenuItemAttributes setTheseAttributes,
                                  MenuItemAttributes clearTheseAttributes )
     {
      ThrowOSStatus( ::ChangeMenuItemAttributes( menu, item, setTheseAttributes, clearTheseAttributes ) );
     }
   
#endif  // #if ! __LP64__
	
   void RegisterMenuManagerErrors()
     {
      RegisterOSStatus< paramErr                >();
      RegisterOSStatus< memFullErr              >();
      RegisterOSStatus< resNotFound             >();
      RegisterOSStatus< hmHelpManagerNotInited  >();
      RegisterOSStatus< menuPropertyInvalidErr  >();
      RegisterOSStatus< menuPropertyNotFoundErr >();
      RegisterOSStatus< menuNotFoundErr         >();
      RegisterOSStatus< menuUsesSystemDefErr    >();
      RegisterOSStatus< menuItemNotFoundErr     >();
      RegisterOSStatus< menuInvalidErr          >();
     }
  }
