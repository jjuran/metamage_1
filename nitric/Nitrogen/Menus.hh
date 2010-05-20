// Nitrogen/Menus.hh
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2009 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_MENUS_HH
#define NITROGEN_MENUS_HH

#ifndef __MENUS__
#include <Menus.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/flag_ops.hh"

#ifndef NITROGEN_FONTS_HH
#include "Nitrogen/Fonts.hh"
#endif
#ifndef NITROGEN_RESOURCES_HH
#include "Nitrogen/Resources.hh"
#endif

#ifdef GetMenuRef
#undef GetMenuRef

inline MenuRef GetMenuRef( MenuID menuID )  { return GetMenuHandle( menuID ); }

#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( MenuManager );
	
	enum MenuAttributes
	{
		kMenuAttributes_Max = nucleus::enumeration_traits< ::MenuAttributes >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( MenuAttributes )
	
	enum MenuItemAttributes
	{
		kMenuItemAttrIgnoreMeta = ::kMenuItemAttrIgnoreMeta,
		
		kMenuItemAttributes_Max = nucleus::enumeration_traits< ::MenuItemAttributes >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( MenuItemAttributes )
	
	enum MenuTrackingMode
	{
		kMenuTrackingMode_Max = nucleus::enumeration_traits< ::MenuTrackingMode >::max
	};
	 
	enum MenuEventOptions
	{
		kMenuEventOptions_Max = nucleus::enumeration_traits< ::MenuEventOptions >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( MenuEventOptions )
	
	enum MenuID
	{
		kMenuID_Max = nucleus::enumeration_traits< ::MenuID >::max
	};
	
}

namespace nucleus
  {
   template <> struct disposer< Nitrogen::MenuID >
     {
      typedef Nitrogen::MenuID  argument_type;
      typedef void              result_type;
      
      void operator()( Nitrogen::MenuID id ) const
        {
         ::DeleteMenu( id );
        }
     };
  }

namespace Nitrogen
  {

   using ::MenuItemIndex;
   
	enum MenuCommand
	{
		kMenuCommand_Max = nucleus::enumeration_traits< ::MenuCommand >::max
	};
   
   using ::MenuRef;
  }

namespace nucleus
  {   
   template <> struct disposer< MenuRef >
     {
      typedef MenuRef  argument_type;
      typedef void     result_type;
      
      void operator()( MenuRef m ) const
        {
         ::DisposeMenu( m );
        }
     };
  }

namespace Nitrogen
  {
   /* ... */
   
	enum MenuItemDataFlags
	{
		kMenuItemDataFlags_Max = nucleus::enumeration_traits< ::MenuItemDataFlags >::max
	};
   
   NUCLEUS_DEFINE_FLAG_OPS( MenuItemDataFlags )
   
   using ::MenuItemDataRec;
   using ::MenuItemDataPtr;
   
	enum MenuItemID
	{
		kMenuItemID_Max = nucleus::enumeration_traits< ::MenuItemID >::max
	};
	
   /* ... */
   
   #pragma mark -
   #pragma mark ¥ Menu Manipulation ¥
   
   class NewMenu_Failed {};
   
   // 1549
   nucleus::owned<MenuRef> NewMenu( MenuID           menuID,
                                    ConstStr255Param menuTitle );
   
	inline MenuRef CheckResource( MenuRef menu )
	{
		CheckResource( reinterpret_cast< ::Handle >( menu ) );
		return menu;
	}
	
   // 1563
   inline MenuRef MacGetMenu( ResID resourceID )             { return CheckResource( ::MacGetMenu( resourceID ) ); }
   
   // 1578
   inline void DisposeMenu( nucleus::owned<MenuRef> /* theMenu */ )          {}
   
   using ::CalcMenuSize;
	
	using ::CountMenuItems;
	
   struct MenuFont
     {
      FontID fontID;
      UInt16 fontSize;
     };
   
   MenuFont GetMenuFont( MenuRef menu );

   void SetMenuFont( MenuRef menu, FontID inFontID, UInt16 inFontSize );
   inline void SetMenuFont( MenuRef menu, MenuFont font )         { SetMenuFont( menu, font.fontID, font.fontSize ); }
   
   inline bool GetMenuExcludesMarkColumn( MenuRef menu )          { return ::GetMenuExcludesMarkColumn( menu ); }
   void SetMenuExcludesMarkColumn( MenuRef menu, bool excludesMark ); 
   
   /* ... RegisterMenuDefinition */
   
   nucleus::owned<MenuRef> CreateNewMenu( MenuID inMenuID, MenuAttributes inMenuAttributes = MenuAttributes() );

   /* ... CreateCustomMenu */
   
   inline bool IsValidMenu( MenuRef inMenu )                      { return ::IsValidMenu( inMenu ); }
   
   /* ... */
   
   #pragma mark -
   #pragma mark ¥ Menu Item Insertion ¥
   
   // 2224
   using ::MacAppendMenu;
   
   // 2241
   inline void InsertResMenu( MenuRef menu, ResType type, SInt16 afterItem )  { ::InsertResMenu( menu, type, afterItem ); }
   
   // 2256
   inline void AppendResMenu( MenuRef menu, ResType type )  { ::AppendResMenu( menu, type ); }
   
   // 2270
   inline void MacInsertMenuItem( MenuRef menu, ConstStr255Param itemString, SInt16 afterItem )  { ::MacInsertMenuItem( menu, itemString, afterItem ); }
   
   // 2288
   inline void DeleteMenuItem( MenuRef menu, SInt16 item )  { ::DeleteMenuItem( menu, item ); }
   
   MenuItemIndex AppendMenuItemTextWithCFString( MenuRef            inMenu,
                                                 CFStringRef        inString,
                                                 MenuItemAttributes inAttributes,
                                                 MenuCommand        inCommandID );

   void InsertMenuItemTextWithCFString( MenuRef            inMenu,
                                        CFStringRef        inString,
                                        MenuItemIndex      inAfterItem,
                                        MenuItemAttributes inAttributes,
                                        MenuCommand        inCommandID );

   /* ... */
   
   inline void SetItemMark( MenuRef theMenu, short item, char markChar )  { ::SetItemMark( theMenu, item, markChar ); }
   
   /* ... */
   
   using ::MacEnableMenuItem;
   using ::DisableMenuItem;

   #pragma mark -
   #pragma mark ¥ Menu Bar ¥
   
   using ::GetMBarHeight;
   using ::MacDrawMenuBar;
   using ::InvalMenuBar;
   
   inline void HiliteMenu( MenuID menuID = MenuID() )  { ::HiliteMenu( menuID ); }
   
   inline MenuRef GetMenuHandle( MenuID menuID )  { return ::GetMenuHandle( menuID ); }
   inline MenuRef GetMenuRef   ( MenuID menuID )  { return ::GetMenuRef   ( menuID ); }
   
   nucleus::owned< MenuID > MacInsertMenu( MenuRef menu, MenuID beforeID = MenuID() );
   inline void MacDeleteMenu( nucleus::owned< MenuID > /* menuID */ )  {}
   
   /* ... */

   #pragma mark -
   #pragma mark ¥ Menu Item Accessors ¥
   
   using ::SetMenuItemText;
   
   Str255 GetMenuItemText( MenuRef menu, SInt16 item );
   
   #pragma mark -
   #pragma mark ¥ Attributes ¥
   
   // 4315
   void ChangeMenuItemAttributes( MenuRef            menu,
                                  MenuItemIndex      item,
                                  MenuItemAttributes setTheseAttributes,
                                  MenuItemAttributes clearTheseAttributes );

   /* ... */
	
	// 5826
	inline MenuID GetMenuID( MenuRef menu )
	{
		return MenuID( ::GetMenuID( menu ) );
	}
	
  }

#endif
