// Nitrogen/MacMemory.cp
// ---------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2006 by Lisa Lippincott, Joshua Juran, and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/MacMemory.h"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( MemoryManager )
	
	
	static void RegisterMemoryManagerErrors();
	
	
#pragma force_active on
	
	class MemoryManagerErrorsRegistration
	{
		public:
			MemoryManagerErrorsRegistration()  { RegisterMemoryManagerErrors(); }
	};
	
	static MemoryManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
	
	
   void MemError()
     {
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
