/*	==================
 *	GetCatInfo_Sync.hh
 *	==================
 */

#ifndef MACIO_GETCATINFOSYNC_HH
#define MACIO_GETCATINFOSYNC_HH

// Nitrogen
#ifndef MAC_FILES_TYPES_FSDIRSPEC_HH
#include "Mac/Files/Types/FSDirSpec.hh"
#endif

#ifndef NITROGEN_STR_HH
#include "Nitrogen/Str.hh"
#endif

// MacIO
#include "MacIO/ThrowOSStatus.hh"


namespace MacIO
{
	
	template < class Policy >
	typename Policy::Result
	//
	GetCatInfo( CInfoPBRec&     pb,
	            SInt16          vRefNum,
	            SInt32          dirID,
	            unsigned char*  name,
	            SInt16          index = 0 );
	
	
	template < class Policy >
	typename Policy::Result
	inline 
	//
	GetCatInfo( CInfoPBRec&    pb,
	            const FSSpec&  item )
	{
		Nitrogen::Str255 name = item.name;
		
		return GetCatInfo< Policy >( pb,
		                             item.vRefNum,
		                             item.parID,
		                             name );
	}
	
	template < class Policy >
	typename Policy::Result
	inline 
	//
	GetCatInfo( CInfoPBRec&  pb,
	            SInt16       vRefNum,
	            SInt32       dirID )
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

