// Nitrogen/CFPropertyList.cp
// --------------------------

// Part of the Nitrogen project.
//
// Written 2002-2003 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_CFPROPERTYLIST_H
#include "Nitrogen/CFPropertyList.h"
#endif

namespace Nitrogen
  {
   template <> CFPropertyListRef CFCast<CFPropertyListRef>( CFTypeRef p )
     {
      const CFTypeID actualType = CFGetTypeID( p );
      if (    actualType != CFType_Traits< CFStringRef     >::ID()
           && actualType != CFType_Traits< CFNumberRef     >::ID()
           && actualType != CFType_Traits< CFBooleanRef    >::ID()
           && actualType != CFType_Traits< CFDateRef       >::ID()
           && actualType != CFType_Traits< CFDataRef       >::ID()
           && actualType != CFType_Traits< CFArrayRef      >::ID()
           && actualType != CFType_Traits< CFDictionaryRef >::ID() )
         throw CFCast_Failed();
      
      return CFPropertyListRef( p.Get() );
     }
  }
