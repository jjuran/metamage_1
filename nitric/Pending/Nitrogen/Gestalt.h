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
   
   struct GestaltAttrDefaults
     {
      static const long defaultValue = 0;
     };
 
    struct GestaltVersionDefaults
     {
      static const long defaultValue = 0;
     };
  
   template <> struct GestaltDefault< gestaltMenuMgrAttr >: GestaltAttrDefaults {};
   
   void RegisterGestaltManagerErrors();
  }

#endif
