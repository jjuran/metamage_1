// CFDictionary.h

#ifndef NITROGEN_CFDICTIONARY_H
#define NITROGEN_CFDICTIONARY_H

#ifndef __CFDICTIONARY__
#include <CFDictionary.h>
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
