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

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MENUS__
#include <Menus.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_ERRORSREGISTERED_HH
#include "nucleus/errors_registered.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_MENUS_TYPES_MENUCOMMAND_HH
#include "Mac/Menus/Types/MenuCommand.hh"
#endif
#ifndef MAC_MENUS_TYPES_MENUID_HH
#include "Mac/Menus/Types/MenuID.hh"
#endif
#ifndef MAC_RESOURCES_TYPES_RESID_HH
#include "Mac/Resources/Types/ResID.hh"
#endif
#ifndef MAC_RESOURCES_TYPES_RESTYPE_HH
#include "Mac/Resources/Types/ResType.hh"
#endif
#ifndef MAC_RESOURCES_UTILITIES_CHECKEDRESOURCE_HH
#include "Mac/Resources/Utilities/Checked_Resource.hh"
#endif

#ifndef NITROGEN_STR_HH
#include "Nitrogen/Str.hh"
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
	
	using Mac::MenuID;
	using Mac::MenuCommand;
	
}

#if ! __LP64__
	
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

#endif  // #if ! __LP64__
	
namespace Nitrogen
  {
   /* ... */
   
	enum MenuItemDataFlags
	{
		kMenuItemDataFlags_Max = nucleus::enumeration_traits< ::MenuItemDataFlags >::max
	};
   
   NUCLEUS_DEFINE_FLAG_OPS( MenuItemDataFlags )
   
	enum MenuItemID
	{
		kMenuItemID_Max = nucleus::enumeration_traits< ::MenuItemID >::max
	};
	
   /* ... */
   
#if ! __LP64__
	
   #pragma mark -
   #pragma mark ** Menu Manipulation **
   
   class NewMenu_Failed {};
   
   // 1549
   nucleus::owned<MenuRef> NewMenu( MenuID           menuID,
                                    ConstStr255Param menuTitle );
   
	inline MenuRef CheckResource( MenuRef menu )
	{
		Mac::Checked_Resource( reinterpret_cast< ::Handle >( menu ) );
		return menu;
	}
	
   // 1563
   inline MenuRef MacGetMenu( Mac::ResID resID )  { return CheckResource( ::MacGetMenu( resID ) ); }
   
   // 1578
   inline void DisposeMenu( nucleus::owned<MenuRef> /* theMenu */ )          {}
   
   using ::CalcMenuSize;
	
	using ::CountMenuItems;
	
   inline bool GetMenuExcludesMarkColumn( MenuRef menu )          { return ::GetMenuExcludesMarkColumn( menu ); }
   void SetMenuExcludesMarkColumn( MenuRef menu, bool excludesMark ); 
   
   /* ... RegisterMenuDefinition */
   
   nucleus::owned<MenuRef> CreateNewMenu( MenuID inMenuID, MenuAttributes inMenuAttributes = MenuAttributes() );

   /* ... CreateCustomMenu */
   
   inline bool IsValidMenu( MenuRef inMenu )                      { return ::IsValidMenu( inMenu ); }
   
   /* ... */
   
   #pragma mark -
   #pragma mark ** Menu Item Insertion **
   
   // 2224
   using ::MacAppendMenu;
   
   // 2241
   inline void InsertResMenu( MenuRef menu, Mac::ResType type, SInt16 afterItem )  { ::InsertResMenu( menu, type, afterItem ); }
   
   // 2256
   inline void AppendResMenu( MenuRef menu, Mac::ResType type )  { ::AppendResMenu( menu, type ); }
   
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
   #pragma mark ** Menu Bar **
   
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
   #pragma mark ** Menu Item Accessors **
   
   using ::SetMenuItemText;
   
   Str255 GetMenuItemText( MenuRef menu, SInt16 item );
   
   #pragma mark -
   #pragma mark ** Attributes **
   
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
	
#endif  // #if ! __LP64__
	
  }

#endif
