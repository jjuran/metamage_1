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
#include "Genie/FS/FSTree_sys_mac_errata.hh"


#define CALLBACK  more::ptr_fun( AsyncYield )


#pragma mark ¥ Nitrogen ¥
#pragma mark -

namespace Nitrogen
{
	
	typedef FNF_Throws Throw_Errors;
	
	struct Return_EOF
	{
		typedef bool Result;
		
		static bool HandleOSStatus( OSStatus err )
		{
			if ( err == eofErr )
			{
				return false;
			}
			
			ThrowOSStatus( err );
			
			return true;
		}
	};
	
	struct ParamBlock_Traits
	{
		typedef ParamBlockRec PB;
		
		static ::IOCompletionUPP& IOCompletion( PB& pb )  { return pb.ioParam.ioCompletion; }
		
		static volatile ::OSErr& IOResult( PB& pb )  { return pb.ioParam.ioResult; }
	};
	
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
	
	
	struct HOpenDF_Traits : HParamBlock_Traits
	{
		static OSStatus Async( PB& pb )
		{
			return ::PBHOpenDFAsync( &pb );
		}
		
		static OSStatus Sync( PB& pb )
		{
			return ::PBHOpenDFSync( &pb );
		}
	};
	
	struct HOpenRF_Traits : HParamBlock_Traits
	{
		static OSStatus Async( PB& pb )
		{
			return ::PBHOpenRFAsync( &pb );
		}
		
		static OSStatus Sync( PB& pb )
		{
			return ::PBHOpenRFSync( &pb );
		}
	};
	
	struct Read_Traits : ParamBlock_Traits
	{
		static OSStatus Async( PB& pb )
		{
			return ::PBReadAsync( &pb );
		}
		
		static OSStatus Sync( PB& pb )
		{
			return ::PBReadSync( &pb );
		}
	};
	
	struct Write_Traits : ParamBlock_Traits
	{
		static OSStatus Async( PB& pb )
		{
			return ::PBWriteAsync( &pb );
		}
		
		static OSStatus Sync( PB& pb )
		{
			return ::PBWriteSync( &pb );
		}
	};
	
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
	
	
	static void Init_PB_For_ReadWrite( ParamBlockRec&  pb,
	                                   FSFileRefNum    file,
	                                   FSIOPosMode     positionMode,
	                                   SInt32          positionOffset,
	                                   SInt32          requestCount,
	                                   const void *    buffer )
	{
		IOParam& io = pb.ioParam;
		
		io.ioRefNum    = file;
		io.ioBuffer    = (char*) buffer;
		io.ioReqCount  = requestCount;
		io.ioPosMode   = positionMode;
		io.ioPosOffset = positionOffset;
	}
	
	enum EOF_Policy
	{
		kThrowEOF_Never,
		kThrowEOF_OnZero,
		kThrowEOF_Always
	};
	
	// Synchronous
	template < EOF_Policy policy >
	inline SInt32 FSRead( FSFileRefNum  file,
	                      FSIOPosMode   positionMode,
	                      SInt32        positionOffset,
	                      SInt32        requestCount,
	                      void *        buffer )
	{
		ParamBlockRec pb;
		
		Init_PB_For_ReadWrite( pb,
		                       file,
		                       positionMode,
		                       positionOffset,
		                       requestCount,
		                       buffer );
		
		const bool ok = PBSync< Read_Traits, Return_EOF >( pb );
		
		if ( ok + !!pb.ioParam.ioActCount < policy )
		{
			throw EOFErr();
		}
		
		return pb.ioParam.ioActCount;
	}
	
