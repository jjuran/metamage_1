// Nitrogen/CFData.cc
// ------------------

// Part of the Nitrogen project.
//
// Written 2003 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/CFData.hh"


namespace Nitrogen
  {
   Nucleus::Owned<CFDataRef> CFDataCreate( CFAllocatorRef allocator,
                                  const UInt8 *  bytes,
                                  CFIndex        length )
     {
      CFDataRef result = ::CFDataCreate( allocator, bytes, length );
      if ( result == 0 )
         throw CFDataCreate_Failed();
      return Nucleus::Owned<CFDataRef>::Seize( result );
     }
   
   Nucleus::Owned<CFDataRef> CFDataCreate( CFAllocatorRef            allocator,
                                  const std::vector<UInt8>& bytes )
     {
      return Nitrogen::CFDataCreate( allocator,
                                     bytes.empty() ? 0 : &*bytes.begin(),
                                     Nucleus::Convert<CFIndex>( bytes.size() ) );
     }
   
   Nucleus::Owned<CFDataRef> CFDataCreateWithBytesNoCopy( CFAllocatorRef allocator,
                                                 const UInt8 *  bytes,
                                                 CFIndex        length,
                                                 CFAllocatorRef bytesDeallocator )
     {
      CFDataRef result = ::CFDataCreateWithBytesNoCopy( allocator, bytes, length, bytesDeallocator );
      if ( result == 0 )
         throw CFDataCreateWithBytesNoCopy_Failed();
      return Nucleus::Owned<CFDataRef>::Seize( result );
     }
   
   Nucleus::Owned<CFDataRef> CFDataCreateWithBytesNoCopy( CFAllocatorRef      allocator,
                                                 std::vector<UInt8>& bytes )
     {
      return Nitrogen::CFDataCreateWithBytesNoCopy( allocator,
                                                    bytes.empty() ? 0 : &*bytes.begin(),
                                                    Nucleus::Convert<CFIndex>( bytes.size() ) );
     }

   Nucleus::Owned<CFDataRef> CFDataCreateCopy( CFAllocatorRef   allocator,
                                      CFDataRef        theData )
     {
      CFDataRef result = ::CFDataCreateCopy( allocator, theData );
      if ( result == 0 )
         throw CFDataCreateCopy_Failed();
      return Nucleus::Owned<CFDataRef>::Seize( result );
     }
   
   Nucleus::Owned<CFMutableDataRef> CFDataCreateMutable( CFAllocatorRef   allocator,
                                                CFIndex          capacity )
     {
      CFMutableDataRef result = ::CFDataCreateMutable( allocator, capacity );
      if ( result == 0 )
         throw CFDataCreateMutable_Failed();
      return Nucleus::Owned<CFMutableDataRef>::Seize( result );
     }
   
   Nucleus::Owned<CFMutableDataRef> CFDataCreateMutableCopy( CFAllocatorRef   allocator,
                                                    CFIndex          capacity,
                                                    CFDataRef        theData )
     {
      CFMutableDataRef result = ::CFDataCreateMutableCopy( allocator, capacity, theData );
      if ( result == 0 )
         throw CFDataCreateMutableCopy_Failed();
      return Nucleus::Owned<CFMutableDataRef>::Seize( result );
     }
   
   std::vector<UInt8> CFDataGetBytes( CFDataRef theData,
                                      CFRange   range )
     {
      typedef std::vector<UInt8> Vector;
      Vector result( Nucleus::Convert<Vector::size_type>( range.length ) );
      if ( !result.empty() )
         CFDataGetBytes( theData, range, &*result.begin() );
      return result;
     }
   
   std::vector<UInt8> CFDataGetBytes( CFDataRef theData )
     {
      return CFDataGetBytes( theData, CFRangeMake( 0, CFDataGetLength( theData ) ) );
     }
   
   void CFDataAppendBytes( CFMutableDataRef          theData,
                           const std::vector<UInt8>& bytes )
     {
      if ( !bytes.empty() )
         CFDataAppendBytes( theData,
                            &*bytes.begin(),
                            Nucleus::Convert<CFIndex>( bytes.size() ) );
     }


   void CFDataReplaceBytes( CFMutableDataRef          theData,
                            CFRange                   range,
                            const std::vector<UInt8>& newBytes )
     {
      CFDataReplaceBytes( theData,
                          range,
                          newBytes.empty() ? 0 : &*newBytes.begin(),
                          Nucleus::Convert<CFIndex>( newBytes.size() ) );
     }
  }
