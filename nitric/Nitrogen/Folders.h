// Folders.h

#ifndef NITROGEN_FOLDERS_H
#define NITROGEN_FOLDERS_H

#ifndef __FOLDERS__
#include <Folders.h>
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_FILES_H
#include "Nitrogen/Files.h"
#endif

namespace Nitrogen
  {
   class FolderTypeTag {};
   typedef SelectorType< FolderTypeTag, ::OSType > FolderType;

   class VolumeTypeTag {};
   typedef SelectorType< VolumeTypeTag, short > VolumeType;

   inline VolumeType OnSystemDisk()        { return VolumeType::Make( kOnSystemDisk      ); }    
   inline VolumeType OnAppropriateDisk()   { return VolumeType::Make( kOnAppropriateDisk ); }    
   inline VolumeType SystemDomain()        { return VolumeType::Make( kSystemDomain      ); }    
   inline VolumeType LocalDomain()         { return VolumeType::Make( kLocalDomain       ); }    
   inline VolumeType NetworkDomain()       { return VolumeType::Make( kNetworkDomain     ); }    
   inline VolumeType UserDomain()          { return VolumeType::Make( kUserDomain        ); }    
   inline VolumeType ClassicDomain()       { return VolumeType::Make( kClassicDomain     ); }    

   FSRef FSFindFolder( FSVolumeRefNum vRefNum, FolderType folderType, bool createFolder );

   inline FSRef FSFindFolder( VolumeType volume, FolderType folderType, bool createFolder )
     {
      return FSFindFolder( FSVolumeRefNum::Make( volume ), folderType, createFolder );
     }

   inline FSRef FSFindFolder( FolderType folderType, bool createFolder )
     {
      return FSFindFolder( OnAppropriateDisk(), folderType, createFolder );
     }
  }

#endif
