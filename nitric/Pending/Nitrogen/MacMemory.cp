// MacMemory.cp

#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
#endif

namespace Nitrogen
  {
   void MemError()
     {
      OnlyOnce<RegisterMemoryManagerErrors>();
      ThrowOSStatus( ::MemError() );
     }
   
   Owned<Handle> NewHandle( std::size_t size )
     {
      ::Handle result = ::NewHandle( static_cast< ::Size >( size ) );
      if ( result == NULL )
         MemError();
      return Owned<Handle>::Seize( result );
     }
   
	Owned< Ptr > NewPtrSysClear( std::size_t size )
	{
		Ptr ptr = ::NewPtrSysClear( size );
		if ( ptr == NULL )
		{
			MemError();
		}
		return Owned< Ptr >::Seize( ptr );
	}
	
	Owned< Handle > NewHandleSys( std::size_t size )
	{
		Handle h = ::NewHandleSys( size );
		if ( h == NULL )
		{
			MemError();
		}
		return Owned< Handle >::Seize( h );
	}
	
	Owned< Handle > NewHandleSys( const unsigned char* str )
	{
		// Length of handle: 1 for length byte plus string length
		Owned< Handle > h = NewHandleSys( 1 + str[0] );
		
		::BlockMoveData( str, *h.Get(), 1 + str[0] );
		
		return h;
	}
	
   void RegisterMemoryManagerErrors()
     {
      RegisterOSStatus< menuPrgErr              >();
      RegisterOSStatus< negZcbFreeErr           >();
      RegisterOSStatus< paramErr                >();
      RegisterOSStatus< memROZErr               >();
      RegisterOSStatus< memFullErr              >();
      RegisterOSStatus< nilHandleErr            >();
      RegisterOSStatus< memAdrErr               >();
      RegisterOSStatus< memWZErr                >();
      RegisterOSStatus< memPurErr               >();
      RegisterOSStatus< memBCErr                >();
      RegisterOSStatus< memLockedErr            >();
      RegisterOSStatus< notEnoughMemoryErr      >();
      RegisterOSStatus< notHeldErr              >();
      RegisterOSStatus< cannotMakeContiguousErr >();
      RegisterOSStatus< notLockedErr            >();
      RegisterOSStatus< interruptsMaskedErr     >();
      RegisterOSStatus< cannotDeferErr          >();
     }
  }