	// Asynchronous
	template < EOF_Policy policy, class Callback >
	inline SInt32 FSRead( FSFileRefNum       file,
	                      FSIOPosMode        positionMode,
	                      SInt32             positionOffset,
	                      SInt32             requestCount,
	                      void *             buffer,
	                      Callback           callback,
	                      ::IOCompletionUPP  completion = NULL )
	{
		ParamBlockRec pb;
		
		Init_PB_For_ReadWrite( pb,
		                       file,
		                       positionMode,
		                       positionOffset,
		                       requestCount,
		                       buffer );
		
		const bool ok = PBAsync< Read_Traits, Return_EOF >( pb,
		                                                    callback,
		                                                    completion );
		
		if ( ok + !!pb.ioParam.ioActCount < policy )
		{
			throw EOFErr();
		}
		
		return pb.ioParam.ioActCount;
	}
	
	// Async read, default position mode
	template < EOF_Policy policy, class Callback >
	inline SInt32 FSRead( FSFileRefNum       file,
	                      SInt32             requestCount,
	                      void *             buffer,
	                      Callback           callback,
	                      ::IOCompletionUPP  completion = NULL )
	{
		return FSRead< policy >( file,
		                         fsAtMark,
		                         0,
		                         requestCount,
		                         buffer,
		                         callback,
		                         completion );
	}
	
	// Synchronous
	inline SInt32 FSWrite( FSFileRefNum       file,
	                       FSIOPosMode        positionMode,
	                       SInt32             positionOffset,
	                       SInt32             requestCount,
	                       const void *       buffer )
	{
		ParamBlockRec pb;
		
		Init_PB_For_ReadWrite( pb,
		                       file,
		                       positionMode,
		                       positionOffset,
		                       requestCount,
		                       buffer );
		
		PBSync< Write_Traits, Throw_Errors >( pb );
		
		return pb.ioParam.ioActCount;
	}
	
	// Asynchronous
	template < class Callback >
	inline SInt32 FSWrite( FSFileRefNum       file,
	                       FSIOPosMode        positionMode,
	                       SInt32             positionOffset,
	                       SInt32             requestCount,
	                       const void *       buffer,
	                       Callback           callback,
	                       ::IOCompletionUPP  completion = NULL )
	{
		ParamBlockRec pb;
		
		Init_PB_For_ReadWrite( pb,
		                       file,
		                       positionMode,
		                       positionOffset,
		                       requestCount,
		                       buffer );
		
		PBAsync< Write_Traits, Throw_Errors >( pb,
		                                       callback,
		                                       completion );
		
		return pb.ioParam.ioActCount;
	}
	
	// Async write, default position mode
	template < class Callback >
	inline SInt32 FSWrite( FSFileRefNum       file,
	                       SInt32             requestCount,
	                       const void *       buffer,
	                       Callback           callback,
	                       ::IOCompletionUPP  completion = NULL )
	{
		return FSWrite( file,
		                fsAtMark,
		                0,
		                requestCount,
		                buffer,
		                callback,
		                completion );
	}
	
	
	// Synchronous
	template < class Policy >
	inline typename Policy::Result
	//
	FSpGetCatInfo( CInfoPBRec&     pb,
	               FSVolumeRefNum  vRefNum,
	               FSDirID         dirID,
	               unsigned char*  name,
	               SInt16          index )
	{
		Nucleus::Initialize< CInfoPBRec >( pb, vRefNum, dirID, name, index );
		
		return PBSync< GetCatInfo_Traits, Policy >( pb );
	}
	
