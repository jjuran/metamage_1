// Nitrogen/Icons.hh
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2009 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_ICONS_HH
#define NITROGEN_ICONS_HH

#ifndef __ICONS__
#include <Icons.h>
#endif

// iota
#include "iota/string_traits.hh"

// nucleus
#include "nucleus/resource_transfer.hh"

// Nitrogen
#include "Mac/Files/Types/FSDirID.hh"
#include "Mac/Files/Types/FSSharingFlags.hh"
#include "Mac/Files/Types/FSUserPrivileges.hh"
#include "Mac/Files/Types/FSVolumeRefNum.hh"
#include "Mac/Icons/Types/IconRef.hh"
#include "Mac/Toolbox/Types/OSType.hh"

#ifndef NITROGEN_RESOURCES_HH
#include "Nitrogen/Resources.hh"
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
		kIconAlignmentType_Max = nucleus::enumeration_traits< ::IconAlignmentType >::max
	};
	
	enum IconTransformType
	{
		kIconTransformType_Max = nucleus::enumeration_traits< ::IconTransformType >::max
	};
	
	enum IconSelectorValue
	{
		kIconSelectorValue_Max = nucleus::enumeration_traits< ::IconSelectorValue >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( IconAlignmentType )
	NUCLEUS_DEFINE_FLAG_OPS( IconTransformType )
	NUCLEUS_DEFINE_FLAG_OPS( IconSelectorValue )
	
	static const IconAlignmentType kAlignNone = IconAlignmentType( ::kAlignNone );
	
	static const IconTransformType kTransformNone = IconTransformType( ::kTransformNone );
	
	static const IconSelectorValue kSelectorAllAvailableData = IconSelectorValue( ::kSelectorAllAvailableData );
	
	
	enum IconLabel
	{
		kIconLabel_Max = nucleus::enumeration_traits< SInt16 >::max
	};
	
	// ...
	
	static const Mac::OSType kSystemIconsCreator = Mac::OSType( ::kSystemIconsCreator );
	
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

namespace nucleus
   {
	template <> struct disposer_traits< Nitrogen::PlainIconHandle  > : disposer_traits< Nitrogen::Handle > {};
	template <> struct disposer_traits< Nitrogen::MaskedIconHandle > : disposer_traits< Nitrogen::Handle > {};
	template <> struct disposer_traits< Nitrogen::SmallIconHandle  > : disposer_traits< Nitrogen::Handle > {};
  }

namespace Nitrogen
  {
	// ResType 'cicn'
	using ::CIconHandle;
	
  }

namespace nucleus
  {
	template <> struct disposer< CIconHandle >
	{
		typedef CIconHandle  argument_type;
		typedef void         result_type;
		
		void operator()( CIconHandle h ) const
		{
			//NUCLEUS_REQUIRE_ERRORS( Nitrogen::IconManager );
			
			::DisposeCIcon( h );
		}
	};
  }

namespace Nitrogen
  {
	
	nucleus::owned< CIconHandle > GetCIcon( ResID iconID );
	
	void PlotCIcon( const Rect& rect, CIconHandle icon );
	
	inline void DisposeCIcon( nucleus::owned< CIconHandle > )  {}
	
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

namespace nucleus
  {
	template <> struct disposer< Nitrogen::IconSuiteRef >
	{
		typedef Nitrogen::IconSuiteRef  argument_type;
		typedef void                    result_type;
		
		// DisposeIconSuite() takes a Boolean argument that tells it whether to
		// dispose the individual icon data associated with the icon suite.
		// It will not dispose resource handles, regardless of the value passed.
		// I'm hard-coding this argument to true, as that seems the most useful to me.
		// If you need to pass false, use DisposeIconSuiteButNotData instead.
		
		enum { disposeData = true };
		
		void operator()( Nitrogen::IconSuiteRef i ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::IconManager );
			
			::Nitrogen::HandleDestructionOSStatus( ::DisposeIconSuite( i, disposeData ) );
		}
	};
  }

namespace Nitrogen
  {
	
	struct DisposeIconSuiteButNotData
	{
		typedef IconSuiteRef  argument_type;
		typedef void          result_type;
		
		enum { disposeData = false };
		
		void operator()( IconSuiteRef i ) const
		{
			NUCLEUS_REQUIRE_ERRORS( IconManager );
			
			HandleDestructionOSStatus( ::DisposeIconSuite( i, disposeData ) );
		}
	};
	
   using ::IconRef;
	
  }

namespace Nitrogen
  {
	
	void PlotIconID( const Rect&        rect,
	                 IconAlignmentType  align,
	                 IconTransformType  transform,
	                 ResID              resID );
	
	nucleus::owned< IconSuiteRef > NewIconSuite();
	
	template < bool disposeData >  struct DisposeData_Traits;
	
	template <>  struct DisposeData_Traits< true >
	{
		typedef nucleus::disposer< IconSuiteRef > Disposer;
	};
	
	template <>  struct DisposeData_Traits< false >
	{
		typedef DisposeIconSuiteButNotData Disposer;
	};
	
	template < bool disposeData >
	nucleus::owned< IconSuiteRef, typename DisposeData_Traits< disposeData >::Disposer > NewIconSuite()
	{
		typedef typename DisposeData_Traits< disposeData >::Disposer Disposer;
		
		return nucleus::owned< IconSuiteRef, Disposer >::seize( NewIconSuite().release() );
	}
	
	void AddIconToSuite( Handle iconData, IconSuiteRef suite, ResType type );
	
	Handle GetIconFromSuite( IconSuiteRef suite, ResType type );
	
	// ForEachIconDo
	
	nucleus::owned< IconSuiteRef > GetIconSuite( ResID resID, IconSelectorValue selector );
	
	void DisposeIconSuite( nucleus::owned< IconSuiteRef                             > iconSuite );  // true
	void DisposeIconSuite( nucleus::owned< IconSuiteRef, DisposeIconSuiteButNotData > iconSuite );  // false
	
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
		
		kIconServicesUsageFlags_Max = nucleus::enumeration_traits< ::IconServicesUsageFlags >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( IconServicesUsageFlags )
	
	enum PlotIconRefFlags
	{
		kPlotIconRefFlags_Max = nucleus::enumeration_traits< ::PlotIconRefFlags >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( PlotIconRefFlags )
	
	// ... Icon Families
	// ... Initialization and Termination
	// ... Conversions
	// ... Reference counting
	
   struct GetIconRefFromFile_Result
     {
      nucleus::owned<IconRef> theIconRef;
      IconLabel theLabel;
      
      GetIconRefFromFile_Result( nucleus::owned<IconRef> icon, IconLabel label )
        : theIconRef( icon ),
          theLabel( label )
        {}
      
      GetIconRefFromFile_Result( nucleus::resource_transfer< GetIconRefFromFile_Result > r )
        : theIconRef( r->theIconRef ),
          theLabel( r->theLabel )
        {}
      
      operator nucleus::resource_transfer< GetIconRefFromFile_Result >()
        {
         return nucleus::resource_transfer< GetIconRefFromFile_Result >( *this );
        }
      
      operator nucleus::owned<IconRef>()            { return theIconRef; }
      operator IconRef() const             { return theIconRef; }
     };

   GetIconRefFromFile_Result GetIconRefFromFile( const FSSpec& theFile );

	nucleus::owned< IconRef > GetIconRef( Mac::FSVolumeRefNum  vRefNum,
	                                      Mac::OSType          creator,
	                                      Mac::OSType          iconType );
	
	nucleus::owned< IconRef > GetIconRef( Mac::OSType creator, Mac::OSType iconType );
	
	nucleus::owned< IconRef > GetIconRef( Mac::OSType iconType );
	
	nucleus::owned<IconRef> GetIconRefFromFolder( Mac::FSVolumeRefNum      vRefNum,
	                                              Mac::FSDirID             parentFolderID,
	                                              Mac::FSDirID             folderID,
	                                              Mac::FSIOFileAttributes  attributes,
	                                              Mac::FSUserPrivileges    accessPrivileges );

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

   template < class UniString >
   inline GetIconRefFromFileInfo_Result GetIconRefFromFileInfo( const FSRef&           inRef,
                                                                const UniString&       inFileName,
                                                                FSCatalogInfoBitmap    inWhichInfo,
                                                                const FSCatalogInfo&   inCatalogInfo,
                                                                IconServicesUsageFlags inUsageFlags = kIconServicesNormalUsageFlag )
     {
      return GetIconRefFromFileInfo( inRef,
                                     iota::get_string_size( inFileName ),
                                     iota::get_string_data( inFileName ),
                                     inWhichInfo,
                                     inCatalogInfo,
                                     inUsageFlags );
     }

   template < class UniString >
   inline GetIconRefFromFileInfo_Result GetIconRefFromFileInfo( const FSRef&           inRef,
                                                                const UniString&       inFileName,
                                                                IconServicesUsageFlags inUsageFlags = kIconServicesNormalUsageFlag )
     {
      return GetIconRefFromFileInfo( inRef,
                                     iota::get_string_size( inFileName ),
                                     iota::get_string_data( inFileName ),
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
	
   nucleus::owned<IconRef> RegisterIconRefFromFSRef( Mac::OSType creator, Mac::OSType iconType, const FSRef& iconFile );
	
	// UnregisterIconRef
	// UpdateIconRef
	// OverrideIconRefFromResource
	// OverrideIconRef
	// RemoveIconRefOverride
	
	// ... Creating composite IconRef
	// ... Using IconRef
	// ... Flushing IconRef data
	// ... Controling custom icons
	
	nucleus::owned< IconRef > RegisterIconRefFromIconFile( Mac::OSType    creator,
	                                                       Mac::OSType    iconType,
	                                                       const FSSpec&  iconFile );

   inline nucleus::owned<IconRef> RegisterIconRefFromIconFile( const FSSpec& iconFile )
     {
      return RegisterIconRefFromIconFile( kSystemIconsCreator, Mac::OSType( 0 ), iconFile );
     }
	
	// ReadIconFile
	// ReadIconFromFSRef
	// WriteIconFile
	
}

#endif

