// Folders.cp

#ifndef NITROGEN_FOLDERS_H
#include "Nitrogen/Folders.h"
#endif

namespace Nitrogen
  {
   FSRef FSFindFolder( FSVolumeRefNum vRefNum, FolderType folderType, bool createFolder )
     {
      FSRef result;
      ThrowOSStatus( ::FSFindFolder( vRefNum, folderType, createFolder, &result ) );
      return result;
     }
  }
