/*	===================
 *	GetCatInfo_Async.hh
 *	===================
 */

#ifndef MACIO_GETCATINFOASYNC_HH
#define MACIO_GETCATINFOASYNC_HH

// Nitrogen
#ifndef MAC_FILES_TYPES_FSDIRID_HH
#include "Mac/Files/Types/FSDirID.hh"
#endif
#ifndef MAC_FILES_TYPES_FSVOLUMEREFNUM_HH
#include "Mac/Files/Types/FSVolumeRefNum.hh"
#endif

// MacIO
#include "MacIO/ThrowOSStatus.hh"


namespace MacIO
{
	
	template < class Policy, class Callback >
	inline typename Policy::Result
	//
	GetCatInfo( CInfoPBRec&          pb,
	            Mac::FSVolumeRefNum  vRefNum,
	            Mac::FSDirID         dirID,
	            unsigned char*       name,
	            SInt16               index,
	            Callback             callback,
	            ::IOCompletionUPP    completion = NULL )
	{
		nucleus::initialize< CInfoPBRec >( pb, vRefNum, dirID, name, index );
		
		return PBAsync< GetCatInfo_Traits, Policy >( pb,
		                                             callback,
		                                             completion );
	}
	
}

#endif

