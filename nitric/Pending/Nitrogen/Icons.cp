// Icons.cp

#ifndef NITROGEN_ICONS_H
#include "Nitrogen/Icons.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_FOLDERS_H
#include "Nitrogen/Folders.h"
#endif

namespace Nitrogen
  {
   Owned<IconRef> GetIconRef( FSVolumeRefNum vRefNum, OSType creator, OSType iconType )
     {
      OnlyOnce<RegisterIconManagerErrors>();
      IconRef result;
      ThrowOSStatus( ::GetIconRef( vRefNum, creator, iconType, &result ) );
      return Owned<IconRef>::Seize( result );
     }

   GetIconRefFromFile_Result GetIconRefFromFile( const FSSpec& theFile )
     {
      OnlyOnce<RegisterIconManagerErrors>();
      IconRef icon;
      SInt16 label;
      ThrowOSStatus( ::GetIconRefFromFile( &theFile, &icon, &label ) );
      return GetIconRefFromFile_Result( Owned<IconRef>::Seize( icon ),
                                        IconLabel( label ) );
     }

   Owned<IconRef> GetIconRef( OSType creator, OSType iconType )
     {
      return GetIconRef( OnSystemDisk(), creator, iconType );
     }

   Owned<IconRef> GetIconRef( OSType iconType )
     {
      return GetIconRef( OnSystemDisk(), kSystemIconsCreator, iconType );
     }

	Owned<IconRef> GetIconRefFromFolder( FSVolumeRefNum vRefNum,
	                                     FSDirID parentFolderID,
	                                     FSDirID folderID,
	                                     FSIOFileAttributes attributes,
	                                     FSUserPrivileges accessPrivileges )
     {
      OnlyOnce<RegisterIconManagerErrors>();
      IconRef result;
      ThrowOSStatus( ::GetIconRefFromFolder( vRefNum,
                                             parentFolderID,
                                             folderID,
                                             attributes,
                                             accessPrivileges,
                                             &result ) );
      return Owned<IconRef>::Seize( result );
     }

   GetIconRefFromFileInfo_Result GetIconRefFromFileInfo( const FSRef&           inRef,
                                                         UniCharCount           inFileNameLength,
                                                         const UniChar         *inFileName,
                                                         FSCatalogInfoBitmap    inWhichInfo,
                                                         const FSCatalogInfo&   inCatalogInfo,
                                                         IconServicesUsageFlags inUsageFlags )
     {
      OnlyOnce<RegisterIconManagerErrors>();
      IconRef icon;
      SInt16 label;
      ThrowOSStatus( ::GetIconRefFromFileInfo( &inRef,
                                               inFileNameLength,
                                               inFileName,
                                               inWhichInfo,
                                               &inCatalogInfo,
                                               inUsageFlags,
                                               &icon,
                                               &label ) );
      return GetIconRefFromFile_Result( Owned<IconRef>::Seize( icon ),
                                        IconLabel( label ) );
     }

   GetIconRefFromFileInfo_Result GetIconRefFromFileInfo( const FSRef&           inRef,
                                                         UniCharCount           inFileNameLength,
                                                         const UniChar         *inFileName,
                                                         IconServicesUsageFlags inUsageFlags )
     {
      OnlyOnce<RegisterIconManagerErrors>();
      IconRef icon;
      SInt16 label;
      ThrowOSStatus( ::GetIconRefFromFileInfo( &inRef,
                                               inFileNameLength,
                                               inFileName,
                                               0,
                                               0,
                                               inUsageFlags,
                                               &icon,
                                               &label ) );
      return GetIconRefFromFile_Result( Owned<IconRef>::Seize( icon ),
                                        IconLabel( label ) );
     }

   Owned<IconRef> RegisterIconRefFromFSRef( OSType creator, OSType iconType, const FSRef& iconFile )
     {
      OnlyOnce<RegisterIconManagerErrors>();
      IconRef result;
      ThrowOSStatus( ::RegisterIconRefFromFSRef( creator, iconType, &iconFile, &result ) );
      return Owned<IconRef>::Seize( result );
     }

   Owned<IconRef> RegisterIconRefFromIconFile( OSType creator,
                                               OSType iconType,
                                               const FSSpec& iconFile )
	  {
      OnlyOnce<RegisterIconManagerErrors>();
      IconRef result;
      ThrowOSStatus( ::RegisterIconRefFromIconFile( creator, iconType, &iconFile, &result ) );
      return Owned<IconRef>::Seize( result );
     }
   
   void RegisterIconManagerErrors()
     {
      RegisterOSStatus< paramErr               >();
      RegisterOSStatus< memFullErr             >();
      RegisterOSStatus< memWZErr               >();
      RegisterOSStatus< resNotFound            >();
      RegisterOSStatus< noMaskFoundErr         >();
      RegisterOSStatus< invalidIconRefErr      >();
      RegisterOSStatus< noSuchIconErr          >();
      RegisterOSStatus< noIconDataAvailableErr >();
      RegisterOSStatus< afpIconTypeError       >();
     }
  }
