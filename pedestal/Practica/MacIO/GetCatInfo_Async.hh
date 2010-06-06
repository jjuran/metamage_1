/*	===================
 *	GetCatInfo_Async.hh
 *	===================
 */

#ifndef MACIO_GETCATINFOASYNC_HH
#define MACIO_GETCATINFOASYNC_HH

// Nitrogen
#include "Mac/Files/Types/FSDirID.hh"
#include "Mac/Files/Types/FSVolumeRefNum.hh"

// MacIO
#include "MacIO/ThrowOSStatus.hh"


namespace MacIO
{
	
	using Mac::FSVolumeRefNum;
	using Mac::FSDirID;
	
	
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
		nucleus::initialize< CInfoPBRec >( pb, vRefNum, dirID, name, index );
		
		return PBAsync< GetCatInfo_Traits, Policy >( pb,
		                                             callback,
		                                             completion );
	}
	
}

#endif

