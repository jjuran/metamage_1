// Icons.h

#ifndef NITROGEN_ICONS_H
#define NITROGEN_ICONS_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __ICONS__
#include FRAMEWORK_HEADER(HIServices,Icons.h)
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_QUICKDRAW_H
#include "Nitrogen/QuickDraw.h"
#endif
#ifndef NITROGEN_FILES_H
#include "Nitrogen/Files.h"
#endif
#ifndef NITROGEN_CODEFRAGMENTS_H
#include "Nitrogen/CodeFragments.h"
#endif
#ifndef NITROGEN_CGCONTEXT_H
#include "Nitrogen/CGContext.h"
#endif
#ifndef NITROGEN_RESOURCETRANSFER_H
#include "Nitrogen/ResourceTransfer.h"
#endif

namespace Nitrogen
  {
   void RegisterIconManagerErrors();
   
   using ::IconRef;

   static const OSType kSystemIconsCreator = OSType::Make( ::kSystemIconsCreator );
   
   class IconLabelTag {};
   typedef IDType< IconLabelTag, SInt16, 0 > IconLabel;
   
   class IconServicesUsageFlagsTag {};
   typedef FlagType< IconServicesUsageFlagsTag, ::IconServicesUsageFlags, 0 > IconServicesUsageFlags;
   
   static const IconServicesUsageFlags kIconServicesNormalUsageFlag = IconServicesUsageFlags::Make( ::kIconServicesNormalUsageFlag );
   
   template <> struct Disposer< IconRef >: public std::unary_function< IconRef, void >,
                                           private DefaultDestructionOSStatusPolicy
     {
      void operator()( IconRef i ) const
        {
         OnlyOnce<RegisterIconManagerErrors>();
         DefaultDestructionOSStatusPolicy::HandleDestructionOSStatus( ::ReleaseIconRef( i ) );
        }
     };
	
   struct GetIconRefFromFile_Result
     {
      Owned<IconRef> theIconRef;
      IconLabel theLabel;
      
      GetIconRefFromFile_Result( Owned<IconRef> icon, IconLabel label )
        : theIconRef( icon ),
          theLabel( label )
        {}
      
      GetIconRefFromFile_Result( ResourceTransfer<GetIconRefFromFile_Result> r )
        : theIconRef( r->theIconRef ),
          theLabel( r->theLabel )
        {}
      
      operator ResourceTransfer<GetIconRefFromFile_Result>()
        {
         return ResourceTransfer<GetIconRefFromFile_Result>( *this );
        }
      
      operator Owned<IconRef>()            { return theIconRef; }
      operator IconRef() const             { return theIconRef; }
     };

   GetIconRefFromFile_Result GetIconRefFromFile( const FSSpec& theFile );

   Owned<IconRef> GetIconRef( FSVolumeRefNum vRefNum, OSType creator, OSType iconType );

   Owned<IconRef> GetIconRef( OSType creator, OSType iconType );

   Owned<IconRef> GetIconRef( OSType iconType );

	Owned<IconRef> GetIconRefFromFolder( FSVolumeRefNum vRefNum,
	                                     FSDirID parentFolderID,
	                                     FSDirID folderID,
	                                     FSIOFileAttributes attributes,
	                                     FSUserPrivileges accessPrivileges );

   typedef GetIconRefFromFile_Result GetIconRefFromFileInfo_Result;
   
   GetIconRefFromFileInfo_Result GetIconRefFromFileInfo( const FSRef&           inRef,
                                                         UniCharCount           inFileNameLength,
                                                         const UniChar         *inFileName,
                                                         FSCatalogInfoBitmap    inWhichInfo,
                                                         const FSCatalogInfo&   inCatalogInfo,
                                                         IconServicesUsageFlags inUsageFlags = kIconServicesNormalUsageFlag );

   GetIconRefFromFileInfo_Result GetIconRefFromFileInfo( const FSRef&           inRef,
                                                         UniCharCount           inFileNameLength,
                                                         const UniChar         *inFileName,
                                                         IconServicesUsageFlags inUsageFlags = kIconServicesNormalUsageFlag );

   inline GetIconRefFromFileInfo_Result GetIconRefFromFileInfo( const FSRef&           inRef,
                                                                FSCatalogInfoBitmap    inWhichInfo,
                                                                const FSCatalogInfo&   inCatalogInfo,
                                                                IconServicesUsageFlags inUsageFlags = kIconServicesNormalUsageFlag )
     {
      return GetIconRefFromFileInfo( inRef, 0, 0, inWhichInfo, inCatalogInfo, inUsageFlags );
     }

   inline GetIconRefFromFileInfo_Result GetIconRefFromFileInfo( const FSRef&           inRef,
                                                                IconServicesUsageFlags inUsageFlags = kIconServicesNormalUsageFlag )
     {
      return GetIconRefFromFileInfo( inRef, 0, 0, inUsageFlags );
     }

   inline GetIconRefFromFileInfo_Result GetIconRefFromFileInfo( const FSRef&           inRef,
                                                                const UniString&       inFileName,
                                                                FSCatalogInfoBitmap    inWhichInfo,
                                                                const FSCatalogInfo&   inCatalogInfo,
                                                                IconServicesUsageFlags inUsageFlags = kIconServicesNormalUsageFlag )
     {
      return GetIconRefFromFileInfo( inRef,
                                     inFileName.size(),
                                     inFileName.empty() ? 0 : &*inFileName.begin(),
                                     inWhichInfo,
                                     inCatalogInfo,
                                     inUsageFlags );
     }

   inline GetIconRefFromFileInfo_Result GetIconRefFromFileInfo( const FSRef&           inRef,
                                                                const UniString&       inFileName,
                                                                IconServicesUsageFlags inUsageFlags = kIconServicesNormalUsageFlag )
     {
      return GetIconRefFromFileInfo( inRef,
                                     inFileName.size(),
                                     inFileName.empty() ? 0 : &*inFileName.begin(),
                                     inUsageFlags );
     }

   inline GetIconRefFromFileInfo_Result GetIconRefFromFileInfo( const FSRef&           inRef,
                                                                const HFSUniStr255&    inFileName,
                                                                FSCatalogInfoBitmap    inWhichInfo,
                                                                const FSCatalogInfo&   inCatalogInfo,
                                                                IconServicesUsageFlags inUsageFlags = kIconServicesNormalUsageFlag )
     {
      return GetIconRefFromFileInfo( inRef,
                                     inFileName.length,
                                     inFileName.unicode,
                                     inWhichInfo,
                                     inCatalogInfo,
                                     inUsageFlags );
     }

   inline GetIconRefFromFileInfo_Result GetIconRefFromFileInfo( const FSRef&           inRef,
                                                                const HFSUniStr255&    inFileName,
                                                                IconServicesUsageFlags inUsageFlags = kIconServicesNormalUsageFlag )
     {
      return GetIconRefFromFileInfo( inRef,
                                     inFileName.length,
                                     inFileName.unicode,
                                     inUsageFlags );
     }

   Owned<IconRef> RegisterIconRefFromFSRef( OSType creator, OSType iconType, const FSRef& iconFile );
   
	Owned<IconRef> RegisterIconRefFromIconFile( OSType creator,
                                               OSType iconType,
                                               const FSSpec& iconFile );

   inline Owned<IconRef> RegisterIconRefFromIconFile( const FSSpec& iconFile )
     {
      return RegisterIconRefFromIconFile( kSystemIconsCreator, OSType::Make( 0 ), iconFile );
     }
  }

#endif
