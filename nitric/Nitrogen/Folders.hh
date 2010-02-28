// Nitrogen/Folders.hh
// -------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2003-2007 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_FOLDERS_HH
#define NITROGEN_FOLDERS_HH

#ifndef __FOLDERS__
#include <Folders.h>
#endif
#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif
#ifndef NITROGEN_FILES_HH
#include "Nitrogen/Files.hh"
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
		
		kExtensionDisabledFolderType       = ::kExtensionDisabledFolderType,
		kControlPanelDisabledFolderType    = ::kControlPanelDisabledFolderType,
		kSystemExtensionDisabledFolderType = ::kSystemExtensionDisabledFolderType,
		kStartupItemsDisabledFolderType    = ::kStartupItemsDisabledFolderType,
		kShutdownItemsDisabledFolderType   = ::kShutdownItemsDisabledFolderType,
		kApplicationsFolderType            = ::kApplicationsFolderType,
		kDocumentsFolderType               = ::kDocumentsFolderType,
		
		kVolumeRootFolderType          = ::kVolumeRootFolderType,
		kChewableItemsFolderType       = ::kChewableItemsFolderType,
		kApplicationSupportFolderType  = ::kApplicationSupportFolderType,
		kTextEncodingsFolderType       = ::kTextEncodingsFolderType,
		kStationeryFolderType          = ::kStationeryFolderType,
		kOpenDocFolderType             = ::kOpenDocFolderType,
		kOpenDocShellPlugInsFolderType = ::kOpenDocShellPlugInsFolderType,
		kEditorsFolderType             = ::kEditorsFolderType,
		kOpenDocEditorsFolderType      = ::kOpenDocEditorsFolderType,
		kOpenDocLibrariesFolderType    = ::kOpenDocLibrariesFolderType,
		kGenEditorsFolderType          = ::kGenEditorsFolderType,
		kHelpFolderType                = ::kHelpFolderType,
		kInternetPlugInFolderType      = ::kInternetPlugInFolderType,
		kModemScriptsFolderType        = ::kModemScriptsFolderType,
		kPrinterDescriptionFolderType  = ::kPrinterDescriptionFolderType,
		kPrinterDriverFolderType       = ::kPrinterDriverFolderType,
		kScriptingAdditionsFolderType  = ::kScriptingAdditionsFolderType,
		kSharedLibrariesFolderType     = ::kSharedLibrariesFolderType,
		kVoicesFolderType              = ::kVoicesFolderType,
		kControlStripModulesFolderType = ::kControlStripModulesFolderType,
		kAssistantsFolderType          = ::kAssistantsFolderType,
		kUtilitiesFolderType           = ::kUtilitiesFolderType,
		kAppleExtrasFolderType         = ::kAppleExtrasFolderType,
		kContextualMenuItemsFolderType = ::kContextualMenuItemsFolderType,
		kMacOSReadMesFolderType        = ::kMacOSReadMesFolderType,
		kALMModulesFolderType          = ::kALMModulesFolderType,
		kALMPreferencesFolderType      = ::kALMPreferencesFolderType,
		kALMLocationsFolderType        = ::kALMLocationsFolderType,
		kColorSyncProfilesFolderType   = ::kColorSyncProfilesFolderType,
		kThemesFolderType              = ::kThemesFolderType,
		kFavoritesFolderType           = ::kFavoritesFolderType,
		kInternetFolderType            = ::kInternetFolderType,
		kAppearanceFolderType          = ::kAppearanceFolderType,
		kSoundSetsFolderType           = ::kSoundSetsFolderType,
		kDesktopPicturesFolderType     = ::kDesktopPicturesFolderType,
		kInternetSearchSitesFolderType = ::kInternetSearchSitesFolderType,
		kFindSupportFolderType         = ::kFindSupportFolderType,
		kFindByContentFolderType       = ::kFindByContentFolderType,
		kInstallerLogsFolderType       = ::kInstallerLogsFolderType,
		kScriptsFolderType             = ::kScriptsFolderType,
		kFolderActionsFolderType       = ::kFolderActionsFolderType,
		kLauncherItemsFolderType       = ::kLauncherItemsFolderType,
		kRecentApplicationsFolderType  = ::kRecentApplicationsFolderType,
		kRecentDocumentsFolderType     = ::kRecentDocumentsFolderType,
		kRecentServersFolderType       = ::kRecentServersFolderType,
		kSpeakableItemsFolderType      = ::kSpeakableItemsFolderType,
		kKeychainFolderType            = ::kKeychainFolderType,
		kQuickTimeExtensionsFolderType = ::kQuickTimeExtensionsFolderType,
		kDisplayExtensionsFolderType   = ::kDisplayExtensionsFolderType,
		kMultiprocessingFolderType     = ::kMultiprocessingFolderType,
		kPrintingPlugInsFolderType     = ::kPrintingPlugInsFolderType,
		
		kDomainTopLevelFolderType     = ::kDomainTopLevelFolderType,
		kDomainLibraryFolderType      = ::kDomainLibraryFolderType,
		kColorSyncFolderType          = ::kColorSyncFolderType,
		kColorSyncCMMFolderType       = ::kColorSyncCMMFolderType,
		kColorSyncScriptingFolderType = ::kColorSyncScriptingFolderType,
		kPrintersFolderType           = ::kPrintersFolderType,
		kSpeechFolderType             = ::kSpeechFolderType,
		kCarbonLibraryFolderType      = ::kCarbonLibraryFolderType,
		kDocumentationFolderType      = ::kDocumentationFolderType,
		kDeveloperDocsFolderType      = ::kDeveloperDocsFolderType,
		kDeveloperHelpFolderType      = ::kDeveloperHelpFolderType,
		kISSDownloadsFolderType       = ::kISSDownloadsFolderType,
		kUserSpecificTmpFolderType    = ::kUserSpecificTmpFolderType,
		kCachedDataFolderType         = ::kCachedDataFolderType,
		kFrameworksFolderType         = ::kFrameworksFolderType,
		kPrivateFrameworksFolderType  = ::kPrivateFrameworksFolderType,
		kClassicDesktopFolderType     = ::kClassicDesktopFolderType,
		
		kDeveloperFolderType                = ::kDeveloperFolderType,
		kSystemSoundsFolderType             = ::kSystemSoundsFolderType,
		kComponentsFolderType               = ::kComponentsFolderType,
		kQuickTimeComponentsFolderType      = ::kQuickTimeComponentsFolderType,
		kCoreServicesFolderType             = ::kCoreServicesFolderType,
		kPictureDocumentsFolderType         = ::kPictureDocumentsFolderType,
		kMovieDocumentsFolderType           = ::kMovieDocumentsFolderType,
		kMusicDocumentsFolderType           = ::kMusicDocumentsFolderType,
		kInternetSitesFolderType            = ::kInternetSitesFolderType,
		kPublicFolderType                   = ::kPublicFolderType,
		kAudioSupportFolderType             = ::kAudioSupportFolderType,
		kAudioSoundsFolderType              = ::kAudioSoundsFolderType,
		kAudioSoundBanksFolderType          = ::kAudioSoundBanksFolderType,
		kAudioAlertSoundsFolderType         = ::kAudioAlertSoundsFolderType,
		kAudioPlugInsFolderType             = ::kAudioPlugInsFolderType,
		kAudioComponentsFolderType          = ::kAudioComponentsFolderType,
		kKernelExtensionsFolderType         = ::kKernelExtensionsFolderType,
		kDirectoryServicesFolderType        = ::kDirectoryServicesFolderType,
		kDirectoryServicesPlugInsFolderType = ::kDirectoryServicesPlugInsFolderType,
		kInstallerReceiptsFolderType        = ::kInstallerReceiptsFolderType,
		kFileSystemSupportFolderType        = ::kFileSystemSupportFolderType,
		kAppleShareSupportFolderType        = ::kAppleShareSupportFolderType,
		kAppleShareAuthenticationFolderType = ::kAppleShareAuthenticationFolderType,
		kMIDIDriversFolderType              = ::kMIDIDriversFolderType,
		
		kLocalesFolderType              = ::kLocalesFolderType,
		kFindByContentPluginsFolderType = ::kFindByContentPluginsFolderType,
		
		kUsersFolderType                 = ::kUsersFolderType,
		kCurrentUserFolderType           = ::kCurrentUserFolderType,
		kCurrentUserRemoteFolderLocation = ::kCurrentUserRemoteFolderLocation,
		kCurrentUserRemoteFolderType     = ::kCurrentUserRemoteFolderType,
		kSharedUserDataFolderType        = ::kSharedUserDataFolderType,
		kVolumeSettingsFolderType        = ::kVolumeSettingsFolderType,
		
		kAppleshareAutomountServerAliasesFolderType = ::kAppleshareAutomountServerAliasesFolderType,
		kPreMacOS91ApplicationsFolderType           = ::kPreMacOS91ApplicationsFolderType,
		kPreMacOS91InstallerLogsFolderType          = ::kPreMacOS91InstallerLogsFolderType,
		kPreMacOS91AssistantsFolderType             = ::kPreMacOS91AssistantsFolderType,
		kPreMacOS91UtilitiesFolderType              = ::kPreMacOS91UtilitiesFolderType,
		kPreMacOS91AppleExtrasFolderType            = ::kPreMacOS91AppleExtrasFolderType,
		kPreMacOS91MacOSReadMesFolderType           = ::kPreMacOS91MacOSReadMesFolderType,
		kPreMacOS91InternetFolderType               = ::kPreMacOS91InternetFolderType,
		kPreMacOS91AutomountedServersFolderType     = ::kPreMacOS91AutomountedServersFolderType,
		kPreMacOS91StationeryFolderType             = ::kPreMacOS91StationeryFolderType,
		
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
