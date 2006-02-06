// MacMemory.cp

#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
#endif

namespace Nitrogen
  {
   void MemError()
     {
      Nucleus::OnlyOnce<RegisterMemoryManagerErrors>();
      ThrowOSStatus( ::MemError() );
     }
   
	Nucleus::Owned< Handle > NewHandle( std::size_t size )
	{
		return Nucleus::Owned< Handle >::Seize( CheckMemory( ::NewHandle( size ) ) );
	}
	
	Nucleus::Owned< Handle > NewHandleSys( std::size_t size )
	{
		return Nucleus::Owned< Handle >::Seize( CheckMemory( ::NewHandleSys( size ) ) );
	}
	
	Nucleus::Owned< Handle > NewHandleClear( std::size_t size )
	{
		return Nucleus::Owned< Handle >::Seize( CheckMemory( ::NewHandleClear( size ) ) );
	}
	
	Nucleus::Owned< Handle > NewHandleSysClear( std::size_t size )
	{
		return Nucleus::Owned< Handle >::Seize( CheckMemory( ::NewHandleSysClear( size ) ) );
	}
	
	Nucleus::Owned< Ptr > NewPtr( std::size_t size )
	{
		return Nucleus::Owned< Ptr >::Seize( CheckMemory( ::NewPtr( size ) ) );
	}
	
	Nucleus::Owned< Ptr > NewPtrSys( std::size_t size )
	{
		return Nucleus::Owned< Ptr >::Seize( CheckMemory( ::NewPtrSys( size ) ) );
	}
	
	Nucleus::Owned< Ptr > NewPtrClear( std::size_t size )
	{
		return Nucleus::Owned< Ptr >::Seize( CheckMemory( ::NewPtrClear( size ) ) );
	}
	
	Nucleus::Owned< Ptr > NewPtrSysClear( std::size_t size )
	{
		return Nucleus::Owned< Ptr >::Seize( CheckMemory( ::NewPtrSysClear( size ) ) );
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
