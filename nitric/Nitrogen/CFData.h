// CFData.h

#ifndef NITROGEN_CFDATA_H
#define NITROGEN_CFDATA_H

#ifndef __CFDATA__
#include <CFData.h>
#endif
#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif

#include <vector>

namespace Nitrogen
  {
   using ::CFDataRef;
   using ::CFMutableDataRef;
   
   template <> struct CFType_Traits< CFDataRef        >: Basic_CFType_Traits< CFDataRef,        ::CFDataGetTypeID > {};
   template <> struct CFType_Traits< CFMutableDataRef >: Basic_CFType_Traits< CFMutableDataRef, ::CFDataGetTypeID > {};

   template <> struct OwnedDefaults< CFDataRef        >: OwnedDefaults< CFTypeRef >  {};
   template <> struct OwnedDefaults< CFMutableDataRef >: OwnedDefaults< CFTypeRef >  {};
   
   using ::CFDataGetTypeID;

   class CFDataCreate_Failed {};
   
   Owned<CFDataRef> CFDataCreate( CFAllocatorRef allocator,
                                  const UInt8 *  bytes,
                                  CFIndex        length );
   
   inline Owned<CFDataRef> CFDataCreate( const UInt8 *bytes,
                                         CFIndex      length )
     {
      return Nitrogen::CFDataCreate( kCFAllocatorDefault, bytes, length );
     }
   
   template < CFIndex length >
   Owned<CFDataRef> CFDataCreate( CFAllocatorRef allocator,
                                  const UInt8    (&bytes)[length] )
     {
      return Nitrogen::CFDataCreate( allocator, bytes, length );
     }
   
   template < CFIndex length >
   Owned<CFDataRef> CFDataCreate( const UInt8 (&bytes)[length] )
     {
      return Nitrogen::CFDataCreate( kCFAllocatorDefault, bytes, length );
     }
   
   Owned<CFDataRef> CFDataCreate( CFAllocatorRef            allocator,
                                  const std::vector<UInt8>& bytes );
   
   inline Owned<CFDataRef> CFDataCreate( const std::vector<UInt8>& bytes )
     {
      return Nitrogen::CFDataCreate( kCFAllocatorDefault, bytes );
     }



   class CFDataCreateWithBytesNoCopy_Failed {};
   
   Owned<CFDataRef> CFDataCreateWithBytesNoCopy( CFAllocatorRef allocator,
                                                 const UInt8 *  bytes,
                                                 CFIndex        length,
                                                 CFAllocatorRef bytesDeallocator = 0 );
   
   inline Owned<CFDataRef> CFDataCreateWithBytesNoCopy( const UInt8  * bytes,
                                                        CFIndex        length,
                                                        CFAllocatorRef bytesDeallocator = 0 )
     {
      return Nitrogen::CFDataCreateWithBytesNoCopy( kCFAllocatorDefault, bytes, length, bytesDeallocator );
     }
   
   template < CFIndex length >
   Owned<CFDataRef> CFDataCreateWithBytesNoCopy( CFAllocatorRef allocator,
                                                 const UInt8    (&bytes)[length],
                                                 CFAllocatorRef bytesDeallocator = 0 )
     {
      return Nitrogen::CFDataCreateWithBytesNoCopy( allocator, bytes, length, bytesDeallocator );
     }
   
   template < CFIndex length >
   Owned<CFDataRef> CFDataCreateWithBytesNoCopy( const UInt8    (&bytes)[length],
                                                 CFAllocatorRef bytesDeallocator = 0 )
     {
      return Nitrogen::CFDataCreateWithBytesNoCopy( kCFAllocatorDefault, bytes, length, bytesDeallocator );
     }

   Owned<CFDataRef> CFDataCreateWithBytesNoCopy( CFAllocatorRef      allocator,
                                                 std::vector<UInt8>& bytes );

   inline Owned<CFDataRef> CFDataCreateWithBytesNoCopy( std::vector<UInt8>& bytes )
     {
      return Nitrogen::CFDataCreateWithBytesNoCopy( kCFAllocatorDefault, bytes );
     }




   class CFDataCreateCopy_Failed {};

   Owned<CFDataRef> CFDataCreateCopy( CFAllocatorRef   allocator,
                                      CFDataRef        theData );
   
   inline Owned<CFDataRef> CFDataCreateCopy( CFDataRef theData )
     {
      return Nitrogen::CFDataCreateCopy( kCFAllocatorDefault, theData );
     }

   class CFDataCreateMutable_Failed {};

   Owned<CFMutableDataRef> CFDataCreateMutable( CFAllocatorRef   allocator,
                                                CFIndex          capacity );
   
   inline Owned<CFMutableDataRef> CFDataCreateMutable( CFIndex capacity )
     {
      return Nitrogen::CFDataCreateMutable( kCFAllocatorDefault, capacity );
     }

   
   class CFDataCreateMutableCopy_Failed {};

   Owned<CFMutableDataRef> CFDataCreateMutableCopy( CFAllocatorRef   allocator,
                                                    CFIndex          capacity,
                                                    CFDataRef        theData );
   
   inline Owned<CFMutableDataRef> CFDataCreateMutableCopy( CFDataRef theData,
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
