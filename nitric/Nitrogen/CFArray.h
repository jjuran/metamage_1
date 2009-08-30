// Nitrogen/CFArray.h
// ------------------

// Part of the Nitrogen project.
//
// Written 2003-2006 by Lisa Lippincott and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


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

namespace Nitrogen
  {

struct CFArrayCreate_Failed {};
   inline Nucleus::Owned<CFArrayRef> CFArrayCreate ( CFAllocatorRef allocator, const void **values, CFIndex numValues, const CFArrayCallBacks *callBacks )
     {
      CFArrayRef result;
      ::CFArrayCreate ( allocator, values, numValues, callBacks );
      if ( NULL == result )
        throw CFArrayCreate_Failed ();
      return Nucleus::Owned<CFArrayRef>::Seize ( result );
      }

   struct CFArrayCreateCopy_Failed {};
   inline Nucleus::Owned<CFArrayRef> CFArrayCreateCopy ( CFAllocatorRef allocator, CFArrayRef theArray )
     {
      CFArrayRef result;
      ::CFArrayCreateCopy ( allocator, theArray );
      if ( NULL == result )
        throw CFArrayCreateCopy_Failed ();
      return Nucleus::Owned<CFArrayRef>::Seize ( result );
      }

   struct CFArrayCreateMutable_Failed {};
   inline Nucleus::Owned<CFMutableArrayRef> CFArrayCreateMutable ( CFAllocatorRef allocator, CFIndex capacity, const CFArrayCallBacks *callBacks )
     {
      CFMutableArrayRef result;
      ::CFArrayCreateMutable ( allocator, capacity, callBacks );
      if ( NULL == result )
        throw CFArrayCreateMutable_Failed ();
      return Nucleus::Owned<CFMutableArrayRef>::Seize ( result );
      }


   struct CFArrayCreateMutableCopy_Failed {};
   inline Nucleus::Owned<CFMutableArrayRef> CFArrayCreateMutableCopy ( CFAllocatorRef allocator, CFIndex capacity, CFArrayRef theArray )
     {
      CFMutableArrayRef result;
      ::CFArrayCreateMutableCopy ( allocator, capacity, theArray );
      if ( NULL == result )
        throw CFArrayCreateMutableCopy_Failed ();
      return Nucleus::Owned<CFMutableArrayRef>::Seize ( result );
      }

  using ::CFArrayGetValueAtIndex;
  }



#endif
