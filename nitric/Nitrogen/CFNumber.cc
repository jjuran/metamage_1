// Nitrogen/CFNumber.cc
// --------------------

// Part of the Nitrogen project.
//
// Written 2002 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/CFNumber.hh"


namespace Nitrogen
  {
   nucleus::owned< CFNumberRef > CFNumberCreate( CFAllocatorRef allocator,
                                        CFNumberType   theType,
                                        const void *   valuePtr )
     {
      CFNumberRef result = ::CFNumberCreate( allocator, theType, valuePtr );
      if ( result == CFNumberRef() )
         throw CFNumberCreate_Failed();
      return nucleus::owned< CFNumberRef >::seize( result );
     }
  }
