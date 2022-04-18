// Nitrogen/MacMemory.cc
// ---------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2006 by Lisa Lippincott, Joshua Juran, and Marshall Clow.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/MacMemory.hh"

// Mac OS
#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

// Nitrogen
#include "Nitrogen/OSStatus.hh"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( MemoryManager )
	
	
	static void RegisterMemoryManagerErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class MemoryManagerErrorsRegistration
	{
		public:
			MemoryManagerErrorsRegistration()  { RegisterMemoryManagerErrors(); }
	};
	
	static MemoryManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
   void MemError()
     {
      Mac::ThrowOSStatus( ::MemError() );
     }
   
	nucleus::owned< Handle > NewHandle( std::size_t size )
	{
		return nucleus::owned< Handle >::seize( CheckMemory( ::NewHandle( size ) ) );
	}
	
	nucleus::owned< Handle > NewHandleSys( std::size_t size )
	{
		return nucleus::owned< Handle >::seize( CheckMemory( ::NewHandleSys( size ) ) );
	}
	
	nucleus::owned< Handle > NewHandleClear( std::size_t size )
	{
		return nucleus::owned< Handle >::seize( CheckMemory( ::NewHandleClear( size ) ) );
	}
	
	nucleus::owned< Handle > NewHandleSysClear( std::size_t size )
	{
		return nucleus::owned< Handle >::seize( CheckMemory( ::NewHandleSysClear( size ) ) );
	}
	
	nucleus::owned< Ptr > NewPtr( std::size_t size )
	{
		return nucleus::owned< Ptr >::seize( CheckMemory( ::NewPtr( size ) ) );
	}
	
	nucleus::owned< Ptr > NewPtrSys( std::size_t size )
	{
		return nucleus::owned< Ptr >::seize( CheckMemory( ::NewPtrSys( size ) ) );
	}
	
	nucleus::owned< Ptr > NewPtrClear( std::size_t size )
	{
		return nucleus::owned< Ptr >::seize( CheckMemory( ::NewPtrClear( size ) ) );
	}
	
	nucleus::owned< Ptr > NewPtrSysClear( std::size_t size )
	{
		return nucleus::owned< Ptr >::seize( CheckMemory( ::NewPtrSysClear( size ) ) );
	}
	
	nucleus::owned< Handle > TempNewHandle( std::size_t size )
	{
		OSErr err = noErr;
		
		const Handle h = ::TempNewHandle( size, &err );
		
		Mac::ThrowOSStatus( err );
		
		nucleus::owned< Handle > result = nucleus::owned< Handle >::seize( h );
		
		return result;
	}
	
	void SetHandleSize( Handle h, std::size_t size )
	{
		::SetHandleSize( h, size );
		MemError();
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
