/*	====================
 *	FSMakeFSSpec_Sync.hh
 *	====================
 */

#ifndef MACIO_FSMAKEFSSPECSYNC_HH
#define MACIO_FSMAKEFSSPECSYNC_HH

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
	
	template < class Policy >
	FSSpec FSMakeFSSpec( Mac::FSVolumeRefNum   vRefNum,
	                     Mac::FSDirID          dirID,
	                     const unsigned char  *name );
	
	template < class Policy, class Dir >
	inline FSSpec FSMakeFSSpec( const Dir&            dir,
	                            const unsigned char*  name )
	{
		return FSMakeFSSpec< Policy >( Mac::FSVolumeRefNum( dir.vRefNum ),
		                               Mac::FSDirID       ( dir.dirID   ),
		                               name );
	}
	
}

#endif
