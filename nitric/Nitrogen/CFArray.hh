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
  
namespace nucleus {
   template <> struct disposer_traits< CFArrayRef >       : disposer_traits< Nitrogen::CFTypeRef >  {};
   template <> struct disposer_traits< CFMutableArrayRef >: disposer_traits< Nitrogen::CFTypeRef >  {};
  }

namespace Nitrogen
  {

struct CFArrayCreate_Failed {};
   inline nucleus::owned<CFArrayRef> CFArrayCreate ( CFAllocatorRef allocator, const void **values, CFIndex numValues, const CFArrayCallBacks *callBacks )
     {
      CFArrayRef result;
      ::CFArrayCreate ( allocator, values, numValues, callBacks );
      if ( NULL == result )
        throw CFArrayCreate_Failed ();
      return nucleus::owned<CFArrayRef>::seize ( result );
      }

   struct CFArrayCreateCopy_Failed {};
   inline nucleus::owned<CFArrayRef> CFArrayCreateCopy ( CFAllocatorRef allocator, CFArrayRef theArray )
     {
      CFArrayRef result;
      ::CFArrayCreateCopy ( allocator, theArray );
      if ( NULL == result )
        throw CFArrayCreateCopy_Failed ();
      return nucleus::owned<CFArrayRef>::seize ( result );
      }

   struct CFArrayCreateMutable_Failed {};
   inline nucleus::owned<CFMutableArrayRef> CFArrayCreateMutable ( CFAllocatorRef allocator, CFIndex capacity, const CFArrayCallBacks *callBacks )
     {
      CFMutableArrayRef result;
      ::CFArrayCreateMutable ( allocator, capacity, callBacks );
      if ( NULL == result )
        throw CFArrayCreateMutable_Failed ();
      return nucleus::owned<CFMutableArrayRef>::seize ( result );
      }


   struct CFArrayCreateMutableCopy_Failed {};
   inline nucleus::owned<CFMutableArrayRef> CFArrayCreateMutableCopy ( CFAllocatorRef allocator, CFIndex capacity, CFArrayRef theArray )
     {
      CFMutableArrayRef result;
      ::CFArrayCreateMutableCopy ( allocator, capacity, theArray );
      if ( NULL == result )
        throw CFArrayCreateMutableCopy_Failed ();
      return nucleus::owned<CFMutableArrayRef>::seize ( result );
      }

  using ::CFArrayGetValueAtIndex;
  }



#endif
