// CFNumber.cp

#ifndef NITROGEN_CFNUMBER_H
#include "Nitrogen/CFNumber.h"
#endif

namespace Nitrogen
  {
   Owned< CFNumberRef > CFNumberCreate( CFAllocatorRef allocator,
                                        CFNumberType   theType,
                                        const void *   valuePtr )
     {
      CFNumberRef result = ::CFNumberCreate( allocator, theType, valuePtr );
      if ( result == CFNumberRef() )
         throw CFNumberCreate_Failed();
      return Owned< CFNumberRef >::Seize( result );
     }
  }
