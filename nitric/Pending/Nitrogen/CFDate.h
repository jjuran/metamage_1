// CFDate.h

#ifndef NITROGEN_CFDATE_H
#define NITROGEN_CFDATE_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __CFDATE__
#include FRAMEWORK_HEADER(CoreFoundation,CFDate.h)
#endif
#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif

namespace Nitrogen
  {
   using ::CFDateRef;
   using ::CFDateGetTypeID;
   template <> struct OwnedDefaults< CFDateRef >: OwnedDefaults< CFTypeRef >  {};
   template <> struct CFType_Traits< CFDateRef >: Basic_CFType_Traits< CFDateRef, ::CFDateGetTypeID > {};

   inline void CFShow( const CFDateRef d )        { ::CFShow( d ); }
  }

#endif