	// Asynchronous
	template < class Policy, class Callback >
	inline typename Policy::Result
	//
	FSpGetCatInfo( CInfoPBRec&        pb,
	               FSVolumeRefNum     vRefNum,
	               FSDirID            dirID,
	               unsigned char*     name,
	               SInt16             index,
	               Callback           callback,
	               ::IOCompletionUPP  completion = NULL )
	{
		Nucleus::Initialize< CInfoPBRec >( pb, vRefNum, dirID, name, index );
		
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
	
	
	template < class Traits, class Callback >
	Nucleus::Owned< FSFileRefNum >
	//
	FSpOpen( const FSSpec&      spec,
	         FSIOPermssn        permissions,
	         Callback           callback,
	         ::IOCompletionUPP  completion = NULL )
	{
		HParamBlockRec pb;
		
		HIOParam& io = pb.ioParam;
		
		io.ioNamePtr  = const_cast< StringPtr >( spec.name );
		io.ioVRefNum  = spec.vRefNum;
		io.ioPermssn  = permissions;
		
		pb.fileParam.ioDirID = spec.parID;
		
		PBAsync< Traits, Throw_Errors >( pb,
		                                 callback,
		                                 completion );
		
		return Nucleus::Owned< FSFileRefNum >::Seize( FSFileRefNum( io.ioRefNum ) );
	}
	
	template < class Callback >
	Nucleus::Owned< FSFileRefNum >
	//
	inline FSpOpenDF( const FSSpec&      spec,
	                  FSIOPermssn        permissions,
	                  Callback           callback,
	                  ::IOCompletionUPP  completion = NULL )
	{
		return FSpOpen< HOpenDF_Traits >( spec,
		                                  permissions,
		                                  callback,
		                                  completion );
	}
	
	template < class Callback >
	Nucleus::Owned< FSFileRefNum >
	//
	inline FSpOpenRF( const FSSpec&      spec,
	                  FSIOPermssn        permissions,
	                  Callback           callback,
	                  ::IOCompletionUPP  completion = NULL )
	{
		return FSpOpen< HOpenRF_Traits >( spec,
		                                  permissions,
		                                  callback,
		                                  completion );
	}
	
}

#pragma mark -
#pragma mark ¥ Genie ¥
#pragma mark -

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
		return N::FSRead< N::kThrowEOF_Always >( file,
		                  positionMode,
		                  positionOffset,
		                  requestCount,
		                  buffer,
		                  CALLBACK,
		                  gWakeUp );
	}
	
	// Async read, throws eofErr if starting at EOF
	SInt32 FSRead( N::FSFileRefNum  file,
	               N::FSIOPosMode   positionMode,
	               SInt32           positionOffset,
	               SInt32           requestCount,
	               void *           buffer,
	               ThrowEOF_OnZero  policy )
	{
		return N::FSRead< N::kThrowEOF_OnZero >( file,
		                  positionMode,
		                  positionOffset,
		                  requestCount,
		                  buffer,
		                  CALLBACK,
		                  gWakeUp );
	}
	
	// Async read, returns zero
	SInt32 FSRead( N::FSFileRefNum  file,
	               N::FSIOPosMode   positionMode,
	               SInt32           positionOffset,
	               SInt32           requestCount,
	               void *           buffer,
	               ThrowEOF_Never   policy )
	{
		return N::FSRead< N::kThrowEOF_Never >( file,
		                  positionMode,
		                  positionOffset,
		                  requestCount,
		                  buffer,
		                  CALLBACK,
		                  gWakeUp );
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
	
	
	template < class Policy >
	typename Policy::Result
	//
	FSpGetCatInfo( CInfoPBRec&        pb,
	               N::FSVolumeRefNum  vRefNum,
	               N::FSDirID         dirID,
	               unsigned char*     name,
	               SInt16             index )
	{
		Str255 dummyName = "\p";
		
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
		
		if ( TARGET_CPU_68K  ||  RunningInClassic::Test() )
		{
			return N::FSpGetCatInfo< Policy >( pb, vRefNum, dirID, name, index );
		}
		
		return N::FSpGetCatInfo< Policy >( pb,
		                                   vRefNum,
		                                   dirID,
		                                   name,
		                                   index,
		                                   CALLBACK,
		                                   gWakeUp );
	}
	
	template
	void FSpGetCatInfo< FNF_Throws >( CInfoPBRec&        pb,
	                                  N::FSVolumeRefNum  vRefNum,
	                                  N::FSDirID         dirID,
	                                  unsigned char*     name,
	                                  SInt16             index );
	
	template
	bool FSpGetCatInfo< FNF_Returns >( CInfoPBRec&        pb,
	                                   N::FSVolumeRefNum  vRefNum,
	                                   N::FSDirID         dirID,
	                                   unsigned char*     name,
	                                   SInt16             index );
	
	
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

