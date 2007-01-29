// Folders.h

#ifndef NITROGEN_FOLDERS_H
#define NITROGEN_FOLDERS_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __FOLDERS__
#include FRAMEWORK_HEADER(CarbonCore,Folders.h)
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_FILES_H
#include "Nitrogen/Files.h"
#endif

namespace Nitrogen
  {
   typedef Nucleus::Selector< class FolderType_Tag, ::OSType >::Type FolderType;
	
	static const FSVolumeRefNum kOnSystemDisk      = FSVolumeRefNum( ::kOnSystemDisk      );
	static const FSVolumeRefNum kOnAppropriateDisk = FSVolumeRefNum( ::kOnAppropriateDisk );
	
	static const FSVolumeRefNum kSystemDomain  = FSVolumeRefNum( ::kSystemDomain  );
	static const FSVolumeRefNum kLocalDomain   = FSVolumeRefNum( ::kLocalDomain   );
	static const FSVolumeRefNum kNetworkDomain = FSVolumeRefNum( ::kNetworkDomain );
	static const FSVolumeRefNum kUserDomain    = FSVolumeRefNum( ::kUserDomain    );
	static const FSVolumeRefNum kClassicDomain = FSVolumeRefNum( ::kClassicDomain );
	
	static const FolderType kSystemFolderType = FolderType( ::kSystemFolderType );
	// ...
	static const FolderType kTemporaryFolderType = FolderType( ::kTemporaryFolderType );
	
	static const FolderType kCurrentUserFolderType = FolderType( ::kCurrentUserFolderType );
	
	FSDirSpec FindFolder( FSVolumeRefNum vRefNum, FolderType folderType, bool createFolder );
	
	inline FSDirSpec FindFolder( FolderType folderType, bool createFolder )
	{
		return FindFolder( kOnAppropriateDisk, folderType, createFolder );
	}
	
   FSRef FSFindFolder( FSVolumeRefNum vRefNum, FolderType folderType, bool createFolder );

   inline FSRef FSFindFolder( FolderType folderType, bool createFolder )
     {
      return FSFindFolder( kOnAppropriateDisk, folderType, createFolder );
     }
  }

#endif
