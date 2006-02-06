// CFArray.h

#ifndef NITROGEN_CFARRAY_H
#define NITROGEN_CFARRAY_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __CFARRAY__
#include FRAMEWORK_HEADER(CoreFoundation,CFArray.h)
#endif
#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif

namespace Nitrogen
  {
   using ::CFArrayRef;
   using ::CFArrayGetTypeID;
   template <> struct CFType_Traits< CFArrayRef >: Basic_CFType_Traits< CFArrayRef, ::CFArrayGetTypeID > {};

   using ::CFMutableArrayRef;
   template <> struct CFType_Traits< CFMutableArrayRef >: Basic_CFType_Traits< CFMutableArrayRef, ::CFArrayGetTypeID > {};

   inline void CFShow( const CFArrayRef a )        { ::CFShow( a ); }
   inline void CFShow( const CFMutableArrayRef a ) { ::CFShow( a ); }
  }

namespace Nucleus {
   template <> struct OwnedDefaults< Nitrogen::CFArrayRef >       : OwnedDefaults< Nitrogen::CFTypeRef >  {};
   template <> struct OwnedDefaults< Nitrogen::CFMutableArrayRef >: OwnedDefaults< Nitrogen::CFTypeRef >  {};
  }

#endif
