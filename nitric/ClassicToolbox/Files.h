/*	=======
 *	Files.h
 *	=======
 */

#ifndef CLASSICTOOLBOX_FILES_H
#define CLASSICTOOLBOX_FILES_H

// Nitrogen
#include "Nitrogen/Files.h"


namespace Nitrogen
{
	
	void Eject( ConstStr63Param volName );
	
	void Eject( FSVolumeRefNum vRefNum );
	
	void MountVol( FSVolumeRefNum driveNumber );
	
}

#endif

