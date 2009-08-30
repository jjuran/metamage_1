// Nitrogen/CFNumber.cp
// --------------------

// Part of the Nitrogen project.
//
// Written 2002 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/CFNumber.h"


namespace Nitrogen
  {
   Nucleus::Owned< CFNumberRef > CFNumberCreate( CFAllocatorRef allocator,
                                        CFNumberType   theType,
                                        const void *   valuePtr )
     {
      CFNumberRef result = ::CFNumberCreate( allocator, theType, valuePtr );
      if ( result == CFNumberRef() )
         throw CFNumberCreate_Failed();
      return Nucleus::Owned< CFNumberRef >::Seize( result );
     }
  }
