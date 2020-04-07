/*	==========
 *	AsyncIO.cc
 *	==========
 */

#include "Genie/Utilities/AsyncIO.hh"

// mac-config
#include "mac_config/upp-macros.hh"

// mac-sys-utils
#include "mac_sys/async_wakeup.hh"

// plus
#include "plus/pointer_to_function.hh"

// MacFeatures
#include "MacFeatures/BlueBoxed.hh"

// MacIO
#include "MacIO/FSMakeFSSpec_Async.hh"
#include "MacIO/FSpOpen_Async.hh"
#include "MacIO/FSRead_Async.hh"
#include "MacIO/FSRead_Sync.hh"
#include "MacIO/FSWrite_Async.hh"
#include "MacIO/FSWrite_Sync.hh"
#include "MacIO/GetCatInfo_Async.hh"
#include "MacIO/GetCatInfo_Sync.hh"

// Genie
#include "Genie/Process/AsyncYield.hh"


#define YIELD  plus::ptr_fun( AsyncYield )


namespace Genie
{
	
	namespace n = nucleus;
	
	
#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
	
	static pascal void WakeUp()
	
#else
	
	static pascal void WakeUp( ParamBlockRec* )
	
#endif
	{
		mac::sys::request_async_wakeup();
	}
	
	DEFINE_UPP( IOCompletion, WakeUp )
	
	SInt32 FSRead( MacIO::EOF_Policy  policy,
	               Mac::FSFileRefNum  file,
	               Mac::FSIOPosMode   positionMode,
	               SInt32             positionOffset,
	               SInt32             requestCount,
	               void              *buffer,
	               bool               async )
	{
		return async ? MacIO::FSRead( policy,
		                              file,
		                              positionMode,
		                              positionOffset,
		                              requestCount,
		                              buffer,
		                              YIELD,
		                              UPP_ARG( WakeUp ) )
		             : MacIO::FSRead( policy,
		                              file,
		                              positionMode,
		                              positionOffset,
		                              requestCount,
		                              buffer );
	}
	
	
	SInt32 FSWrite( Mac::FSFileRefNum  file,
	                Mac::FSIOPosMode   positionMode,
	                SInt32             positionOffset,
	                SInt32             requestCount,
	                const void *       buffer,
	                bool               async )
	{
		return async ? MacIO::FSWrite( file,
		                               positionMode,
		                               positionOffset,
		                               requestCount,
		                               buffer,
		                               YIELD,
		                               UPP_ARG( WakeUp ) )
		             : MacIO::FSWrite( file,
		                               positionMode,
		                               positionOffset,
		                               requestCount,
		                               buffer );
	}
	
	
	template < class Policy >
	typename Policy::Result
	//
	FSpGetCatInfo( CInfoPBRec&     pb,
	               bool            async,
	               SInt16          vRefNum,
	               SInt32          dirID,
	               unsigned char*  name,
	               SInt16          index )
	{
		//Str255 dummyName = "\p";  // clang hates this
		
		Str255 dummyName;
		       dummyName[ 0 ] = 0;
		
		if ( index == 0  &&  (name == NULL  ||  name[ 0 ] == '\0') )
		{
			if ( name == NULL )
			{
				name = dummyName;
			}
			
			index = -1;
		}
		
		// Calling the asynchronous variant of FSpGetCatInfo() reliably elicits
		// System Error 27 (dsFSErr: file system map has been trashed) in Classic
		// (eventually), but only when calling StatFile() from CheckProgramFile().
		
		// UPDATE
		// ...until now.  So now we shoot with a shorter barrel and disable
		// async I/O for FSpGetCatInfo() generally.
		
		// On infrequent (but not rare) occasion, ioResult is set and control is
		// returned to the application (allowing it to destroy the parameter block)
		// before the completion routine has run.  Sadly, polling a flag set by
		// the completion routine only delays the crash instead of avoiding it.
		// Apparently this is a bug in the .BlueBoxShared driver.
		
		if ( TARGET_CPU_68K  ||  !async  ||  MacFeatures::Is_BlueBoxed() )
		{
			return MacIO::GetCatInfo< Policy >( pb, vRefNum, dirID, name, index );
		}
		
		return MacIO::GetCatInfo< Policy >( pb,
		                                    vRefNum,
		                                    dirID,
		                                    name,
		                                    index,
		                                    YIELD,
		                                    UPP_ARG( WakeUp ) );
	}
	
	template
	void FSpGetCatInfo< FNF_Throws >( CInfoPBRec&     pb,
	                                  bool            async,
	                                  SInt16          vRefNum,
	                                  SInt32          dirID,
	                                  unsigned char*  name,
	                                  SInt16          index );
	
	template
	bool FSpGetCatInfo< FNF_Returns >( CInfoPBRec&     pb,
	                                   bool            async,
	                                   SInt16          vRefNum,
	                                   SInt32          dirID,
	                                   unsigned char*  name,
	                                   SInt16          index );
	
	
	template < class Policy >
	FSSpec FSMakeFSSpec( Mac::FSVolumeRefNum   vRefNum,
	                     Mac::FSDirID          dirID,
	                     const unsigned char*  name )
	{
		return MacIO::FSMakeFSSpec< Policy >( vRefNum,
		                                      dirID,
		                                      name,
		                                      YIELD,
		                                      UPP_ARG( WakeUp ) );
	}
	
	template
	FSSpec FSMakeFSSpec< FNF_Throws >( Mac::FSVolumeRefNum   vRefNum,
	                                   Mac::FSDirID          dirID,
	                                   const unsigned char*  name );
	
	template
	FSSpec FSMakeFSSpec< FNF_Returns >( Mac::FSVolumeRefNum   vRefNum,
	                                    Mac::FSDirID          dirID,
	                                    const unsigned char*  name );
	
}
