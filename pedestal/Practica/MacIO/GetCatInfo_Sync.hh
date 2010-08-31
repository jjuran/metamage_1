/*	==================
 *	GetCatInfo_Sync.hh
 *	==================
 */

#ifndef MACIO_GETCATINFOSYNC_HH
#define MACIO_GETCATINFOSYNC_HH

// Nitrogen
#include "Mac/Files/Types/FSDirID.hh"
#include "Mac/Files/Types/FSDirSpec.hh"
#include "Mac/Files/Types/FSVolumeRefNum.hh"
#include "Nitrogen/Str.hh"

// MacIO
#include "MacIO/ThrowOSStatus.hh"


namespace MacIO
{
	
	template < class Policy >
	typename Policy::Result
	//
	GetCatInfo( CInfoPBRec&          pb,
	            Mac::FSVolumeRefNum  vRefNum,
	            Mac::FSDirID         dirID,
	            unsigned char*       name,
	            SInt16               index = 0 );
	
	
	template < class Policy >
	typename Policy::Result
	inline 
	//
	GetCatInfo( CInfoPBRec&    pb,
	            const FSSpec&  item )
	{
		Nitrogen::Str255 name = item.name;
		
		return GetCatInfo< Policy >( pb,
		                             Mac::FSVolumeRefNum( item.vRefNum ),
		                             Mac::FSDirID       ( item.parID   ),
		                             name );
	}
	
	template < class Policy >
	typename Policy::Result
	inline 
	//
	GetCatInfo( CInfoPBRec&          pb,
	            Mac::FSVolumeRefNum  vRefNum,
	            Mac::FSDirID         dirID )
	{
		return GetCatInfo< Policy >( pb,
		                             vRefNum,
		                             dirID,
		                             NULL,
		                             -1 );
	}
	
	template < class Policy >
	typename Policy::Result
	inline 
	//
	GetCatInfo( CInfoPBRec&            pb,
	            const Mac::FSDirSpec&  dir )
	{
		return GetCatInfo< Policy >( pb,
		                             dir.vRefNum,
		                             dir.dirID );
	}
	
}

#endif

