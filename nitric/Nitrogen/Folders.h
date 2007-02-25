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
	
	static const FSVolumeRefNum kOnSystemDisk      = FSVolumeRefNum( ::kOnSystemDisk      );
	static const FSVolumeRefNum kOnAppropriateDisk = FSVolumeRefNum( ::kOnAppropriateDisk );
	
	static const FSVolumeRefNum kSystemDomain  = FSVolumeRefNum( ::kSystemDomain  );
	static const FSVolumeRefNum kLocalDomain   = FSVolumeRefNum( ::kLocalDomain   );
	static const FSVolumeRefNum kNetworkDomain = FSVolumeRefNum( ::kNetworkDomain );
	static const FSVolumeRefNum kUserDomain    = FSVolumeRefNum( ::kUserDomain    );
	static const FSVolumeRefNum kClassicDomain = FSVolumeRefNum( ::kClassicDomain );
	
	enum FolderType
	{
		kSystemFolderType             = ::kSystemFolderType,
		kDesktopFolderType            = ::kDesktopFolderType,
		kSystemDesktopFolderType      = ::kSystemDesktopFolderType,
		kTrashFolderType              = ::kTrashFolderType,
		kSystemTrashFolderType        = ::kSystemTrashFolderType,
		kWhereToEmptyTrashFolderType  = ::kWhereToEmptyTrashFolderType,
		kPrintMonitorDocsFolderType   = ::kPrintMonitorDocsFolderType,
		kStartupFolderType            = ::kStartupFolderType,
		kShutdownFolderType           = ::kShutdownFolderType,
		kAppleMenuFolderType          = ::kAppleMenuFolderType,
		kControlPanelFolderType       = ::kControlPanelFolderType,
		kSystemControlPanelFolderType = ::kSystemControlPanelFolderType,
		kExtensionFolderType          = ::kExtensionFolderType,
		kFontsFolderType              = ::kFontsFolderType,
		kPreferencesFolderType        = ::kPreferencesFolderType,
		kSystemPreferencesFolderType  = ::kSystemPreferencesFolderType,
		kTemporaryFolderType          = ::kTemporaryFolderType,
		
		kFolderType_Max = Nucleus::Enumeration_Traits< ::OSType >::max
	};
	
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
