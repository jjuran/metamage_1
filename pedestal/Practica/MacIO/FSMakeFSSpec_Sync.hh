/*	====================
 *	FSMakeFSSpec_Sync.hh
 *	====================
 */

#ifndef MACIO_FSMAKEFSSPECSYNC_HH
#define MACIO_FSMAKEFSSPECSYNC_HH

// Nitrogen
#include "Mac/Files/Types/FSDirID.hh"
#include "Mac/Files/Types/FSDirSpec.hh"
#include "Mac/Files/Types/FSVolumeRefNum.hh"

// MacIO
#include "MacIO/ThrowOSStatus.hh"


namespace MacIO
{
	
	template < class Policy >
	FSSpec FSMakeFSSpec( Mac::FSVolumeRefNum   vRefNum,
	                     Mac::FSDirID          dirID,
	                     const unsigned char  *name );
	
	template < class Policy >
	inline FSSpec FSMakeFSSpec( const Mac::FSDirSpec&  dir,
	                            const unsigned char*   name )
	{
		return FSMakeFSSpec< Policy >( dir.vRefNum, dir.dirID, name );
	}
	
}

#endif

