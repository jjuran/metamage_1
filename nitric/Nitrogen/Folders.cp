// Folders.cp

#ifndef NITROGEN_FOLDERS_H
#include "Nitrogen/Folders.h"
#endif

namespace Nitrogen
  {
   
	FSDirSpec FindFolder( FSVolumeRefNum  vRefNum,
	                      FolderType      folderType,
	                      bool            createFolder )
	{
		::FSVolumeRefNum outVRefNum;
		SInt32 dirID;
		
		ThrowOSStatus( ::FindFolder( vRefNum,
		                             folderType,
		                             createFolder,
		                             &outVRefNum,
		                             &dirID ) );
		
		return Nucleus::Make< FSDirSpec >( FSVolumeRefNum( outVRefNum ), FSDirID( dirID ) );
	}
	
   FSRef FSFindFolder( FSVolumeRefNum vRefNum, FolderType folderType, bool createFolder )
     {
      FSRef result;
      ThrowOSStatus( ::FSFindFolder( vRefNum, folderType, createFolder, &result ) );
      return result;
     }
     
  }
