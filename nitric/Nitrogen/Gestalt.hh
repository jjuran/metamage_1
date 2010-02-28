// Nitrogen/Gestalt.hh
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2007 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_GESTALT_HH
#define NITROGEN_GESTALT_HH

#ifndef __GESTALT__
#include <Gestalt.h>
#endif
#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif
#include "Nucleus/ErrorsRegistered.h"


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( GestaltManager );
	
	enum Gestalt_Selector
	{
		gestaltAliasMgrAttr     = ::gestaltAliasMgrAttr,
		gestaltAppleEventsAttr  = ::gestaltAppleEventsAttr,
		gestaltFindFolderAttr   = ::gestaltFindFolderAttr,
		gestaltHelpMgrAttr      = ::gestaltHelpMgrAttr,
		gestaltMenuMgrAttr      = ::gestaltMenuMgrAttr,
		gestaltStandardFileAttr = ::gestaltStandardFileAttr,
		
		gestaltComponentMgr     = ::gestaltComponentMgr,
		gestaltQuickTimeVersion = ::gestaltQuickTimeVersion,
		
		kGestaltSelector_Max = nucleus::enumeration_traits< ::OSType >::max
	};
	
	typedef Gestalt_Selector GestaltSelector;
	
	
   SInt32 Gestalt( Gestalt_Selector selector );
   SInt32 Gestalt( Gestalt_Selector selector, SInt32 defaultValue );
   
   template < Gestalt_Selector selector > struct GestaltDefault;
   
   template < Gestalt_Selector selector >
   inline SInt32 Gestalt()
     {
      return Gestalt( selector, GestaltDefault<selector>::defaultValue );
     }
   
	template < Gestalt_Selector selector, SInt32 bitMask >
	inline bool Gestalt_Mask()
	{
		return Gestalt< selector >() & bitMask;
	}
	
	template < Gestalt_Selector selector, SInt32 bitOffset >
	inline bool Gestalt_Bit()
	{
		return Gestalt_Mask< selector, 1 << bitOffset >();
	}
	
	template < Gestalt_Selector selector >
	inline bool Gestalt_NonZero()
	{
		return Gestalt_Mask< selector, 0xffffffff >();
	}
	
   struct GestaltAttrDefaults
     {
      static const SInt32 defaultValue = 0;
     };
 
    struct GestaltVersionDefaults
     {
      static const SInt32 defaultValue = 0;
     };
	
	template <> struct GestaltDefault< gestaltAliasMgrAttr     >: GestaltAttrDefaults {};  // 263
	template <> struct GestaltDefault< gestaltAppleEventsAttr  >: GestaltAttrDefaults {};  // 614
	template <> struct GestaltDefault< gestaltFindFolderAttr   >: GestaltAttrDefaults {};  // 668
	template <> struct GestaltDefault< gestaltHelpMgrAttr      >: GestaltAttrDefaults {};  // 760
	template <> struct GestaltDefault< gestaltMenuMgrAttr      >: GestaltAttrDefaults {};  // 1117
	template <> struct GestaltDefault< gestaltStandardFileAttr >: GestaltAttrDefaults {};  // 1720
	
	template <> struct GestaltDefault< gestaltComponentMgr     >: GestaltVersionDefaults {};  // 427
	template <> struct GestaltDefault< gestaltQuickTimeVersion >: GestaltVersionDefaults {};  // 1521
	
  }

#endif
