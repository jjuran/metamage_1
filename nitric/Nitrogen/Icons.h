// Nitrogen/Icons.h
// ----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2009 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


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
#ifndef NUCLEUS_RESOURCETRANSFER_H
#include "Nucleus/ResourceTransfer.h"
#endif

namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( IconManager );
	
	namespace Constants
	{
		
		static const ResType kPlainIconResourceType = ResType( 'ICON' );
		static const ResType kColorIconResourceType = ResType( 'cicn' );
		
	}
	
	using namespace Constants;
	
	static const ResType kLarge1BitMask  = ResType( ::kLarge1BitMask  );
	static const ResType kLarge4BitData  = ResType( ::kLarge4BitData  );
	static const ResType kLarge8BitData  = ResType( ::kLarge8BitData  );
	static const ResType kLarge32BitData = ResType( ::kLarge32BitData );
	static const ResType kLarge8BitMask  = ResType( ::kLarge8BitMask  );
	static const ResType kSmall1BitMask  = ResType( ::kSmall1BitMask  );
	static const ResType kSmall4BitData  = ResType( ::kSmall4BitData  );
	static const ResType kSmall8BitData  = ResType( ::kSmall8BitData  );
	static const ResType kSmall32BitData = ResType( ::kSmall32BitData );
	static const ResType kSmall8BitMask  = ResType( ::kSmall8BitMask  );
	static const ResType kMini1BitMask   = ResType( ::kMini1BitMask   );
	static const ResType kMini4BitData   = ResType( ::kMini4BitData   );
	static const ResType kMini8BitData   = ResType( ::kMini8BitData   );
	
	
	enum IconAlignmentType
	{
		kIconAlignmentType_Max = Nucleus::Enumeration_Traits< ::IconAlignmentType >::max
	};
	
	enum IconTransformType
	{
		kIconTransformType_Max = Nucleus::Enumeration_Traits< ::IconTransformType >::max
	};
	
	enum IconSelectorValue
	{
		kIconSelectorValue_Max = Nucleus::Enumeration_Traits< ::IconSelectorValue >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( IconAlignmentType )
	NUCLEUS_DEFINE_FLAG_OPS( IconTransformType )
	NUCLEUS_DEFINE_FLAG_OPS( IconSelectorValue )
	
	static const IconAlignmentType kAlignNone = IconAlignmentType( ::kAlignNone );
	
	static const IconTransformType kTransformNone = IconTransformType( ::kTransformNone );
	
	static const IconSelectorValue kSelectorAllAvailableData = IconSelectorValue( ::kSelectorAllAvailableData );
	
	
	enum IconLabel
	{
		kIconLabel_Max = Nucleus::Enumeration_Traits< SInt16 >::max
	};
	
	// ...
	
	static const OSType kSystemIconsCreator = OSType( ::kSystemIconsCreator );
	
	// ResType 'ICON'
	struct PlainIcon
	{
		UInt32 data[ 32 ];
	};
	typedef PlainIcon** PlainIconHandle;
	
	// ResType 'ICN#'
	struct MaskedIcon
	{
		PlainIcon icon;
		PlainIcon mask;
	};
	typedef MaskedIcon** MaskedIconHandle;
	
	typedef MaskedIcon       IconAndMask;
	typedef MaskedIconHandle IconAndMaskHandle;
	
	// ResType 'SICN'
	struct SmallIcon
	{
		UInt16 data[ 16 ];
	};
	typedef SmallIcon** SmallIconHandle;
	
	typedef SmallIcon       SICN;
	typedef SmallIconHandle SICNHandle;
	
	
	template <> struct ResType_Traits< kPlainIconResourceType > : Handle_ResType_Traits< PlainIcon   > {};
	template <> struct ResType_Traits< kColorIconResourceType > : Handle_ResType_Traits< CIcon       > {};
	template <> struct ResType_Traits< kLarge1BitMask         > : Handle_ResType_Traits< MaskedIcon  > {};
	
}

namespace Nucleus
   {
	template <> struct Disposer_Traits< Nitrogen::PlainIconHandle  > : Disposer_Traits< Nitrogen::Handle > {};
	template <> struct Disposer_Traits< Nitrogen::MaskedIconHandle > : Disposer_Traits< Nitrogen::Handle > {};
	template <> struct Disposer_Traits< Nitrogen::SmallIconHandle  > : Disposer_Traits< Nitrogen::Handle > {};
  }

