/*
	resources.cc
	------------
*/

#include "Genie/FS/resources.hh"

// Standard C
#include <ctype.h>

// POSIX
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

// Genie
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/IO/Handle.hh"
#include "Genie/Utilities/RdWr_OpenResFile_Scope.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	using MacScribe::make_quad_name;
	using MacScribe::parse_quad_name;
	
	
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
	
	static N::GetResInfo_Result GetResInfo_from_name( const plus::string& name )
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
		
		const N::GetResInfo_Result result = { N::ResID( id ), N::ResType( type ) };
		
		return result;
	}
	
	
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
			Rsrc_IOHandle( const FSTreePtr&               file,
			               OpenFlags                      flags,
			               const n::shared< N::Handle >&  h,
			               const FSSpec&                  resFile )
			:
				Handle_IOHandle( file, flags, h ),
				itsFileSpec( resFile )
			{
			}
			
			~Rsrc_IOHandle();
			
			IOPtr Clone();
			
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
	
	static N::Handle GetOrAddResource( const N::GetResInfo_Result& resinfo )
	{
		try
		{
			return N::Get1Resource( resinfo.type, resinfo.id );
		}
		catch ( const Mac::OSStatus& err )
		{
			if ( err != resNotFound )
			{
				throw;
			}
			
			return N::AddResource( N::NewHandle( 0 ), resinfo );
		}
	}
	
	void Rsrc_IOHandle::FlushResource()
	{
		FSTreePtr file = GetFile();
		
		N::GetResInfo_Result resInfo = GetResInfo_from_name( file->name() );
		
		RdWr_OpenResFile_Scope openResFile( itsFileSpec );
		
		const N::Handle r = GetOrAddResource( resInfo );
		
		const size_t size = GetEOF();
		
		N::SetHandleSize( r, size );
		
		Positioned_Read( *r.Get(), size, 0 );
		
		N::ChangedResource( r );
		N::WriteResource  ( r );
	}
	
	IOPtr Rsrc_IOHandle::Clone()
	{
		return new Rsrc_IOHandle( GetFile(),
		                          GetFlags(),
		                          GetHandle(),
		                          itsFileSpec );
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
	
	
	static IOPtr unrsrc_file_open( const FSTree* node, int flags, mode_t mode );
	
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
	
	
	static void rsrc_file_remove( const FSTree* node );
	
	static IOPtr rsrc_file_open( const FSTree* node, int flags, mode_t mode );
	
	static off_t rsrc_file_geteof( const FSTree* node );
	
	static const data_method_set rsrc_file_data_methods =
	{
		&rsrc_file_open,
		&rsrc_file_geteof
	};
	
	static const node_method_set rsrc_file_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		&rsrc_file_remove,
		NULL,
		&rsrc_file_data_methods
	};
	
	
	static bool has_resource( const FSSpec& file, const N::GetResInfo_Result& resinfo )
	{
		n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( file, Mac::fsRdPerm );
		
		return ::Get1Resource( resinfo.type, resinfo.id ) != NULL;
	}
	
	static void rsrc_file_remove( const FSTree* node )
	{
		const FSSpec& fileSpec = *(FSSpec*) node->extra();
		
		RdWr_OpenResFile_Scope openResFile( fileSpec );
		
		N::GetResInfo_Result resinfo = GetResInfo_from_name( node->name() );
		
		const N::Handle r = N::Get1Resource( resinfo.type, resinfo.id );
		
		(void) N::RemoveResource( r );
	}
	
	static off_t rsrc_file_geteof( const FSTree* node )
	{
		const FSSpec& fileSpec = *(FSSpec*) node->extra();
		
		n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( fileSpec, Mac::fsRdPerm );
		
		N::GetResInfo_Result resinfo = GetResInfo_from_name( node->name() );
		
		const N::Handle r = N::Get1Resource( resinfo.type, resinfo.id );
		
		return N::GetHandleSize( r );
	}
	
	static IOPtr unrsrc_file_open( const FSTree* node, int flags, mode_t mode )
	{
		const bool writing = flags + (1 - O_RDONLY) & 2;
		
		const bool creating = flags & O_CREAT;
		
		if ( !creating )
		{
			p7::throw_errno( ENOENT );
		}
		
		const FSSpec& fileSpec = *(FSSpec*) node->extra();
		
		{
			RdWr_OpenResFile_Scope openResFile( fileSpec );
			
			N::GetResInfo_Result resinfo = GetResInfo_from_name( node->name() );
			
			(void) N::AddResource( N::NewHandle( 0 ), resinfo );
		}
		
		n::owned< N::Handle > h = N::NewHandle( 0 );
		
		IOHandle* result = writing ? new Rsrc_IOHandle  ( node, flags, h, fileSpec )
		                           : new Handle_IOHandle( node, flags, h );
		
		return result;
	}
	
	static IOPtr rsrc_file_open( const FSTree* node, int flags, mode_t mode )
	{
		const bool writing = flags + (1 - O_RDONLY) & 2;
		
		const FSSpec& fileSpec = *(FSSpec*) node->extra();
		
		n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( fileSpec, Mac::fsRdPerm );
		
		N::GetResInfo_Result resinfo = GetResInfo_from_name( node->name() );
		
		const N::Handle r = N::Get1Resource( resinfo.type, resinfo.id );
		
		n::owned< N::Handle > h = N::DetachResource( r );
		
		IOHandle* result = writing ? new Rsrc_IOHandle  ( node, flags, h, fileSpec )
		                           : new Handle_IOHandle( node, flags, h );
		
		return result;
	}
	
	FSTreePtr Get_RsrcFile_FSTree( const FSTree*        parent,
	                               const plus::string&  name,
	                               const FSSpec&        file )
	{
		const N::GetResInfo_Result resinfo = GetResInfo_from_name( name );
		
		const bool exists = has_resource( file, resinfo );
		
		// FIXME:  Check perms
		const mode_t mode = exists ? S_IFREG | 0400 : 0;
		
		const node_method_set* methods = exists ? &rsrc_file_methods
		                                        : &unrsrc_file_methods;
		
		FSTree* result = new FSTree( parent, name, mode, methods, sizeof (FSSpec) );
		
		*(FSSpec*) result->extra() = file;
		
		return result;
	}
}

