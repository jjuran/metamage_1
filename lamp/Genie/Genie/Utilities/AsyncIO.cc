/*	==========
 *	AsyncIO.cc
 *	==========
 */

#include "Genie/Utilities/AsyncIO.hh"

// MoreFunctional
#include "PointerToFunction.hh"

// Nucleus
#include "Nucleus/NAssert.h"

// Pedestal
#include "Pedestal/Application.hh"

// Genie
#include "Genie/FileSystem/FSTree_sys_mac_errata.hh"


#define CALLBACK  more::ptr_fun( AsyncYield )


namespace Nitrogen
{
	
	struct HParamBlock_Traits
	{
		typedef HParamBlockRec PB;
		
		static ::IOCompletionUPP& IOCompletion( PB& pb )  { return pb.ioParam.ioCompletion; }
		
		static volatile ::OSErr& IOResult( PB& pb )  { return pb.ioParam.ioResult; }
	};
	
	struct CInfoPB_Traits
	{
		typedef CInfoPBRec PB;
		
		static ::IOCompletionUPP& IOCompletion( PB& pb )  { return pb.dirInfo.ioCompletion; }
		
		static volatile ::OSErr& IOResult( PB& pb )  { return pb.dirInfo.ioResult; }
	};
	
	
	template < class Call_Traits, class Policy, class Callback >
	typename Policy::Result
	//
	PBAsync( typename Call_Traits::PB&  pb,
	         Callback                   callback,
	         ::IOCompletionUPP          completion )
	{
		Call_Traits::IOCompletion( pb ) = completion;
		
		::OSErr err = Call_Traits::Async( pb );
		
		Private::FixAsyncResult( err, Call_Traits::IOResult( pb ) );
		
		Policy::HandleOSStatus( err );
		
		while ( Call_Traits::IOResult( pb ) > 0 )
		{
			callback();
		}
		
		return Policy::HandleOSStatus( Call_Traits::IOResult( pb ) );
	}
	
	template < class Call_Traits, class Policy >
	inline typename Policy::Result
	//
	PBSync( typename Call_Traits::PB& pb )
	{
		return Policy::HandleOSStatus( Call_Traits::Sync( pb ) );
	}
	
	
	struct GetCatInfo_Traits : CInfoPB_Traits
	{
		static OSStatus Async( PB& pb )
		{
			return ::PBGetCatInfoAsync( &pb );
		}
		
		static OSStatus Sync( PB& pb )
		{
			return ::PBGetCatInfoSync( &pb );
		}
	};
	
	struct MakeFSSpec_Traits : HParamBlock_Traits
	{
		static OSStatus Async( PB& pb )
		{
			return ::PBMakeFSSpecAsync( &pb );
		}
		
		static OSStatus Sync( PB& pb )
		{
			return ::PBMakeFSSpecSync( &pb );
		}
	};
	
	
	// Synchronous
	template < class Policy >
	inline typename Policy::Result
	//
	FSpGetCatInfo( CInfoPBRec&           pb,
	               FSVolumeRefNum        vRefNum,
	               FSDirID               dirID,
	               const unsigned char*  name = NULL,
	               SInt16                index = 0 )
	{
		Str255 nameCopy = name != NULL ? name : "\p";
	
		Nucleus::Initialize< CInfoPBRec >( pb, vRefNum, dirID, nameCopy, index );
		
		return PBSync< GetCatInfo_Traits, Policy >( pb );
	}
	
	// Asynchronous
	template < class Policy, class Callback >
	inline typename Policy::Result
	//
	FSpGetCatInfo( CInfoPBRec&           pb,
	               FSVolumeRefNum        vRefNum,
	               FSDirID               dirID,
	               const unsigned char*  name,
	               SInt16                index,
	               Callback              callback,
	               ::IOCompletionUPP     completion = NULL )
	{
		Str255 nameCopy = name != NULL ? name : "\p";
		
		Nucleus::Initialize< CInfoPBRec >( pb, vRefNum, dirID, nameCopy, index );
		
		return PBAsync< GetCatInfo_Traits, Policy >( pb,
		                                             callback,
		                                             completion );
	}
	
	
	static void Init_PB_For_MakeFSSpec( HParamBlockRec&       pb,
	                                    FSVolumeRefNum        vRefNum,
	                                    FSDirID               dirID,
	                                    const unsigned char*  name,
	                                    FSSpec&               result )
	{
		pb.fileParam.ioVRefNum = vRefNum;
		pb.fileParam.ioDirID   = dirID;
		pb.fileParam.ioNamePtr = const_cast< StringPtr >( name );
		pb.ioParam  .ioMisc    = (char*) &result;
	}
	
	// Synchronous
	template < class Policy >
	inline FSSpec FSMakeFSSpec( FSVolumeRefNum        vRefNum,
	                            FSDirID               dirID,
	                            const unsigned char*  name )
	{
		HParamBlockRec  pb;
		FSSpec          result;
		
		Init_PB_For_MakeFSSpec( pb, vRefNum, dirID, name, result );
		
		(void) PBSync< MakeFSSpec_Traits, Policy >( pb );
		
		return result;
	}
	
