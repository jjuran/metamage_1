/*	===============
 *	FSMakeFSSpec.hh
 *	===============
 */

#ifndef MACIO_FSMAKEFSSPEC_HH
#define MACIO_FSMAKEFSSPEC_HH

// Nitrogen
#include "Mac/Files/Types/FSDirID.hh"
#include "Mac/Files/Types/FSVolumeRefNum.hh"


namespace MacIO
{
	
	inline void Init_PB_For_MakeFSSpec( HParamBlockRec&       pb,
	                                    Mac::FSVolumeRefNum   vRefNum,
	                                    Mac::FSDirID          dirID,
	                                    const unsigned char  *name,
	                                    FSSpec&               result )
	{
		pb.fileParam.ioVRefNum = vRefNum;
		pb.fileParam.ioDirID   = dirID;
		pb.fileParam.ioNamePtr = const_cast< StringPtr >( name );
		pb.ioParam  .ioMisc    = (char *) &result;
	}
	
}

#endif

