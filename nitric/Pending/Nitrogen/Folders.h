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
   class FolderTypeTag {};
   typedef SelectorType< FolderTypeTag, ::OSType > FolderType;

   inline FSVolumeRefNum OnSystemDisk()        { return FSVolumeRefNum::Make( kOnSystemDisk      ); }    
   inline FSVolumeRefNum OnAppropriateDisk()   { return FSVolumeRefNum::Make( kOnAppropriateDisk ); }    
   inline FSVolumeRefNum SystemDomain()        { return FSVolumeRefNum::Make( kSystemDomain      ); }    
   inline FSVolumeRefNum LocalDomain()         { return FSVolumeRefNum::Make( kLocalDomain       ); }    
   inline FSVolumeRefNum NetworkDomain()       { return FSVolumeRefNum::Make( kNetworkDomain     ); }    
   inline FSVolumeRefNum UserDomain()          { return FSVolumeRefNum::Make( kUserDomain        ); }    
   inline FSVolumeRefNum ClassicDomain()       { return FSVolumeRefNum::Make( kClassicDomain     ); }    

   FSRef FSFindFolder( FSVolumeRefNum vRefNum, FolderType folderType, bool createFolder );

   inline FSRef FSFindFolder( FolderType folderType, bool createFolder )
     {
      return FSFindFolder( OnAppropriateDisk(), folderType, createFolder );
     }
  }

#endif
