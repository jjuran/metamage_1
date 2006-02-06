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
	
	Nucleus::Owned< CIconHandle > GetCIcon( ResID iconID )
	{
		return Nucleus::Owned< CIconHandle >::Seize( ::GetCIcon( iconID ) );
	}
	
	void PlotCIcon( const Rect& rect, CIconHandle icon )
	{
		::PlotCIcon( &rect, icon );
	}
	
	PlainIconHandle GetIcon( ResID iconID )
	{
		// Returns a resource handle
		return Handle_Cast< PlainIcon >( Handle( ::GetIcon( iconID ) ) );
	}
	
	void PlotIcon( const Rect& rect, PlainIconHandle icon )
	{
		::PlotIcon( &rect, Handle( icon ) );
	}
	
	void PlotIconID( const Rect& rect,
	                 IconAlignmentType align,
	                 IconTransformType transform,
	                 ResID resID )
	{
		ThrowOSStatus( ::PlotIconID( &rect, align, transform, resID ) );
	}
	
	Nucleus::Owned< IconSuiteRef > NewIconSuite()
	{
		::IconSuiteRef result;
		ThrowOSStatus( ::NewIconSuite( &result ) );
		return Nucleus::Owned< IconSuiteRef >::Seize( IconSuiteRef( result ) );
	}
	
	void DisposeIconSuite( Nucleus::Owned< IconSuiteRef > iconSuite )
	{
		ThrowOSStatus( ::DisposeIconSuite( iconSuite.Release(), true ) );
	}
	
	void DisposeIconSuite( Nucleus::Owned< IconSuiteRef, DisposeIconSuiteButNotData > iconSuite )
	{
		ThrowOSStatus( ::DisposeIconSuite( iconSuite.Release(), false ) );
	}
	
	void PlotIconSuite( const Rect&        rect,
	                    IconAlignmentType  align,
	                    IconTransformType  transform,
	                    IconSuiteRef       iconSuite )
	{
		ThrowOSStatus( ::PlotIconSuite( &rect, align, transform, iconSuite ) );
	}
	
	static void PlotIconHandle_Internal( const Rect&        area,
	                                     IconAlignmentType  align,
	                                     IconTransformType  transform,
	                                     Handle             icon )
	{
		ThrowOSStatus( ::PlotIconHandle( &area, align, transform, icon ) );
	}
	
	void PlotIconHandle( const Rect&        area,
	                     IconAlignmentType  align,
	                     IconTransformType  transform,
	                     PlainIconHandle    icon )
	{
		PlotIconHandle_Internal( area, align, transform, icon );
	}
	
	void PlotIconHandle( const Rect&        area,
	                     IconAlignmentType  align,
	                     IconTransformType  transform,
	                     MaskedIconHandle   icon )
	{
		PlotIconHandle_Internal( area, align, transform, icon );
	}
	
	// The plot sickens...
	
	void PlotSICNHandle( const Rect&        area,
	                     IconAlignmentType  align,
	                     IconTransformType  transform,
	                     SmallIconHandle    theSICN )
	{
		ThrowOSStatus( ::PlotSICNHandle( &area, align, transform, Handle( theSICN ) ) );
	}
	
	void PlotCIconHandle( const Rect&        rect,
	                      IconAlignmentType  align,
	                      IconTransformType  transform,
	                      CIconHandle        theCIcon )
	{
		ThrowOSStatus( ::PlotCIconHandle( &rect, align, transform, theCIcon ) );
	}
	
   Nucleus::Owned<IconRef> GetIconRef( FSVolumeRefNum vRefNum, OSType creator, OSType iconType )
     {
      Nucleus::OnlyOnce<RegisterIconManagerErrors>();
      IconRef result;
      ThrowOSStatus( ::GetIconRef( vRefNum, creator, iconType, &result ) );
      return Nucleus::Owned<IconRef>::Seize( result );
     }

   GetIconRefFromFile_Result GetIconRefFromFile( const FSSpec& theFile )
     {
      Nucleus::OnlyOnce<RegisterIconManagerErrors>();
      IconRef icon;
      SInt16 label;
      ThrowOSStatus( ::GetIconRefFromFile( &theFile, &icon, &label ) );
      return GetIconRefFromFile_Result( Nucleus::Owned<IconRef>::Seize( icon ),
                                        IconLabel( label ) );
     }

   Nucleus::Owned<IconRef> GetIconRef( OSType creator, OSType iconType )
     {
      return GetIconRef( OnSystemDisk(), creator, iconType );
     }

   Nucleus::Owned<IconRef> GetIconRef( OSType iconType )
     {
      return GetIconRef( OnSystemDisk(), kSystemIconsCreator, iconType );
     }

	Nucleus::Owned<IconRef> GetIconRefFromFolder( FSVolumeRefNum vRefNum,
	                                     FSDirID parentFolderID,
	                                     FSDirID folderID,
	                                     FSIOFileAttributes attributes,
	                                     FSUserPrivileges accessPrivileges )
     {
      Nucleus::OnlyOnce<RegisterIconManagerErrors>();
      IconRef result;
      ThrowOSStatus( ::GetIconRefFromFolder( vRefNum,
                                             parentFolderID,
                                             folderID,
                                             attributes,
                                             accessPrivileges,
                                             &result ) );
      return Nucleus::Owned<IconRef>::Seize( result );
     }

   GetIconRefFromFileInfo_Result GetIconRefFromFileInfo( const FSRef&           inRef,
                                                         UniCharCount           inFileNameLength,
                                                         const UniChar         *inFileName,
                                                         FSCatalogInfoBitmap    inWhichInfo,
                                                         const FSCatalogInfo&   inCatalogInfo,
                                                         IconServicesUsageFlags inUsageFlags )
     {
      Nucleus::OnlyOnce<RegisterIconManagerErrors>();
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
      return GetIconRefFromFile_Result( Nucleus::Owned<IconRef>::Seize( icon ),
                                        IconLabel( label ) );
     }

   GetIconRefFromFileInfo_Result GetIconRefFromFileInfo( const FSRef&           inRef,
                                                         UniCharCount           inFileNameLength,
                                                         const UniChar         *inFileName,
                                                         IconServicesUsageFlags inUsageFlags )
     {
      Nucleus::OnlyOnce<RegisterIconManagerErrors>();
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
      return GetIconRefFromFile_Result( Nucleus::Owned<IconRef>::Seize( icon ),
                                        IconLabel( label ) );
     }

   Nucleus::Owned<IconRef> RegisterIconRefFromFSRef( OSType creator, OSType iconType, const FSRef& iconFile )
     {
      Nucleus::OnlyOnce<RegisterIconManagerErrors>();
      IconRef result;
      ThrowOSStatus( ::RegisterIconRefFromFSRef( creator, iconType, &iconFile, &result ) );
      return Nucleus::Owned<IconRef>::Seize( result );
     }

   Nucleus::Owned<IconRef> RegisterIconRefFromIconFile( OSType creator,
                                               OSType iconType,
                                               const FSSpec& iconFile )
	  {
      Nucleus::OnlyOnce<RegisterIconManagerErrors>();
      IconRef result;
      ThrowOSStatus( ::RegisterIconRefFromIconFile( creator, iconType, &iconFile, &result ) );
      return Nucleus::Owned<IconRef>::Seize( result );
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

