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
   template <> struct OwnedDefaults< CFArrayRef >: OwnedDefaults< CFTypeRef >  {};
   template <> struct CFType_Traits< CFArrayRef >: Basic_CFType_Traits< CFArrayRef, ::CFArrayGetTypeID > {};
  }

#endif