namespace Nitrogen
  {
	// ResType 'cicn'
	using ::CIconHandle;
	
  }

namespace Nucleus
  {
	template <> struct Disposer< Nitrogen::CIconHandle > : public std::unary_function< Nitrogen::CIconHandle, void >
	{
		void operator()( Nitrogen::CIconHandle h ) const
		{
			//NUCLEUS_REQUIRE_ERRORS( Nitrogen::IconManager );
			
			::DisposeCIcon( h );
		}
	};
  }

namespace Nitrogen
  {
	
	Nucleus::Owned< CIconHandle > GetCIcon( ResID iconID );
	
	void PlotCIcon( const Rect& rect, CIconHandle icon );
	
	inline void DisposeCIcon( Nucleus::Owned< CIconHandle > )  {}
	
	PlainIconHandle GetIcon( ResID iconID );  // Returns a resource handle
	
	void PlotIcon( const Rect& rect, PlainIconHandle icon );
	
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
	
  }

namespace Nucleus
  {
	template <> struct Disposer< Nitrogen::IconSuiteRef > : public std::unary_function< Nitrogen::IconSuiteRef, void >,
	                                                        private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		// DisposeIconSuite() takes a Boolean argument that tells it whether to
		// dispose the individual icon data associated with the icon suite.
		// It will not dispose resource handles, regardless of the value passed.
		// I'm hard-coding this argument to true, as that seems the most useful to me.
		// If you need to pass false, use DisposeIconSuiteButNotData instead.
		
		enum { disposeData = true };
		
		void operator()( Nitrogen::IconSuiteRef i ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::IconManager );
			
			HandleDestructionOSStatus( ::DisposeIconSuite( i, disposeData ) );
		}
	};
  }

namespace Nitrogen
  {
	
	struct DisposeIconSuiteButNotData : public std::unary_function< IconSuiteRef, void >,
	                                    private DefaultDestructionOSStatusPolicy
	{
		enum { disposeData = false };
		
		void operator()( IconSuiteRef i ) const
		{
			NUCLEUS_REQUIRE_ERRORS( IconManager );
			
			HandleDestructionOSStatus( ::DisposeIconSuite( i, disposeData ) );
		}
	};
	
   using ::IconRef;
	
  }

namespace Nucleus
  {
   template <> struct Disposer< Nitrogen::IconRef >: public std::unary_function< Nitrogen::IconRef, void >,
												     private Nitrogen::DefaultDestructionOSStatusPolicy
     {
      void operator()( Nitrogen::IconRef i ) const
        {
         NUCLEUS_REQUIRE_ERRORS( Nitrogen::IconManager );
         
         HandleDestructionOSStatus( ::ReleaseIconRef( i ) );
        }
     };
  }

