/*	===============
 *	FSMakeFSSpec.hh
 *	===============
 */

#ifndef MACIO_FSMAKEFSSPEC_HH
#define MACIO_FSMAKEFSSPEC_HH

// Nitrogen
#include "Nitrogen/Files.hh"


namespace MacIO
{
	
	using Nitrogen::FSVolumeRefNum;
	using Nitrogen::FSDirID;
	
	
	inline void Init_PB_For_MakeFSSpec( HParamBlockRec&       pb,
	                                    FSVolumeRefNum        vRefNum,
	                                    FSDirID               dirID,
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

