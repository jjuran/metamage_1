// Folders.cp

#ifndef NITROGEN_FOLDERS_H
#include "Nitrogen/Folders.h"
#endif

namespace Nitrogen
  {
   
	FSDirSpec FindFolder( FSVolumeRefNum vRefNum, FolderType folderType, bool createFolder )
	{
		::FSVolumeRefNum outVRefNum;
		SInt32 dirID;
		ThrowOSStatus
		(
			::FindFolder( vRefNum, folderType, createFolder, &outVRefNum, &dirID )
		);
		
		//return FSMakeFSSpec( FSVolumeRefNum( outVRefNum ), dirID, NULL );
		return Make< FSDirSpec >( FSVolumeRefNum( outVRefNum ), dirID );
	}
	
   FSRef FSFindFolder( FSVolumeRefNum vRefNum, FolderType folderType, bool createFolder )
     {
      FSRef result;
      ThrowOSStatus( ::FSFindFolder( vRefNum, folderType, createFolder, &result ) );
      return result;
     }
     
  }