namespace Nitrogen
  {
	
	void PlotIconID( const Rect&        rect,
	                 IconAlignmentType  align,
	                 IconTransformType  transform,
	                 ResID              resID );
	
	Nucleus::Owned< IconSuiteRef > NewIconSuite();
	
	template < bool disposeData >  struct DisposeData_Traits;
	
	template <>  struct DisposeData_Traits< true >
	{
		typedef Nucleus::Disposer< IconSuiteRef > Disposer;
	};
	
	template <>  struct DisposeData_Traits< false >
	{
		typedef DisposeIconSuiteButNotData Disposer;
	};
	
	template < bool disposeData >
	Nucleus::Owned< IconSuiteRef, typename DisposeData_Traits< disposeData >::Disposer > NewIconSuite()
	{
		typedef typename DisposeData_Traits< disposeData >::Disposer Disposer;
		
		return Nucleus::Owned< IconSuiteRef, Disposer >::Seize( NewIconSuite().Release() );
	}
	
	void AddIconToSuite( Handle iconData, IconSuiteRef suite, ResType type );
	
	Handle GetIconFromSuite( IconSuiteRef suite, ResType type );
	
	// ForEachIconDo
	
	Nucleus::Owned< IconSuiteRef > GetIconSuite( ResID resID, IconSelectorValue selector );
	
	void DisposeIconSuite( Nucleus::Owned< IconSuiteRef                             > iconSuite );  // true
	void DisposeIconSuite( Nucleus::Owned< IconSuiteRef, DisposeIconSuiteButNotData > iconSuite );  // false
	
	void PlotIconSuite( const Rect&        rect,
	                    IconAlignmentType  align,
	                    IconTransformType  transform,
	                    IconSuiteRef       iconSuite );
	
	// MakeIconCache
	// LoadIconCache
	// PlotIconMethod
	// GetLabel
	// PtInIconID
	// PtInIconSuite
	// PtInIconMethod
	// RectInIconID
	// RectInIconSuite
	// RectInIconMethod
	// IconIDToRgn
	// IconSuiteToRgn
	// IconMethodToRgn
	// SetSuiteLabel
	// GetSuiteLabel
	// GetIconCacheData
	// SetIconCacheData
	// GetIconCacheProc
	// SetIconCacheProc
	
	void PlotIconHandle( const Rect&        area,
	                     IconAlignmentType  align,
	                     IconTransformType  transform,
	                     PlainIconHandle    icon );
	
	void PlotIconHandle( const Rect&        area,
	                     IconAlignmentType  align,
	                     IconTransformType  transform,
	                     MaskedIconHandle   icon );
	
	void PlotSICNHandle( const Rect&        area,
	                     IconAlignmentType  align,
	                     IconTransformType  transform,
	                     SmallIconHandle    theSICN );
	
	void PlotCIconHandle( const Rect&        rect,
	                      IconAlignmentType  align,
	                      IconTransformType  transform,
	                      CIconHandle        theCIcon );
	
	enum IconServicesUsageFlags
	{
		kIconServicesNormalUsageFlag = ::kIconServicesNormalUsageFlag,
		
		kIconServicesUsageFlags_Max = Nucleus::Enumeration_Traits< ::IconServicesUsageFlags >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( IconServicesUsageFlags )
	
	enum PlotIconRefFlags
	{
		kPlotIconRefFlags_Max = Nucleus::Enumeration_Traits< ::PlotIconRefFlags >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( PlotIconRefFlags )
	
	// ... Icon Families
	// ... Initialization and Termination
	// ... Conversions
	// ... Reference counting
	
   struct GetIconRefFromFile_Result
     {
      Nucleus::Owned<IconRef> theIconRef;
      IconLabel theLabel;
      
      GetIconRefFromFile_Result( Nucleus::Owned<IconRef> icon, IconLabel label )
        : theIconRef( icon ),
          theLabel( label )
        {}
      
      GetIconRefFromFile_Result( Nucleus::ResourceTransfer<GetIconRefFromFile_Result> r )
        : theIconRef( r->theIconRef ),
          theLabel( r->theLabel )
        {}
      
      operator Nucleus::ResourceTransfer<GetIconRefFromFile_Result>()
        {
         return Nucleus::ResourceTransfer<GetIconRefFromFile_Result>( *this );
        }
      
      operator Nucleus::Owned<IconRef>()            { return theIconRef; }
      operator IconRef() const             { return theIconRef; }
     };

   GetIconRefFromFile_Result GetIconRefFromFile( const FSSpec& theFile );

   Nucleus::Owned<IconRef> GetIconRef( FSVolumeRefNum vRefNum, OSType creator, OSType iconType );

   Nucleus::Owned<IconRef> GetIconRef( OSType creator, OSType iconType );

   Nucleus::Owned<IconRef> GetIconRef( OSType iconType );

	Nucleus::Owned<IconRef> GetIconRefFromFolder( FSVolumeRefNum vRefNum,
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
	
	// RegisterIconRefFromIconFamily
	// RegisterIconRefFromResource
	
   Nucleus::Owned<IconRef> RegisterIconRefFromFSRef( OSType creator, OSType iconType, const FSRef& iconFile );
	
	// UnregisterIconRef
	// UpdateIconRef
	// OverrideIconRefFromResource
	// OverrideIconRef
	// RemoveIconRefOverride
	
	// ... Creating composite IconRef
	// ... Using IconRef
	// ... Flushing IconRef data
	// ... Controling custom icons
	
	Nucleus::Owned< IconRef > RegisterIconRefFromIconFile( OSType creator,
	                                              OSType iconType,
	                                              const FSSpec& iconFile );

   inline Nucleus::Owned<IconRef> RegisterIconRefFromIconFile( const FSSpec& iconFile )
     {
      return RegisterIconRefFromIconFile( kSystemIconsCreator, OSType( 0 ), iconFile );
     }
	
	// ReadIconFile
	// ReadIconFromFSRef
	// WriteIconFile
	
}

#endif

