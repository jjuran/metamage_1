// CFDate.h

#ifndef NITROGEN_CFDATE_H
#define NITROGEN_CFDATE_H

#ifndef __CFDATE__
#include <CFDate.h>
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
  }

#endif
