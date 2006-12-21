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
#ifndef NUCLEUS_IDTYPE_H
#include "Nucleus/IDType.h"
#endif
#ifndef NUCLEUS_FLAGTYPE_H
#include "Nucleus/FlagType.h"
#endif
#ifndef NUCLEUS_PSEUDOREFERENCE_H
#include "Nucleus/Pseudoreference.h"
#endif

namespace Nitrogen
{
	
	class ResourceManagerErrorsRegistrationDependency
	{
		public:
			ResourceManagerErrorsRegistrationDependency();
	};
	
	class ResFileRefNum_Tag {};
	typedef Nucleus::IDType< ResFileRefNum_Tag, ::ResFileRefNum, kResFileNotOpened > ResFileRefNum;
	
	class ResID_Tag {};
	typedef Nucleus::IDType< ResID_Tag, ::ResID, 0 > ResID;
	typedef ResID ResourceID;
	
	class ResAttributes_Tag {};
	typedef Nucleus::FlagType< ResAttributes_Tag, ::ResAttributes, 0 > ResAttributes;
	
	class ResFileAttributes_Tag {};
	typedef Nucleus::FlagType< ResFileAttributes_Tag, ::ResFileAttributes, 0 > ResFileAttributes;
	
	// ResourceReleaser is not used as a Disposer for Owned because resource 
	// handles are owned by the Resource Manager, not the application.
	// But here it is anyway for completeness, in case someone finds it useful.
	
	struct ResourceReleaser : public std::unary_function< Handle, void >, 
	                          private DefaultDestructionOSStatusPolicy
	{
		void operator()( Handle r ) const
		{
			(void) ResourceManagerErrorsRegistrationDependency();
			::ReleaseResource( r );
			HandleDestructionOSStatus( ::ResError() );
		}
	};
	
  }

namespace Nucleus
  {
	template <>
	struct Disposer< Nitrogen::ResFileRefNum > : public std::unary_function< Nitrogen::ResFileRefNum, void >,
	                                             private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( Nitrogen::ResFileRefNum resFile ) const
		{
			(void) Nitrogen::ResourceManagerErrorsRegistrationDependency();
			::CloseResFile( resFile );
			HandleDestructionOSStatus( ::ResError() );
		}
	};
  }

namespace Nitrogen
  {
	
	Handle CheckResource( Handle r );
	
	template < class T >
	T** CheckResource( T** r )
	{
		return Handle_Cast< T >( CheckResource( Handle( r ) ) );
	}
	
	// InitResources
	// RsrcZoneInit
	
	void CloseResFile( Nucleus::Owned< ResFileRefNum > resFileRefNum );
	
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
	
	// Nucleus::Owned< Handle > is never a resource handle, so any attempt to
	// detach or remove one is invalid.
	// This function is not defined, and calling it results in a link error.
	// It's here so the real DetachResource() (below) doesn't get called instead.
	
	void DetachResource( Nucleus::Owned< Handle > h );
	
	Nucleus::Owned< Handle >  DetachResource( Handle r );
	
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
	
	Handle AddResource( Nucleus::Owned< Handle >  h,
	                    ResType                   type,
	                    ResID                     resID,
	                    ConstStr255Param          name );
	
	Handle AddResource( Nucleus::Owned< Handle > h, const GetResInfo_Result& resInfo );
	
	std::size_t GetResourceSizeOnDisk( Handle r );
	
	std::size_t GetMaxResourceSize( Handle r );
	
	// RsrcMapEntry
	
	void SetResAttrs( Handle r, ResAttributes attrs );
	
	void ChangedResource( Handle r );
	
	// Invalid; not defined.  See notes for DetachResource() above.
	void RemoveResource( Nucleus::Owned< Handle > h );
	
	Nucleus::Owned< Handle > RemoveResource( Handle r );
	
	void UpdateResFile( ResFileRefNum refNum );
	
	void WriteResource( Handle r );
	
	void SetResPurge( bool install );
	
	ResFileAttributes GetResFileAttrs( ResFileRefNum refNum );
	
	void SetResFileAttrs( ResFileRefNum refNum, ResFileAttributes attrs );
	
	// OpenRFPerm     -- not implemented; use FSpOpenRF
	// RGetResource
	// HOpenResFile   -- not implemented; use FSpOpenResFile
	// HCreateResFile -- not implemented; use FSpCreateResFile
	
	Nucleus::Owned< ResFileRefNum > FSpOpenResFile( const FSSpec&  spec,
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
	
	Nucleus::Owned< ResFileRefNum > FSOpenResourceFile( const FSRef&    ref,
	                                                    UniCharCount    forkNameLength,
	                                                    const UniChar*  forkName,
	                                                    FSIOPermssn     permissions );
	
	Nucleus::Owned< ResFileRefNum > FSOpenResourceFile( const FSRef&      ref,
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
	
	typedef Nucleus::Pseudoreference< ResFile_Details > ResFile;
	
}

#endif

