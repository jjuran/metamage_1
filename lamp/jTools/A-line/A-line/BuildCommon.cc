/*	==============
 *	BuildCommon.cc
 *	==============
 */

#include "A-line/BuildCommon.hh"

// Universal Interfaces
#include <Files.h>

// Nitrogen
#include "Nitrogen/OSStatus.h"


namespace ALine
{
	
	namespace N = Nitrogen;
	
	void GetCatInfo( const FSSpec& item, CInfoPBRec& pb )
	{
		pb.hFileInfo.ioNamePtr = const_cast< FSSpec& >( item ).name;
		pb.hFileInfo.ioFDirIndex = 0;	// use ioNamePtr and ioDirID
		pb.hFileInfo.ioVRefNum = item.vRefNum;
		pb.hFileInfo.ioDirID = item.parID;
		N::ThrowOSStatus( ::PBGetCatInfoSync( &pb ) );
	}
	
}

