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
   
	Owned< Handle > NewHandle( std::size_t size )
	{
		return Owned< Handle >::Seize( CheckMemory( ::NewHandle( size ) ) );
	}
	
	Owned< Handle > NewHandleSys( std::size_t size )
	{
		return Owned< Handle >::Seize( CheckMemory( ::NewHandleSys( size ) ) );
	}
	
	Owned< Handle > NewHandleClear( std::size_t size )
	{
		return Owned< Handle >::Seize( CheckMemory( ::NewHandleClear( size ) ) );
	}
	
	Owned< Handle > NewHandleSysClear( std::size_t size )
	{
		return Owned< Handle >::Seize( CheckMemory( ::NewHandleSysClear( size ) ) );
	}
	
	Owned< Ptr > NewPtr( std::size_t size )
	{
		return Owned< Ptr >::Seize( CheckMemory( ::NewPtr( size ) ) );
	}
	
	Owned< Ptr > NewPtrSys( std::size_t size )
	{
		return Owned< Ptr >::Seize( CheckMemory( ::NewPtrSys( size ) ) );
	}
	
	Owned< Ptr > NewPtrClear( std::size_t size )
	{
		return Owned< Ptr >::Seize( CheckMemory( ::NewPtrClear( size ) ) );
	}
	
	Owned< Ptr > NewPtrSysClear( std::size_t size )
	{
		return Owned< Ptr >::Seize( CheckMemory( ::NewPtrSysClear( size ) ) );
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
