/*	=========
 *	FSSpec.cc
 *	=========
 */

#include "Genie/FS/FSSpec.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


namespace Genie
{
	
	using mac::types::VRefNum_DirID;
	
	
	VRefNum_DirID Dir_From_CInfo( const CInfoPBRec& cInfo )
	{
		const bool is_dir = cInfo.hFileInfo.ioFlAttrib & kioFlAttribDirMask;
		
		if ( !is_dir )
		{
			// I wanted a dir but you gave me a file.  You creep.
			Mac::ThrowOSStatus( errFSNotAFolder );
		}
		
		const short vRefNum = cInfo.dirInfo.ioVRefNum;
		const long  dirID   = cInfo.dirInfo.ioDrDirID;
		
		const VRefNum_DirID result = { vRefNum, dirID };
		
		return result;
	}
	
}
