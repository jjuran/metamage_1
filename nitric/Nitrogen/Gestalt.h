// Nitrogen/Gestalt.h
// ------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2007 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_GESTALT_H
#define NITROGEN_GESTALT_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __GESTALT__
#include FRAMEWORK_HEADER(CarbonCore,Gestalt.h)
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
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
		
		kGestaltSelector_Max = Nucleus::Enumeration_Traits< ::OSType >::max
	};
	
	typedef Gestalt_Selector GestaltSelector;
	
	
   long Gestalt( Gestalt_Selector selector );
   long Gestalt( Gestalt_Selector selector, long defaultValue );
   
   template < Gestalt_Selector selector > struct GestaltDefault;
   
   template < Gestalt_Selector selector >
   inline long Gestalt()
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
      static const long defaultValue = 0;
     };
 
    struct GestaltVersionDefaults
     {
      static const long defaultValue = 0;
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
