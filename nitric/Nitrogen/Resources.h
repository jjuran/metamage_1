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
#ifndef NUCLEUS_ID_H
#include "Nucleus/ID.h"
#endif
#ifndef NUCLEUS_FLAG_H
#include "Nucleus/Flag.h"
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
	
	
	typedef Nucleus::ID< class ResFileRefNum_Tag, ::ResFileRefNum >::Type ResFileRefNum;
	
	static const ResFileRefNum kResFileNotOpened = ResFileRefNum( ::kResFileNotOpened );
	
	typedef Nucleus::ID< class ResID_Tag, ::ResID >::Type ResID;
	typedef ResID ResourceID;
	
	typedef Nucleus::Flag< class ResAttributes_Tag, ::ResAttributes >::Type ResAttributes;
	
	NUCLEUS_DEFINE_FLAG_OPS( ResAttributes )
	
	typedef Nucleus::Flag< class ResFileAttributes_Tag, ::ResFileAttributes >::Type ResFileAttributes;
	
	NUCLEUS_DEFINE_FLAG_OPS( ResFileAttributes )
	
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
	inline T** CheckResource( T** r )
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
	                       OSType         creator   = OSType( 'RSED' ),
	                       OSType         type      = OSType( 'rsrc' ),
	                       ScriptCode     scriptTag = smSystemScript );
	
	inline void FSpCreateResFile( const FSSpec&         spec,
	                              const FileSignature&  signature,
	                              ScriptCode            scriptTag = smSystemScript )
	{
		FSpCreateResFile( spec, signature.creator, signature.type, scriptTag );
	}
	
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
	
	
	namespace Constants
	{
		
		static const ResType kTextResourceType = ResType( 'TEXT' );
		
	}
	
	using namespace Constants;
	
	
	#pragma mark -
	#pragma mark ¥ ResType_Traits ¥
	#pragma mark -
	
	template < ResType resType >
	struct ResType_Traits
	{
		typedef                 Handle    Result;
		typedef Nucleus::Owned< Handle >  Parameter;
		
		static Result MakeFromHandle( Handle h )  { return h; }
		
		static Nucleus::Owned< Handle > MakeIntoHandle( Parameter h )  { return h; }
	};
	
	template <>
	struct ResType_Traits< kTextResourceType >
	{
		typedef       std::string   Result;
		typedef const std::string&  Parameter;
		
		static Result MakeFromHandle( Handle h )
		{
			std::string result;
			result.resize( GetHandleSize( h ) );
			
			std::copy( *h.Get(),
			           *h.Get() + result.size(),
			           result.begin() );
			
			return result;
		}
		
		static Nucleus::Owned< Handle > MakeIntoHandle( Parameter text )
		{
			Nucleus::Owned< Handle > result = NewHandle( text.size() );
			
			std::copy( text.begin(), text.end(), *result.Get().Get() );
			
			return result;
		}
	};
	
	template < class Data >
	struct Handle_ResType_Traits
	{
		typedef                 Data**                                 Result;
		typedef Nucleus::Owned< Data**, Nucleus::Disposer< Handle > >  Parameter;
		
		static Result MakeFromHandle( Handle h )  { return Handle_Cast< Data >( h ); }
		
		static Nucleus::Owned< Handle > MakeIntoHandle( Parameter h )
		{
			return Nucleus::Owned< Handle >( h );
		}
	};
	
	template < class FinalType >
	struct POD_ResType_Traits
	{
		typedef       FinalType   Result;
		typedef const FinalType&  Parameter;
		
		static Result MakeFromHandle( Handle h )  { return **Handle_Cast< Result >( h ); }
		
		static Nucleus::Owned< Handle > MakeIntoHandle( Parameter pod )
		{
			Nucleus::Owned< Handle > result = NewHandle( sizeof (Result) );
			
			**result.Get() = pod;
			
			return result;
		}
	};
	
	template < class FinalType, std::size_t (*SizeOf)( const FinalType& ) >
	struct VariableLengthPOD_ResType_Traits
	{
		typedef       FinalType   Result;
		typedef const FinalType&  Parameter;
		
		static Result MakeFromHandle( Handle h )  { return **Handle_Cast< Result >( h ); }
		
		static Nucleus::Owned< Handle > MakeIntoHandle( Parameter pod )
		{
			const std::size_t size = SizeOf( pod );
			
			Nucleus::Owned< Handle > result = NewHandle( size );
			
			const char* const begin = reinterpret_cast< const char* >( &pod );
			
			std::copy( begin, begin + size, *result.Get() );
			
			return result;
		}
	};
	
	template <>
	struct ResType_Traits< kVersionResType > : VariableLengthPOD_ResType_Traits< VersRec, SizeOf_VersRec > {};
	
	
	template < ResType type >
	inline typename ResType_Traits< type >::Result GetIndResource( short index )
	{
		return ResType_Traits< type >::MakeFromHandle( GetIndResource( type, index ) );
	}
	
	template < ResType type >
	inline typename ResType_Traits< type >::Result Get1IndResource( short index )
	{
		return ResType_Traits< type >::MakeFromHandle( Get1IndResource( type, index ) );
	}
	
	template < ResType type >
	inline typename ResType_Traits< type >::Result GetResource( ResID resID )
	{
		return ResType_Traits< type >::MakeFromHandle( GetResource( type, resID ) );
	}
	
	template < ResType type >
	inline typename ResType_Traits< type >::Result Get1Resource( ResID resID )
	{
		return ResType_Traits< type >::MakeFromHandle( Get1Resource( type, resID ) );
	}
	
	template < class Data >
	inline Nucleus::Owned< Data**, Nucleus::Disposer< Handle > > DetachResource( Data** h )
	{
		return Handle_Cast< Data >( DetachResource( Handle( h ) ) );
	}
	
	template < ResType type >
	inline Handle AddResource( typename ResType_Traits< type >::Parameter  param,
	                           ResID                                       id,
	                           ConstStr255Param                            name = "\p" )
	{
		return AddResource( ResType_Traits< type >::MakeIntoHandle( param ), type, id, name );
	}
	
}

#endif

