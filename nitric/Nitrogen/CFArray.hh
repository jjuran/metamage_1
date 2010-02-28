// Nitrogen/CFArray.hh
// -------------------

// Part of the Nitrogen project.
//
// Written 2003-2006 by Lisa Lippincott and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_CFARRAY_HH
#define NITROGEN_CFARRAY_HH

#ifndef __CFARRAY__
#include <CFArray.h>
#endif
#ifndef NITROGEN_CFBASE_HH
#include "Nitrogen/CFBase.hh"
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
   template <> struct Disposer_Traits< Nitrogen::CFArrayRef >       : Disposer_Traits< Nitrogen::CFTypeRef >  {};
   template <> struct Disposer_Traits< Nitrogen::CFMutableArrayRef >: Disposer_Traits< Nitrogen::CFTypeRef >  {};
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
