// Nitrogen/CFData.hh
// ------------------

// Part of the Nitrogen project.
//
// Written 2003-2010 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
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
   template <> struct disposer_class< CFDataRef        >: disposer_class< Nitrogen::CFTypeRef >  {};
   template <> struct disposer_class< CFMutableDataRef >: disposer_class< Nitrogen::CFTypeRef >  {};
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
   
	template < class Data >
	inline nucleus::owned< CFDataRef > CFDataCreate( CFAllocatorRef  allocator,
	                                                 const Data&     bytes )
	{
		return Nitrogen::CFDataCreate( allocator,
		                               &bytes[ 0 ],
		                               bytes.size() );
	}
	
	template < class Data >
	inline nucleus::owned< CFDataRef > CFDataCreate( const Data& bytes )
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

	template < class Data >
	inline nucleus::owned< CFDataRef > CFDataCreateWithBytesNoCopy( CFAllocatorRef  allocator,
	                                                                Data&           bytes )
	{
		return Nitrogen::CFDataCreateWithBytesNoCopy( allocator,
		                                              &bytes[ 0 ],
		                                              bytes.size() );
	}
	
	template < class Data >
	inline nucleus::owned< CFDataRef > CFDataCreateWithBytesNoCopy( Data& bytes )
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
   
	template < class Data >  // originally std::vector< UInt8 >
	Data CFDataGetBytes( CFDataRef  theData,
	                     CFRange    range )
	{
		Data result( range.length );
		
		if ( !result.empty() )
		{
			CFDataGetBytes( theData, range, &result[ 0 ] );
		}
		
		return result;
	}
	
	template < class Data >
	Data CFDataGetBytes( CFDataRef theData )
	{
		return CFDataGetBytes< Data >( theData, CFRangeMake( 0, CFDataGetLength( theData ) ) );
	}
   
   using ::CFDataSetLength;
   using ::CFDataIncreaseLength;

   using ::CFDataAppendBytes;
   
   template < CFIndex length >
   void CFDataAppendBytes( CFMutableDataRef theData,
                           const UInt8      (&bytes)[length] )
     {
      ::CFDataAppendBytes( theData, bytes, length );
     }
	
	template < class Data >
	inline void CFDataAppendBytes( CFMutableDataRef  theData,
	                               const Data& bytes )
	{
		if ( !bytes.empty() )
		{
			CFDataAppendBytes( theData,
			                   &bytes[ 0 ],
			                   bytes.size() );
		}
	}


   using ::CFDataReplaceBytes;
   
   template < CFIndex newLength >
   void CFDataReplaceBytes( CFMutableDataRef theData,
                            CFRange          range,
                            const UInt8      (&newBytes)[newLength] )
     {
      ::CFDataReplaceBytes( theData, range, newBytes, newLength );
     }
	
	template < class Data >
	inline void CFDataReplaceBytes( CFMutableDataRef  theData,
	                                CFRange           range,
	                                const Data&       newBytes )
	{
		CFDataReplaceBytes( theData,
		                    range,
		                    &newBytes[ 0 ],
		                    newBytes.size() );
	}

   using ::CFDataDeleteBytes;
  }

#endif
