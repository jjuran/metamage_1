// Menus.cp

#ifndef NITROGEN_MENUS_H
#include "Nitrogen/Menus.h"
#endif

namespace Nitrogen
  {
   Owned<MenuRef> NewMenu( MenuID           menuID,
                           ConstStr255Param menuTitle )
     {
      MenuRef result = ::NewMenu( menuID, menuTitle );
      if ( result == 0 )
         throw NewMenu_Failed();
      return Owned<MenuRef>::Seize( result );
     }
   
   MenuFont GetMenuFont( MenuRef menu )
     {
      OnlyOnce< RegisterMenuManagerErrors >();
      MenuFont result;
      SInt16 fontID;
      ThrowOSStatus( ::GetMenuFont( menu, &fontID, &result.fontSize ) );
      result.fontID = fontID;
      return result;
     }

   void SetMenuFont( MenuRef menu, FontID inFontID, UInt16 inFontSize )
     {
      OnlyOnce< RegisterMenuManagerErrors >();
      ThrowOSStatus( ::SetMenuFont( menu, inFontID, inFontSize ) );
     }
   
   void SetMenuExcludesMarkColumn( MenuRef menu, bool excludesMark )
     {
      OnlyOnce< RegisterMenuManagerErrors >();
      ThrowOSStatus( ::SetMenuExcludesMarkColumn( menu, excludesMark ) );
     }

   
   Owned<MenuRef> CreateNewMenu( MenuID inMenuID, MenuAttributes inMenuAttributes )
      {
       OnlyOnce< RegisterMenuManagerErrors >();
       MenuRef result;
       ThrowOSStatus( ::CreateNewMenu( inMenuID, inMenuAttributes, &result ) );
       return Owned<MenuRef>::Seize ( result );
      }
   
   MenuItemIndex AppendMenuItemTextWithCFString( MenuRef            inMenu,
                                                 CFStringRef        inString,
                                                 MenuItemAttributes inAttributes,
                                                 MenuCommand        inCommandID )
     {
      OnlyOnce< RegisterMenuManagerErrors >();
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
      OnlyOnce< RegisterMenuManagerErrors >();
      ThrowOSStatus ( ::InsertMenuItemTextWithCFString( inMenu, inString, inAfterItem, inAttributes, inCommandID ) );
     }

   void ChangeMenuItemAttributes( MenuRef            menu,
                                  MenuItemIndex      item,
                                  MenuItemAttributes setTheseAttributes,
                                  MenuItemAttributes clearTheseAttributes )
     {
      OnlyOnce< RegisterMenuManagerErrors >();
      ThrowOSStatus( ::ChangeMenuItemAttributes( menu, item, setTheseAttributes, clearTheseAttributes ) );
     }
   
   void RegisterMenuManagerErrors()
     {
      RegisterOSStatus< paramErr               >();
      RegisterOSStatus< memFullErr             >();
      RegisterOSStatus< resNotFound            >();
      RegisterOSStatus< hmHelpManagerNotInited >();
     }
  }
