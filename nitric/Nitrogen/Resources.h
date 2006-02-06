// Resources.h

#ifndef NITROGEN_RESOURCES_H
#define NITROGEN_RESOURCES_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __RESOURCES__
#include FRAMEWORK_HEADER(CarbonCore,Resources.h)
#endif
#ifndef NITROGEN_FILES_H
#include "Nitrogen/Files.h"
#endif
#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_IDTYPE_H
#include "Nitrogen/IDType.h"
#endif
#ifndef NITROGEN_FLAGTYPE_H
#include "Nitrogen/FlagType.h"
#endif
#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif
#ifndef NITROGEN_PSEUDOREFERENCE_H
#include "Nitrogen/Pseudoreference.h"
#endif

namespace Nitrogen
{
	
	void RegisterResourceManagerErrors();
	
	class ResFileRefNum_Tag {};
	typedef IDType< ResFileRefNum_Tag, ::ResFileRefNum, kResFileNotOpened > ResFileRefNum;
	
	class ResID_Tag {};
	typedef IDType< ResID_Tag, ::ResID, 0 > ResID;
	typedef ResID ResourceID;
	
	class ResAttributes_Tag {};
	typedef FlagType< ResAttributes_Tag, ::ResAttributes, 0 > ResAttributes;
	
	class ResFileAttributes_Tag {};
	typedef FlagType< ResFileAttributes_Tag, ::ResFileAttributes, 0 > ResFileAttributes;
	
	// ResourceReleaser is not used as a Disposer for Owned because resource 
	// handles are owned by the Resource Manager, not the application.
	// But here it is anyway for completeness, in case someone finds it useful.
	
	struct ResourceReleaser : public std::unary_function< Handle, void >, 
	                          private DefaultDestructionOSStatusPolicy
	{
		void operator()( Handle r ) const
		{
			OnlyOnce< RegisterResourceManagerErrors >();
			::ReleaseResource( r );
			HandleDestructionOSStatus( ::ResError() );
		}
	};
	
	template <>
	struct Disposer< ResFileRefNum > : public std::unary_function< ResFileRefNum, void >,
	                                   private DefaultDestructionOSStatusPolicy
	{
		void operator()( ResFileRefNum resFile ) const
		{
			OnlyOnce< RegisterResourceManagerErrors >();
			::CloseResFile( resFile );
			HandleDestructionOSStatus( ::ResError() );
		}
	};
	
	Handle CheckResource( Handle r );
	
	template < class T >
	T** CheckResource( T** r )
	{
		return Handle_Cast< T >( CheckResource( Handle( r ) ) );
	}
	
	// InitResources
	// RsrcZoneInit
	
	void CloseResFile( Owned< ResFileRefNum > resFileRefNum );
	
	void ResError();
	
	ResFileRefNum CurResFile();
	
	ResFileRefNum HomeResFile( Handle r );
	
	// CreateResFile -- not implemented; use FSpCreateResFile
	// OpenResFile   -- not implemented; use FSpOpenResFile
	
	void UseResFile( ResFileRefNum resFileRefNum );
	
	short CountTypes();
	
	short Count1Types();
	
	ResType GetIndType( short index );
	
	ResType Get1IndType( short index );
	
	// SetResLoad
	
	short CountResources ( ResType type );
	short Count1Resources( ResType type );
	
	Handle GetIndResource ( ResType type, short index );
	Handle Get1IndResource( ResType type, short index );
	
	Handle GetResource ( ResType type, ResID resID );
	Handle Get1Resource( ResType type, ResID resID );
	
	Handle GetNamedResource ( ResType type, ConstStr255Param name );
	Handle Get1NamedResource( ResType type, ConstStr255Param name );
	
	inline Handle GetNamedResource( ResType type, const std::string& name )
	{
		return GetNamedResource( type, Str255( name ) );
	}
	
	inline Handle Get1NamedResource( ResType type, const std::string& name )
	{
		return Get1NamedResource( type, Str255( name ) );
	}
	
	void MacLoadResource( Handle r );
	
	void ReleaseResource( Handle r );
	
	void DetachResource( Owned< Handle > h );  // invalid, not defined
	Owned< Handle >  DetachResource( Handle r );
	
	ResID UniqueID ( ResType type );
	ResID Unique1ID( ResType type );
	
	ResAttributes GetResAttrs( Handle r );
	
	struct GetResInfo_Result
	{
		ResID   id;
		ResType type;
		::Str255  name;
	};
	
	GetResInfo_Result GetResInfo( Handle r );
	
	void SetResInfo( Handle r, ResID id, ConstStr255Param name );
	
	Handle AddResource( Owned< Handle >   h,
	                    ResType           type,
	                    ResID             resID,
	                    ConstStr255Param  name );
	
	Handle AddResource( Owned< Handle > h, const GetResInfo_Result& resInfo );
	
	std::size_t GetResourceSizeOnDisk( Handle r );
	
	std::size_t GetMaxResourceSize( Handle r );
	
	// RsrcMapEntry
	
	void SetResAttrs( Handle r, ResAttributes attrs );
	
	void ChangedResource( Handle r );
	
	void RemoveResource( Owned< Handle > h );  // invalid, not defined
	Owned< Handle > RemoveResource( Handle r );
	
	void UpdateResFile( ResFileRefNum refNum );
	
	void WriteResource( Handle r );
	
	void SetResPurge( bool install );
	
	ResFileAttributes GetResFileAttrs( ResFileRefNum refNum );
	
	void SetResFileAttrs( ResFileRefNum refNum, ResFileAttributes attrs );
	
	// OpenRFPerm     -- not implemented; use FSpOpenRF
	// RGetResource
	// HOpenResFile   -- not implemented; use FSpOpenResFile
	// HCreateResFile -- not implemented; use FSpCreateResFile
	
	Owned< ResFileRefNum > FSpOpenResFile( const FSSpec&  spec,
	                                       FSIOPermssn    permissions );
	
	void FSpCreateResFile( const FSSpec&  spec,
	                       OSType         creator   = 'RSED',
	                       OSType         type      = 'rsrc',
	                       ScriptCode     scriptTag = ScriptCode( smSystemScript ) );
	
	// ReadPartialResource
	// WritePartialResource
	// SetResourceSize
	// GetNextFOND
	// RegisterResourceEndianFilter
	// TempInsertROMMap
	// InsertResourceFile
	// DetachResourceFile
	// FSpResourceFileAlreadyOpen
	// FSpOpenOrphanResFile
	// GetTopResourceFile
	// GetNextResourceFile
	
	Owned< ResFileRefNum > FSOpenResourceFile( const FSRef&    ref,
	                                           UniCharCount    forkNameLength,
	                                           const UniChar*  forkName,
	                                           FSIOPermssn     permissions );
	
	Owned< ResFileRefNum > FSOpenResourceFile( const FSRef&      ref,
	                                           const UniString&  forkName,
	                                           FSIOPermssn       permissions );
	
	// FSCreateResFile
	// FSResourceFileAlreadyOpen
	// FSCreateResourceFile
	// FSOpenResourceFile
	
	class ResFile_Value
	{
		public:
			typedef ResFileRefNum Value;
			typedef ResFileRefNum GetResult;
			typedef ResFileRefNum SetParameter;
			
			static const bool hasSwap = false;
			
			GetResult Get() const                        { return CurResFile();   }
			void Set( SetParameter resFile ) const       { UseResFile( resFile ); }
	};
	
	typedef ResFile_Value ResFile_Details;
	
	typedef Pseudoreference< ResFile_Details > ResFile;
	
}

#endif

