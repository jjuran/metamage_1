/*	==================
 *	GetCatInfo_Sync.hh
 *	==================
 */

#ifndef MACIO_GETCATINFOSYNC_HH
#define MACIO_GETCATINFOSYNC_HH

// Nitrogen
#include "Nitrogen/Files.h"

// MacIO
#include "MacIO/ThrowOSStatus.hh"


namespace MacIO
{
	
	template < class Policy >
	typename Policy::Result
	//
	GetCatInfo( CInfoPBRec&               pb,
	            Nitrogen::FSVolumeRefNum  vRefNum,
	            Nitrogen::FSDirID         dirID,
	            unsigned char*            name,
	            SInt16                    index = 0 );
	
	template <>
	void GetCatInfo< Throw_All >( CInfoPBRec&               pb,
	                              Nitrogen::FSVolumeRefNum  vRefNum,
	                              Nitrogen::FSDirID         dirID,
	                              unsigned char*            name,
	                              SInt16                    index );
	
	template <>
	bool GetCatInfo< Return_FNF >( CInfoPBRec&               pb,
	                               Nitrogen::FSVolumeRefNum  vRefNum,
	                               Nitrogen::FSDirID         dirID,
	                               unsigned char*            name,
	                               SInt16                    index );
	
	
	template < class Policy >
	typename Policy::Result
	inline 
	//
	GetCatInfo( CInfoPBRec&    pb,
	            const FSSpec&  item )
	{
		Nitrogen::Str255 name = item.name;
		
		return GetCatInfo< Policy >( pb,
		                             Nitrogen::FSVolumeRefNum( item.vRefNum ),
		                             Nitrogen::FSDirID       ( item.parID   ),
		                             name );
	}
	
	template < class Policy >
	typename Policy::Result
	inline 
	//
	GetCatInfo( CInfoPBRec&               pb,
	            Nitrogen::FSVolumeRefNum  vRefNum,
	            Nitrogen::FSDirID         dirID )
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
	GetCatInfo( CInfoPBRec&                 pb,
	            const Nitrogen::FSDirSpec&  dir )
	{
		return GetCatInfo< Policy >( pb,
		                             dir.vRefNum,
		                             dir.dirID );
	}
	
}

#endif

