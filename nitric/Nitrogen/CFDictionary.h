// CFDictionary.h

#ifndef NITROGEN_CFDICTIONARY_H
#define NITROGEN_CFDICTIONARY_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __CFDICTIONARY__
#include FRAMEWORK_HEADER(CoreFoundation,CFDictionary.h)
#endif
#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif

namespace Nitrogen
  {
   using ::CFDictionaryRef;
   using ::CFDictionaryGetTypeID;
   template <> struct OwnedDefaults< CFDictionaryRef >: OwnedDefaults< CFTypeRef >  {};
   template <> struct CFType_Traits< CFDictionaryRef >: Basic_CFType_Traits< CFDictionaryRef, ::CFDictionaryGetTypeID > {};
  }

#endif
