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
	
	// 254
	struct IconAlignmentType_Tag  {};
	typedef FlagType< IconAlignmentType_Tag, ::IconAlignmentType, kAlignNone > IconAlignmentType;
	
	// 293
	struct IconTransformType_Tag  {};
	typedef FlagType< IconTransformType_Tag, ::IconTransformType, kTransformNone > IconTransformType;
	
	struct IconSelectorValue_Tag  {};
	typedef FlagType< IconSelectorValue_Tag, ::IconSelectorValue, kSelectorAllAvailableData > IconSelectorValue;
	
   class IconLabelTag {};
   typedef IDType< IconLabelTag, SInt16, 0 > IconLabel;
   
   // 1275
   class IconServicesUsageFlagsTag {};
   typedef FlagType< IconServicesUsageFlagsTag, ::IconServicesUsageFlags, 0 > IconServicesUsageFlags;
   
	
	struct Icon
	{
		long data[32];
	};
	typedef Icon** IconHandle;
	
	struct IconAndMask
	{
		Icon icon;
		Icon mask;
	};
	typedef IconAndMask** IconAndMaskHandle;
	
	template <> struct OwnedDefaults< IconHandle        > : OwnedDefaults< Handle > {};
	template <> struct OwnedDefaults< IconAndMaskHandle > : OwnedDefaults< Handle > {};
	
	using ::CIconHandle;
	
	class IconSuiteRef
	{
		private:
			typedef ::IconSuiteRef UnderlyingType;
			UnderlyingType value;
		
		public:
			IconSuiteRef()                   : value( NULL )  {}
			IconSuiteRef( UnderlyingType v ) : value( v    )  {}
			
			UnderlyingType Get() const       { return value; }
			operator UnderlyingType() const  { return Get(); }
	};
	
	typedef IconSuiteRef IconCacheRef;
	
   // 564
   using ::IconRef;
	
   template <> struct Disposer< CIconHandle >: public std::unary_function< CIconHandle, void >
     {
      void operator()( CIconHandle h ) const
        {
         //OnlyOnce<RegisterIconManagerErrors>();
         ::DisposeCIcon( h );
        }
     };
	
	template <> struct Disposer< IconSuiteRef >: public std::unary_function< IconSuiteRef, void >,
	                                             private DefaultDestructionOSStatusPolicy
	{
		// DisposeIconSuite() takes a Boolean argument that tells it whether to
		// dispose the individual icon data associated with the icon suite.
		// It will not dispose resource handles, regardless of the value passed.
		// I'm hard-coding this argument to true, as that seems the most useful to me.
		// If you need to pass false, use DisposeIconSuiteButNotData instead.
		
		enum { disposeData = true };
		
		void operator()( IconSuiteRef i ) const
		{
			OnlyOnce< RegisterIconManagerErrors >();
			HandleDestructionOSStatus( ::DisposeIconSuite( i, disposeData ) );
		}
	};
	
	struct DisposeIconSuiteButNotData: public std::unary_function< IconSuiteRef, void >,
	                                   private DefaultDestructionOSStatusPolicy
	{
		enum { disposeData = false };
		
		void operator()( IconSuiteRef i ) const
		{
			OnlyOnce< RegisterIconManagerErrors >();
			HandleDestructionOSStatus( ::DisposeIconSuite( i, disposeData ) );
		}
	};
	
   template <> struct Disposer< IconRef >: public std::unary_function< IconRef, void >,
                                           private DefaultDestructionOSStatusPolicy
     {
      void operator()( IconRef i ) const
        {
         OnlyOnce<RegisterIconManagerErrors>();
         HandleDestructionOSStatus( ::ReleaseIconRef( i ) );
        }
     };
	
	// 490
	Owned< CIconHandle > GetCIcon( ResID iconID );
	
	// 502
	void PlotCIcon( const Rect& rect, CIconHandle icon );
	
	// 516
	inline void DisposeCIcon( Owned< CIconHandle > )  {}
	
	// 530
	IconHandle GetIcon( ResID iconID );  // Returns a resource handle
	
	// 542
	void PlotIcon( const Rect& rect, IconHandle icon );
	
	// 566
	void PlotIconID( const Rect&        rect,
	                 IconAlignmentType  align,
	                 IconTransformType  transform,
	                 ResID              resID );
	
	// 582
	Owned< IconSuiteRef > NewIconSuite();
	
	template < bool disposeData >  struct DisposeData_Traits;
	
	template <>  struct DisposeData_Traits< true >
	{
		typedef Disposer<IconSuiteRef > Disposer;
	};
	
	template <>  struct DisposeData_Traits< false >
	{
		typedef DisposeIconSuiteButNotData Disposer;
	};
	
	template < bool disposeData >
	Owned< IconSuiteRef, typename DisposeData_Traits< disposeData >::Disposer > NewIconSuite()
	{
		typedef typename DisposeData_Traits< disposeData >::Disposer Disposer;
		
		return Owned< IconSuiteRef, Disposer >::Seize( NewIconSuite().Release() );
	}
	
	// 594
	void AddIconToSuite( Handle iconData, IconSuiteRef suite, ResType type );
	
	// 609
	Handle GetIconFromSuite( IconSuiteRef suite, ResType type );
	
	// ...
	
	// 640
	Owned< IconSuiteRef > GetIconSuite( ResID resID, IconSelectorValue selector );
	
	// 655
	void DisposeIconSuite( Owned< IconSuiteRef                             > iconSuite );  // true
	void DisposeIconSuite( Owned< IconSuiteRef, DisposeIconSuiteButNotData > iconSuite );  // false
	
	template < bool disposeData >
	void DisposeIconSuite( Owned< IconSuiteRef > iconSuite )
	{
		DisposeIconSuite( iconSuite, disposeData );
	}
	
	// 669
	void PlotIconSuite( const Rect&        rect,
	                    IconAlignmentType  align,
	                    IconTransformType  transform,
	                    IconSuiteRef       iconSuite );
	
	// ...
	
	// 1009
	void PlotCIconHandle( const Rect&        rect,
	                      IconAlignmentType  align,
	                      IconTransformType  transform,
	                      CIconHandle        theCIcon );
	
	// ...
	
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

   // 1614
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
      return RegisterIconRefFromIconFile( kSystemIconsCreator, OSType( 0 ), iconFile );
     }
  }

#endif