	// Asynchronous
	template < class Policy, class Callback >
	inline FSSpec FSMakeFSSpec( FSVolumeRefNum        vRefNum,
	                            FSDirID               dirID,
	                            const unsigned char*  name,
	                            Callback              callback,
	                            ::IOCompletionUPP     completion = NULL )
	{
		HParamBlockRec  pb;
		FSSpec          result;
		
		Init_PB_For_MakeFSSpec( pb, vRefNum, dirID, name, result );
		
		(void) PBAsync< MakeFSSpec_Traits, Policy >( pb,
		                                             callback,
		                                             completion );
		
		return result;
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Ped = Pedestal;
	
	
#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
	
	static pascal void WakeUp()
	
#else
	
	static pascal void WakeUp( ParamBlockRec* )
	
#endif
	{
		Ped::WakeUp();
	}
	
	static IOCompletionUPP gWakeUp = ::NewIOCompletionUPP( WakeUp );
	
	// Async read, throws eofErr on hitting EOF
	SInt32 FSRead( N::FSFileRefNum  file,
	               N::FSIOPosMode   positionMode,
	               SInt32           positionOffset,
	               SInt32           requestCount,
	               void *           buffer,
	               ThrowEOF_Always  policy )
	{
		return N::FSRead( file,
		                  positionMode,
		                  positionOffset,
		                  requestCount,
		                  buffer,
		                  CALLBACK,
		                  gWakeUp,
		                  policy );
	}
	
	// Async read, throws eofErr if starting at EOF
	SInt32 FSRead( N::FSFileRefNum  file,
	               N::FSIOPosMode   positionMode,
	               SInt32           positionOffset,
	               SInt32           requestCount,
	               void *           buffer,
	               ThrowEOF_OnZero  policy )
	{
		return N::FSRead( file,
		                  positionMode,
		                  positionOffset,
		                  requestCount,
		                  buffer,
		                  CALLBACK,
		                  gWakeUp,
		                  policy );
	}
	
	// Async read, returns zero
	SInt32 FSRead( N::FSFileRefNum  file,
	               N::FSIOPosMode   positionMode,
	               SInt32           positionOffset,
	               SInt32           requestCount,
	               void *           buffer,
	               ThrowEOF_Never   policy )
	{
		return N::FSRead( file,
		                  positionMode,
		                  positionOffset,
		                  requestCount,
		                  buffer,
		                  CALLBACK,
		                  gWakeUp,
		                  policy );
	}
	
	
	// Async write
	SInt32 FSWrite( N::FSFileRefNum  file,
	                N::FSIOPosMode   positionMode,
	                SInt32           positionOffset,
	                SInt32           requestCount,
	                const void *     buffer )
	{
		return N::FSWrite( file,
		                   positionMode,
		                   positionOffset,
		                   requestCount,
		                   buffer,
		                   CALLBACK,
		                   gWakeUp );
	}
	
	
	// Asynchronous, throws FNFErr
	void FSpGetCatInfo( const FSSpec&  item,
	                    CInfoPBRec&    pb,
	                    Async          async,
	                    FNF_Throws     policy )
	{
		// Calling the asynchronous variant of FSpGetCatInfo() reliably elicits
		// System Error 27 (dsFSErr: file system map has been trashed) in Classic
		// (eventually), but only when calling StatFile() from CheckProgramFile().
		
		// UPDATE
		// ...until now.  So now we shoot with a shorter barrel and disable
		// async I/O for FSpGetCatInfo( ..., FNF_Throws ) generally.
		
		// On infrequent (but not rare) occasion, ioResult is set and control is
		// returned to the application (allowing it to destroy the parameter block)
		// before the completion routine has run.  Sadly, polling a flag set by
		// the completion routine only delays the crash instead of avoiding it.
		// Apparently this is a bug in the .BlueBoxShared driver.
		
		if ( RunningInClassic::Test() )
		{
			N::FSpGetCatInfo( item, pb, policy );
		}
		else
		{
			N::FSpGetCatInfo( item,
			                  pb,
			                  CALLBACK,
			                  gWakeUp,
			                  policy );
		}
	}
	
	// Asynchronous, returns false on fnfErr
	bool FSpGetCatInfo( const FSSpec&  item,
	                    CInfoPBRec&    pb,
	                    Async          async,
	                    FNF_Returns    policy )
	{
		return N::FSpGetCatInfo< FNF_Returns >( pb,
		                                        N::FSVolumeRefNum( item.vRefNum ),
		                                        N::FSDirID( item.parID ),
		                                        item.name,
		                                        0,
		                                        CALLBACK,
		                                        gWakeUp );
	}
	
	
	template < class Policy >
	FSSpec FSMakeFSSpec( N::FSVolumeRefNum     vRefNum,
	                     N::FSDirID            dirID,
	                     const unsigned char*  name )
	{
		return N::FSMakeFSSpec< Policy >( vRefNum, dirID, name, CALLBACK, gWakeUp );
	}
	
	template
	FSSpec FSMakeFSSpec< FNF_Throws >( N::FSVolumeRefNum     vRefNum,
	                                   N::FSDirID            dirID,
	                                   const unsigned char*  name );
	
	template
	FSSpec FSMakeFSSpec< FNF_Returns >( N::FSVolumeRefNum     vRefNum,
	                                    N::FSDirID            dirID,
	                                    const unsigned char*  name );
	
	
	NN::Owned< N::FSFileRefNum >
	//
	FSpOpenDF( const FSSpec&   spec,
	           N::FSIOPermssn  permissions )
	{
		return N::FSpOpenDF( spec, permissions, CALLBACK, gWakeUp );
	}
	
	NN::Owned< N::FSFileRefNum >
	//
	FSpOpenRF( const FSSpec&   spec,
	           N::FSIOPermssn  permissions )
	{
		return N::FSpOpenRF( spec, permissions, CALLBACK, gWakeUp );
	}
	
}

