// Nitrogen/CFData.hh
// ------------------

// Part of the Nitrogen project.
//
// Written 2003-2004 by Lisa Lippincott and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_CFDATA_HH
#define NITROGEN_CFDATA_HH

#ifndef __CFDATA__
#include <CFData.h>
#endif
#ifndef NITROGEN_CFBASE_HH
#include "Nitrogen/CFBase.hh"
#endif

#include <vector>

namespace Nitrogen
  {
   using ::CFDataRef;
   using ::CFMutableDataRef;
   
   template <> struct CFType_Traits< CFDataRef        >: Basic_CFType_Traits< CFDataRef,        ::CFDataGetTypeID > {};
   template <> struct CFType_Traits< CFMutableDataRef >: Basic_CFType_Traits< CFMutableDataRef, ::CFDataGetTypeID > {};
   inline void CFShow( const CFDataRef d )        { ::CFShow( d ); }

   inline void CFShow( const CFMutableDataRef d ) { ::CFShow( d ); }
   }
   
namespace nucleus {
   template <> struct disposer_traits< CFDataRef        >: disposer_traits< Nitrogen::CFTypeRef >  {};
   template <> struct disposer_traits< CFMutableDataRef >: disposer_traits< Nitrogen::CFTypeRef >  {};
   }
   
namespace Nitrogen {
   using ::CFDataGetTypeID;

   class CFDataCreate_Failed {};
   
   nucleus::owned<CFDataRef> CFDataCreate( CFAllocatorRef allocator,
                                  const UInt8 *  bytes,
                                  CFIndex        length );
   
   inline nucleus::owned<CFDataRef> CFDataCreate( const UInt8 *bytes,
                                         CFIndex      length )
     {
      return Nitrogen::CFDataCreate( kCFAllocatorDefault, bytes, length );
     }
   
   template < CFIndex length >
   nucleus::owned<CFDataRef> CFDataCreate( CFAllocatorRef allocator,
                                  const UInt8    (&bytes)[length] )
     {
      return Nitrogen::CFDataCreate( allocator, bytes, length );
     }
   
   template < CFIndex length >
   nucleus::owned<CFDataRef> CFDataCreate( const UInt8 (&bytes)[length] )
     {
      return Nitrogen::CFDataCreate( kCFAllocatorDefault, bytes, length );
     }
   
   nucleus::owned<CFDataRef> CFDataCreate( CFAllocatorRef            allocator,
                                  const std::vector<UInt8>& bytes );
   
   inline nucleus::owned<CFDataRef> CFDataCreate( const std::vector<UInt8>& bytes )
     {
      return Nitrogen::CFDataCreate( kCFAllocatorDefault, bytes );
     }



   class CFDataCreateWithBytesNoCopy_Failed {};
   
   nucleus::owned<CFDataRef> CFDataCreateWithBytesNoCopy( CFAllocatorRef allocator,
                                                 const UInt8 *  bytes,
                                                 CFIndex        length,
                                                 CFAllocatorRef bytesDeallocator = 0 );
   
   inline nucleus::owned<CFDataRef> CFDataCreateWithBytesNoCopy( const UInt8  * bytes,
                                                        CFIndex        length,
                                                        CFAllocatorRef bytesDeallocator = 0 )
     {
      return Nitrogen::CFDataCreateWithBytesNoCopy( kCFAllocatorDefault, bytes, length, bytesDeallocator );
     }
   
   template < CFIndex length >
   nucleus::owned<CFDataRef> CFDataCreateWithBytesNoCopy( CFAllocatorRef allocator,
                                                 const UInt8    (&bytes)[length],
                                                 CFAllocatorRef bytesDeallocator = 0 )
     {
      return Nitrogen::CFDataCreateWithBytesNoCopy( allocator, bytes, length, bytesDeallocator );
     }
   
   template < CFIndex length >
   nucleus::owned<CFDataRef> CFDataCreateWithBytesNoCopy( const UInt8    (&bytes)[length],
                                                 CFAllocatorRef bytesDeallocator = 0 )
     {
      return Nitrogen::CFDataCreateWithBytesNoCopy( kCFAllocatorDefault, bytes, length, bytesDeallocator );
     }

   nucleus::owned<CFDataRef> CFDataCreateWithBytesNoCopy( CFAllocatorRef      allocator,
                                                 std::vector<UInt8>& bytes );

   inline nucleus::owned<CFDataRef> CFDataCreateWithBytesNoCopy( std::vector<UInt8>& bytes )
     {
      return Nitrogen::CFDataCreateWithBytesNoCopy( kCFAllocatorDefault, bytes );
     }




   class CFDataCreateCopy_Failed {};

   nucleus::owned<CFDataRef> CFDataCreateCopy( CFAllocatorRef   allocator,
                                      CFDataRef        theData );
   
   inline nucleus::owned<CFDataRef> CFDataCreateCopy( CFDataRef theData )
     {
      return Nitrogen::CFDataCreateCopy( kCFAllocatorDefault, theData );
     }

   class CFDataCreateMutable_Failed {};

   nucleus::owned<CFMutableDataRef> CFDataCreateMutable( CFAllocatorRef   allocator,
                                                CFIndex          capacity );
   
   inline nucleus::owned<CFMutableDataRef> CFDataCreateMutable( CFIndex capacity )
     {
      return Nitrogen::CFDataCreateMutable( kCFAllocatorDefault, capacity );
     }

   
   class CFDataCreateMutableCopy_Failed {};

   nucleus::owned<CFMutableDataRef> CFDataCreateMutableCopy( CFAllocatorRef   allocator,
                                                    CFIndex          capacity,
                                                    CFDataRef        theData );
   
   inline nucleus::owned<CFMutableDataRef> CFDataCreateMutableCopy( CFDataRef theData,
                                                           CFIndex   capacity )
     {
      return Nitrogen::CFDataCreateMutableCopy( kCFAllocatorDefault, capacity, theData );
     }
   
   
   
   
   using ::CFDataGetLength;
   using ::CFDataGetBytePtr;
   using ::CFDataGetMutableBytePtr;

   using ::CFDataGetBytes;
   
   std::vector<UInt8> CFDataGetBytes( CFDataRef theData,
                                      CFRange   range );
   
   std::vector<UInt8> CFDataGetBytes( CFDataRef theData );
   
   using ::CFDataSetLength;
   using ::CFDataIncreaseLength;

   using ::CFDataAppendBytes;
   
   template < CFIndex length >
   void CFDataAppendBytes( CFMutableDataRef theData,
                           const UInt8      (&bytes)[length] )
     {
      ::CFDataAppendBytes( theData, bytes, length );
     }

   void CFDataAppendBytes( CFMutableDataRef          theData,
                           const std::vector<UInt8>& bytes );


   using ::CFDataReplaceBytes;
   
   template < CFIndex newLength >
   void CFDataReplaceBytes( CFMutableDataRef theData,
                            CFRange          range,
                            const UInt8      (&newBytes)[newLength] )
     {
      ::CFDataReplaceBytes( theData, range, newBytes, newLength );
     }

   void CFDataReplaceBytes( CFMutableDataRef          theData,
                            CFRange                   range,
                            const std::vector<UInt8>& newBytes );

   using ::CFDataDeleteBytes;
  }

#endif
