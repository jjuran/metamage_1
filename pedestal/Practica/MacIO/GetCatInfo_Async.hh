/*	===================
 *	GetCatInfo_Async.hh
 *	===================
 */

#ifndef MACIO_GETCATINFOASYNC_HH
#define MACIO_GETCATINFOASYNC_HH

// Nitrogen
#include "Nitrogen/Files.hh"

// MacIO
#include "MacIO/ThrowOSStatus.hh"


namespace MacIO
{
	
	using Nitrogen::FSVolumeRefNum;
	using Nitrogen::FSDirID;
	
	
	template < class Policy, class Callback >
	inline typename Policy::Result
	//
	GetCatInfo( CInfoPBRec&        pb,
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
	
}

#endif

