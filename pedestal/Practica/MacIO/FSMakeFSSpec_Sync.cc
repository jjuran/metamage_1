/*	====================
 *	FSMakeFSSpec_Sync.cc
 *	====================
 */

#include "MacIO/FSMakeFSSpec_Sync.hh"

// MacIO
#include "MacIO/FSMakeFSSpec.hh"
#include "MacIO/Routines.hh"
#include "MacIO/Sync.hh"


namespace MacIO
{
	
	template < class Policy >
	FSSpec FSMakeFSSpec( Mac::FSVolumeRefNum   vRefNum,
	                     Mac::FSDirID          dirID,
	                     const unsigned char  *name )
	{
		HParamBlockRec  pb;
		FSSpec          result;
		
		Init_PB_For_MakeFSSpec( pb, vRefNum, dirID, name, result );
		
		(void) PBSync< MakeFSSpec_Traits, Policy >( pb );
		
		return result;
	}
	
	
	template
	FSSpec FSMakeFSSpec< Throw_All >( Mac::FSVolumeRefNum   vRefNum,
	                                  Mac::FSDirID          dirID,
	                                  const unsigned char  *name );
	
	template
	FSSpec FSMakeFSSpec< Return_FNF >( Mac::FSVolumeRefNum   vRefNum,
	                                   Mac::FSDirID          dirID,
	                                   const unsigned char  *name );
	
}

