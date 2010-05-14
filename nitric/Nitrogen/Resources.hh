// Nitrogen/Resources.hh
// ---------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_RESOURCES_HH
#define NITROGEN_RESOURCES_HH

#ifndef __RESOURCES__
#include <Resources.h>
#endif

// iota
#include "iota/string_traits.hh"

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/flag_ops.hh"

#ifndef NITROGEN_FILES_HH
#include "Nitrogen/Files.hh"
#endif
#ifndef NITROGEN_MACMEMORY_HH
#include "Nitrogen/MacMemory.hh"
#endif
#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( ResourceManager );
	
	
	enum ResFileRefNum
	{
		kResFileNotOpened = ::kResFileNotOpened,
		kSystemResFile    = ::kSystemResFile,
		
		kResFileRefNum_Max = nucleus::enumeration_traits< ::ResFileRefNum >::max
	};
	
	enum ResID
	{
		kResID_Max = nucleus::enumeration_traits< ::ResID >::max
	};
	
	typedef ResID ResourceID;
	
	enum ResAttributes
	{
		resSysHeap   = ::resSysHeap,
		resPurgeable = ::resPurgeable,
		resLocked    = ::resLocked,
		resProtected = ::resProtected,
		resPreload   = ::resPreload,
		resChanged   = ::resChanged,
		
		kResAttributes_Max = nucleus::enumeration_traits< ::ResAttributes >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( ResAttributes )
	
	enum ResFileAttributes
	{
		mapReadOnly = ::mapReadOnly,
		mapCompact  = ::mapCompact,
		mapChanged  = ::mapChanged,
		
		kResFileAttributes_Max = nucleus::enumeration_traits< ::ResFileAttributes >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( ResFileAttributes )
	
	// ResourceReleaser is not used as a disposer for owned<> because resource 
	// handles are owned by the Resource Manager, not the application.
	// But here it is anyway for completeness, in case someone finds it useful.
	
	struct ResourceReleaser : public std::unary_function< Handle, void >
	{
		void operator()( Handle r ) const
		{
			NUCLEUS_REQUIRE_ERRORS( ResourceManager );
			
			::ReleaseResource( r );
			
			HandleDestructionOSStatus( ::ResError() );
		}
	};
	
  }

namespace nucleus
  {
	template <>
	struct disposer< Nitrogen::ResFileRefNum > : public std::unary_function< Nitrogen::ResFileRefNum, void >
	{
		void operator()( Nitrogen::ResFileRefNum resFile ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::ResourceManager );
			
			::CloseResFile( resFile );
			
			::Nitrogen::HandleDestructionOSStatus( ::ResError() );
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
	
	void CloseResFile( nucleus::owned< ResFileRefNum > resFileRefNum );
	
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
	
	template < class String >
	inline Handle GetNamedResource( ResType type, const String& name )
	{
		return GetNamedResource( type, Str255( iota::get_string_data( name ),
		                                       iota::get_string_size( name ) ) );
	}
	
	template < class String >
	inline Handle Get1NamedResource( ResType type, const String& name )
	{
		return Get1NamedResource( type, Str255( iota::get_string_data( name ),
		                                        iota::get_string_size( name ) ) );
	}
	
	void MacLoadResource( Handle r );
	
	void ReleaseResource( Handle r );
	
	// nucleus::owned< Handle > is never a resource handle, so any attempt to
	// detach or remove one is invalid.
	// This function is not defined, and calling it results in a link error.
	// It's here so the real DetachResource() (below) doesn't get called instead.
	
	void DetachResource( nucleus::owned< Handle > h );
	
	nucleus::owned< Handle >  DetachResource( Handle r );
	
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
	
	Handle AddResource( nucleus::owned< Handle >  h,
	                    ResType                   type,
	                    ResID                     resID,
	                    ConstStr255Param          name );
	
	Handle AddResource( nucleus::owned< Handle > h, const GetResInfo_Result& resInfo );
	
	std::size_t GetResourceSizeOnDisk( Handle r );
	
	std::size_t GetMaxResourceSize( Handle r );
	
	// RsrcMapEntry
	
	void SetResAttrs( Handle r, ResAttributes attrs );
	
	void ChangedResource( Handle r );
	
	// Invalid; not defined.  See notes for DetachResource() above.
	void RemoveResource( nucleus::owned< Handle > h );
	
	nucleus::owned< Handle > RemoveResource( Handle r );
	
	void UpdateResFile( ResFileRefNum refNum );
	
	void WriteResource( Handle r );
	
	void SetResPurge( bool install );
	
	ResFileAttributes GetResFileAttrs( ResFileRefNum refNum );
	
	void SetResFileAttrs( ResFileRefNum refNum, ResFileAttributes attrs );
	
	// OpenRFPerm     -- not implemented; use FSpOpenRF
	// RGetResource
	// HOpenResFile   -- not implemented; use FSpOpenResFile
	// HCreateResFile -- not implemented; use FSpCreateResFile
	
	nucleus::owned< ResFileRefNum > FSpOpenResFile( const FSSpec&  spec,
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
	
	nucleus::owned< ResFileRefNum > FSOpenResourceFile( const FSRef&    ref,
	                                                    UniCharCount    forkNameLength,
	                                                    const UniChar*  forkName,
	                                                    FSIOPermssn     permissions );
	
	nucleus::owned< ResFileRefNum > FSOpenResourceFile( const FSRef&      ref,
	                                                    const UniString&  forkName,
	                                                    FSIOPermssn       permissions );
	
	// FSCreateResFile
	// FSResourceFileAlreadyOpen
	// FSCreateResourceFile
	// FSOpenResourceFile
	
	class ResFile
	{
		public:
			typedef ResFileRefNum value_type;
			typedef ResFileRefNum param_type;
			
			value_type get() const                { return CurResFile();   }
			void set( param_type resFile ) const  { UseResFile( resFile ); }
	};
	
	
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
		typedef nucleus::owned< Handle >  Parameter;
		
		static Result MakeFromHandle( Handle h )  { return h; }
		
		static nucleus::owned< Handle > MakeIntoHandle( Parameter h )  { return h; }
	};
	
	template <>
	struct ResType_Traits< kTextResourceType >
	{
		typedef       nucleus::string   Result;
		typedef const nucleus::string&  Parameter;
		
		static Result MakeFromHandle( Handle h )
		{
			nucleus::mutable_string result;
			
			result.resize( GetHandleSize( h ) );
			
			std::copy( *h.Get(),
			           *h.Get() + result.size(),
			           result.begin() );
			
			return result;
		}
		
		static nucleus::owned< Handle > MakeIntoHandle( Parameter text )
		{
			nucleus::owned< Handle > result = NewHandle( text.size() );
			
			std::copy( text.begin(), text.end(), *result.get().Get() );
			
			return result;
		}
	};
	
	template < class Data >
	struct Handle_ResType_Traits
	{
		typedef                 Data**                                 Result;
		typedef nucleus::owned< Data**, nucleus::disposer< Handle > >  Parameter;
		
		static Result MakeFromHandle( Handle h )  { return Handle_Cast< Data >( h ); }
		
		static nucleus::owned< Handle > MakeIntoHandle( Parameter h )
		{
			return nucleus::owned< Handle >( h );
		}
	};
	
	template < class FinalType >
	struct POD_ResType_Traits
	{
		typedef       FinalType   Result;
		typedef const FinalType&  Parameter;
		
		static Result MakeFromHandle( Handle h )  { return **Handle_Cast< Result >( h ); }
		
		static nucleus::owned< Handle > MakeIntoHandle( Parameter pod )
		{
			nucleus::owned< Handle > result = NewHandle( sizeof (Result) );
			
			**result.get() = pod;
			
			return result;
		}
	};
	
	template < class FinalType, std::size_t (*SizeOf)( const FinalType& ) >
	struct VariableLengthPOD_ResType_Traits
	{
		typedef       FinalType   Result;
		typedef const FinalType&  Parameter;
		
		static Result MakeFromHandle( Handle h )  { return **Handle_Cast< Result >( h ); }
		
		static nucleus::owned< Handle > MakeIntoHandle( Parameter pod )
		{
			const std::size_t size = SizeOf( pod );
			
			nucleus::owned< Handle > result = NewHandle( size );
			
			const char* const begin = reinterpret_cast< const char* >( &pod );
			
			std::copy( begin, begin + size, *result.get() );
			
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
	inline nucleus::owned< Data**, nucleus::disposer< Handle > > DetachResource( Data** h )
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

