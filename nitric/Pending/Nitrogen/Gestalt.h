// Gestalt.h

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

namespace Nitrogen
  {
   long Gestalt( OSType selector );
   long Gestalt( OSType selector, long defaultValue );
   
   template < ::OSType selector > struct GestaltDefault;
   
   template < ::OSType selector >
   long Gestalt()
     {
      return Gestalt( OSType( selector ), GestaltDefault<selector>::defaultValue );
     }
   
	template < ::OSType selector, SInt32 bitMask >
	bool Gestalt_Mask()
	{
		return Gestalt< selector >() & bitMask;
	}
	
	template < ::OSType selector, SInt32 bitOffset >
	bool Gestalt_Bit()
	{
		return Gestalt_Mask< selector, 1 << bitOffset >();
	}
	
	template < ::OSType selector >
	bool Gestalt_NonZero()
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
	
   void RegisterGestaltManagerErrors();
  }

#endif
