/*	====================
 *	FSMakeFSSpec_Sync.hh
 *	====================
 */

#ifndef MACIO_FSMAKEFSSPECSYNC_HH
#define MACIO_FSMAKEFSSPECSYNC_HH

// Nitrogen
#include "Nitrogen/Files.hh"

// MacIO
#include "MacIO/ThrowOSStatus.hh"


namespace MacIO
{
	
	using Nitrogen::FSVolumeRefNum;
	using Nitrogen::FSDirID;
	
	
	template < class Policy >
	FSSpec FSMakeFSSpec( FSVolumeRefNum        vRefNum,
	                     FSDirID               dirID,
	                     const unsigned char  *name );
	
	template <>
	FSSpec FSMakeFSSpec< Throw_All >( FSVolumeRefNum        vRefNum,
	                                  FSDirID               dirID,
	                                  const unsigned char  *name );
	
	template <>
	FSSpec FSMakeFSSpec< Return_FNF >( FSVolumeRefNum        vRefNum,
	                                   FSDirID               dirID,
	                                   const unsigned char  *name );
	
	template < class Policy >
	inline FSSpec FSMakeFSSpec( const Nitrogen::FSDirSpec&  dir,
	                            const unsigned char*        name )
	{
		return FSMakeFSSpec< Policy >( dir.vRefNum, dir.dirID, name );
	}
	
}

#endif

