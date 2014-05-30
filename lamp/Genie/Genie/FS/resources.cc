/*
	resources.cc
	------------
*/

#include "Genie/FS/resources.hh"

// Standard C
#include <ctype.h>

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

// gear
#include "gear/hexidecimal.hh"

// plus
#include "plus/hexidecimal.hh"
#include "plus/mac_utf8.hh"
#include "plus/var_string.hh"

// MacScribe
#include "quad/quad_name.hh"

// nucleus
#include "nucleus/shared.hh"

// Nitrogen
#ifndef MAC_TOOLBOX_TYPES_OSSTATUS_HH
#include "Mac/Toolbox/Types/OSStatus.hh"
#endif

#include "Nitrogen/Resources.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/primitives/get_file.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/dir_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/FS/property.hh"
#include "Genie/FS/utf8_text_property.hh"
#include "Genie/IO/Handle.hh"
#include "Genie/Utilities/RdWr_OpenResFile_Scope.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	using MacScribe::make_quad_name;
	using MacScribe::parse_quad_name;
	
	
	struct ResSpec
	{
		Mac::ResType  type;
		Mac::ResID    id;
	};
	
	class ResLoad_false_scope
	{
		private:
			// non-copyable
			ResLoad_false_scope           ( const ResLoad_false_scope& );
			ResLoad_false_scope& operator=( const ResLoad_false_scope& );
		
		public:
			ResLoad_false_scope()
			{
				::SetResLoad( false );
			}
			
			~ResLoad_false_scope()
			{
				::SetResLoad( true );
			}
	};
	
	
	void iterate_resources( const FSSpec& file, vfs::dir_contents& cache )
	{
		n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( file, Mac::fsRdPerm );
		
		const short n_types = N::Count1Types();
		
		for ( short i = 1;  i <= n_types;  ++i )
		{
			const N::ResType type = N::Get1IndType( i );
			
			const short n_rsrcs = N::Count1Resources( type );
			
			for ( short j = 1;  j <= n_rsrcs;  ++j )
			{
				const N::Handle r = N::Get1IndResource( type, j );
				
				const N::GetResInfo_Result info = N::GetResInfo( r );
				
				plus::var_string name = plus::encode_16_bit_hex( info.id );
				
				name += '.';
				
				name += make_quad_name( type );
				
				const vfs::dir_entry node( info.id, name );
				
				cache.push_back( node );
			}
		}
	}
	
	static ResSpec GetResSpec_from_name( const plus::string& name )
	{
		const plus::string& mac_name = plus::mac_from_utf8( name );
		
		const char* begin = mac_name.data();
		const char* end   = mac_name.size() + begin;
		
		// In the event of a short name, isxdigit( '\0' ) will return false
		
		const bool has_id =      isxdigit( begin[0] )
		                     &&  isxdigit( begin[1] )
		                     &&  isxdigit( begin[2] )
		                     &&  isxdigit( begin[3] );
		
		if ( !has_id )
		{
			p7::throw_errno( ENOENT );
		}
		
		const short id = gear::decode_16_bit_hex( begin );
		
		begin += 4;
		
		if ( *begin++ != '.' )
		{
			p7::throw_errno( ENOENT );
		}
		
		const ::OSType type = parse_quad_name( begin, end - begin );
		
		const ResSpec result = { Mac::ResType( type ), Mac::ResID( id ) };
		
		return result;
	}
	
	
	struct resource_name : readwrite_property
	{
		static void get( plus::var_string& result, const vfs::node* that, bool binary )
		{
			const vfs::node* res_file = that->owner();
			
			const FSSpec& fileSpec = *(FSSpec*) res_file->extra();
			
			n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( fileSpec, Mac::fsRdPerm );
			
			const ResSpec resSpec = GetResSpec_from_name( that->name() );
			
			const N::Handle r = (ResLoad_false_scope(),
			                     N::Get1Resource( resSpec.type, resSpec.id ));
			
			const N::GetResInfo_Result resInfo = N::GetResInfo( r );
			
			::ReleaseResource( r );
			
			result.assign( resInfo.name );
		}
		
		static void set( const vfs::node* that, const char* begin, const char* end, bool binary )
		{
			const size_t length = end - begin;
			
			if ( length > 255 )
			{
				p7::throw_errno( ENAMETOOLONG );
			}
			
			Str255 name;
			
			name[ 0 ] = length;
			
			memcpy( name + 1, begin, length );
			
			const vfs::node* res_file = that->owner();
			
			const FSSpec& fileSpec = *(FSSpec*) res_file->extra();
			
			RdWr_OpenResFile_Scope openResFile( fileSpec );
			
			const ResSpec resSpec = GetResSpec_from_name( that->name() );
			
			const N::Handle r = (ResLoad_false_scope(),
			                     N::Get1Resource( resSpec.type, resSpec.id ));
			
			N::SetResInfo( r, resSpec.id, name );
			
			::ReleaseResource( r );
		}
	};
	
	
	class Rsrc_IOHandle : public Handle_IOHandle
	{
		private:
			FSSpec itsFileSpec;
		
		private:
			void FlushResource();
		
		private:
			// non-copyable
			Rsrc_IOHandle           ( const Rsrc_IOHandle& );
			Rsrc_IOHandle& operator=( const Rsrc_IOHandle& );
		
		public:
			Rsrc_IOHandle( const vfs::node&               file,
			               int                            flags,
			               const n::shared< N::Handle >&  h,
			               const FSSpec&                  resFile )
			:
				Handle_IOHandle( file, flags, h ),
				itsFileSpec( resFile )
			{
			}
			
			~Rsrc_IOHandle();
			
			void Synchronize( bool metadata );
	};
	
	Rsrc_IOHandle::~Rsrc_IOHandle()
	{
		try
		{
			FlushResource();
		}
		catch ( ... )
		{
		}
	}
	
	static N::Handle GetOrAddResource( const ResSpec& resSpec )
	{
		try
		{
			return N::Get1Resource( resSpec.type, resSpec.id );
		}
		catch ( const Mac::OSStatus& err )
		{
			if ( err != resNotFound )
			{
				throw;
			}
			
			return N::AddResource( N::NewHandle( 0 ), resSpec.type, resSpec.id, NULL );
		}
	}
	
	void Rsrc_IOHandle::FlushResource()
	{
		FSTreePtr file = get_file( *this );
		
		const ResSpec resSpec = GetResSpec_from_name( file->name() );
		
		RdWr_OpenResFile_Scope openResFile( itsFileSpec );
		
		const N::Handle r = GetOrAddResource( resSpec );
		
		const size_t size = GetEOF();
		
		N::SetHandleSize( r, size );
		
		Positioned_Read( *r.Get(), size, 0 );
		
		N::ChangedResource( r );
		N::WriteResource  ( r );
	}
	
	void Rsrc_IOHandle::Synchronize( bool metadata )
	{
		FlushResource();
		
		metadata = true;  // until we implement data-only flush
		
		if ( metadata )
		{
			// Just flush the whole volume, since we can't be more specific.
			Mac::ThrowOSStatus( ::FlushVol( NULL, itsFileSpec.vRefNum ) );
		}
		else
		{
			// Call PBFlushFile(), or high-level wrapper
		}
	}
	
	
	static vfs::filehandle_ptr unrsrc_file_open( const FSTree* that, int flags, mode_t mode );
	
	static const data_method_set unrsrc_file_data_methods =
	{
		&unrsrc_file_open
	};
	
	static const node_method_set unrsrc_file_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&unrsrc_file_data_methods
	};
	
	
	static void rsrc_file_remove( const FSTree* that );
	
	static void rsrc_file_rename( const vfs::node* that, const vfs::node* destination );
	
	static vfs::filehandle_ptr rsrc_file_open( const FSTree* that, int flags, mode_t mode );
	
	static off_t rsrc_file_geteof( const FSTree* that );
	
	static vfs::node_ptr rsrc_file_lookup( const vfs::node*     that,
	                                       const plus::string&  name,
	                                       const vfs::node*     parent )
	{
		bool binary = false;
		bool mac    = false;
		
		const char* p = name.c_str();
		
		if ( *p == '.' )
		{
			++p;
			
			if ( *p == '~' )
			{
				binary = true;
				
				++p;
			}
			
			if ( memcmp( p, "mac-", sizeof "mac-" - 1) == 0 )
			{
				mac = true;
				
				p += sizeof "mac-" - 1;
			}
			else if ( !binary )
			{
				p = "";  // Don't match on ".name"
			}
		}
		
		if ( memcmp( p, "name", sizeof "name" ) != 0 )
		{
			p7::throw_errno( ENOENT );
		}
		
		typedef property_params_factory<                     resource_name   > mac_factory;
		typedef property_params_factory< utf8_text_property< resource_name > > utf8_factory;
		
		const property_params& params = mac ? mac_factory ::value
		                                    : utf8_factory::value;
		
		return new_property( that, name, &params );
	}
	
	static const data_method_set rsrc_file_data_methods =
	{
		&rsrc_file_open,
		&rsrc_file_geteof
	};
	
	static const dir_method_set rsrc_file_dirmethods =
	{
		&rsrc_file_lookup
	};
	
	static const node_method_set rsrc_file_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		&rsrc_file_remove,
		&rsrc_file_rename,
		&rsrc_file_data_methods,
		NULL,
		&rsrc_file_dirmethods
	};
	
	
	static bool has_resource( const FSSpec& file, const ResSpec& resSpec )
	{
		n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( file, Mac::fsRdPerm );
		
		return ::Get1Resource( resSpec.type, resSpec.id ) != NULL;
	}
	
	static void rsrc_file_remove( const FSTree* that )
	{
		const FSSpec& fileSpec = *(FSSpec*) that->extra();
		
		RdWr_OpenResFile_Scope openResFile( fileSpec );
		
		const ResSpec resSpec = GetResSpec_from_name( that->name() );
		
		const N::Handle r = N::Get1Resource( resSpec.type, resSpec.id );
		
		(void) N::RemoveResource( r );
	}
	
	static void rsrc_file_rename( const vfs::node* that, const vfs::node* destination )
	{
		if ( destination->owner() != that->owner() )
		{
			p7::throw_errno( EXDEV );
		}
		
		const plus::string& new_name = destination->name();
		
		const ResSpec old_resSpec = GetResSpec_from_name( that->name() );
		
		const ResSpec new_resSpec = GetResSpec_from_name( new_name );
		
		if ( old_resSpec.type != new_resSpec.type )
		{
			p7::throw_errno( EXDEV );
		}
		
		const FSSpec& fileSpec = *(FSSpec*) that->extra();
		
		RdWr_OpenResFile_Scope openResFile( fileSpec );
		
		::SetResLoad( false );
		
		if ( const Handle r = ::Get1Resource( new_resSpec.type, new_resSpec.id ) )
		{
			::RemoveResource( r );
		}
		
		::SetResLoad( true );
		
		const N::Handle r = N::Get1Resource( old_resSpec.type, old_resSpec.id );
		
		const N::GetResInfo_Result resInfo = N::GetResInfo( r );
		
		N::SetResInfo( r, new_resSpec.id, resInfo.name );
	}
	
	static off_t rsrc_file_geteof( const FSTree* that )
	{
		const FSSpec& fileSpec = *(FSSpec*) that->extra();
		
		n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( fileSpec, Mac::fsRdPerm );
		
		const ResSpec resSpec = GetResSpec_from_name( that->name() );
		
		const N::Handle r = N::Get1Resource( resSpec.type, resSpec.id );
		
		return N::GetHandleSize( r );
	}
	
	static vfs::filehandle_ptr unrsrc_file_open( const FSTree* that, int flags, mode_t mode )
	{
		const bool writing = flags + (1 - O_RDONLY) & 2;
		
		const bool creating = flags & O_CREAT;
		
		if ( !creating )
		{
			p7::throw_errno( ENOENT );
		}
		
		const FSSpec& fileSpec = *(FSSpec*) that->extra();
		
		{
			RdWr_OpenResFile_Scope openResFile( fileSpec );
			
			const ResSpec resSpec = GetResSpec_from_name( that->name() );
			
			(void) N::AddResource( N::NewHandle( 0 ), resSpec.type, resSpec.id, NULL );
		}
		
		n::owned< N::Handle > h = N::NewHandle( 0 );
		
		// that refers to an unrsrc; make a new one that's a live rsrc
		
		FSTreePtr new_node = Get_RsrcFile_FSTree( that->owner(),
		                                          that->name(),
		                                          fileSpec );
		
		that = new_node.get();
		
		vfs::filehandle* result = writing ? new Rsrc_IOHandle  ( *that, flags, h, fileSpec )
		                                  : new Handle_IOHandle( *that, flags, h );
		
		return result;
	}
	
	static vfs::filehandle_ptr rsrc_file_open( const FSTree* that, int flags, mode_t mode )
	{
		const bool writing = flags + (1 - O_RDONLY) & 2;
		
		const FSSpec& fileSpec = *(FSSpec*) that->extra();
		
		n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( fileSpec, Mac::fsRdPerm );
		
		const ResSpec resSpec = GetResSpec_from_name( that->name() );
		
		const N::Handle r = N::Get1Resource( resSpec.type, resSpec.id );
		
		n::owned< N::Handle > h = N::DetachResource( r );
		
		vfs::filehandle* result = writing ? new Rsrc_IOHandle  ( *that, flags, h, fileSpec )
		                                  : new Handle_IOHandle( *that, flags, h );
		
		return result;
	}
	
	FSTreePtr Get_RsrcFile_FSTree( const FSTree*        parent,
	                               const plus::string&  name,
	                               const FSSpec&        file )
	{
		const ResSpec resSpec = GetResSpec_from_name( name );
		
		const bool exists = has_resource( file, resSpec );
		
		// FIXME:  Check perms
		const mode_t mode = exists ? S_IFREG | 0400 : 0;
		
		const node_method_set* methods = exists ? &rsrc_file_methods
		                                        : &unrsrc_file_methods;
		
		FSTree* result = new FSTree( parent, name, mode, methods, sizeof (FSSpec) );
		
		*(FSSpec*) result->extra() = file;
		
		return result;
	}
}

