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

// Genie
#include "Genie/FS/FSTreeCache.hh"
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
	
	
	void iterate_resources( const FSSpec& file, FSTreeCache& cache )
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
				
				const FSNode node( info.id, name );
				
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
		
		N::GetResInfo_Result resInfo = GetResInfo_from_name( file->Name() );
		
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
	
	
	class FSTree_Unrsrc_File : public FSTree
	{
		private:
			FSSpec itsFileSpec;
			
			N::GetResInfo_Result its_resinfo;
		
		public:
			FSTree_Unrsrc_File( const FSTreePtr&     parent,
			                    const plus::string&  name,
			                    const FSSpec&        file )
			:
				FSTree( parent, name, 0 ),
				itsFileSpec( file ),
				its_resinfo( GetResInfo_from_name( name ) )
			{
			}
			
			IOPtr Open( OpenFlags flags, mode_t mode ) const;
	};
	
	
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
		NULL,
		NULL,
		NULL,
		NULL,
		&rsrc_file_data_methods
	};
	
	
	class FSTree_Rsrc_File : public FSTree
	{
		private:
			FSSpec itsFileSpec;
			
			N::GetResInfo_Result its_resinfo;
		
		public:
			FSTree_Rsrc_File( const FSTreePtr&     parent,
			                  const plus::string&  name,
			                  const FSSpec&        file )
			:
				FSTree( parent, name, S_IFREG | 0400, &rsrc_file_methods ),  // FIXME:  Check perms
				itsFileSpec( file ),
				its_resinfo( GetResInfo_from_name( name ) )
			{
			}
			
			void Delete() const;
			
			off_t GetEOF() const;
			
			IOPtr Open( OpenFlags flags, mode_t mode ) const;
	};
	
	static bool has_resource( const FSSpec& file, const N::GetResInfo_Result& resinfo )
	{
		n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( file, Mac::fsRdPerm );
		
		return ::Get1Resource( resinfo.type, resinfo.id ) != NULL;
	}
	
	void FSTree_Rsrc_File::Delete() const
	{
		RdWr_OpenResFile_Scope openResFile( itsFileSpec );
		
		const N::Handle r = N::Get1Resource( its_resinfo.type, its_resinfo.id );
		
		(void) N::RemoveResource( r );
	}
	
	off_t FSTree_Rsrc_File::GetEOF() const
	{
		n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( itsFileSpec, Mac::fsRdPerm );
		
		const N::Handle r = N::Get1Resource( its_resinfo.type, its_resinfo.id );
		
		return N::GetHandleSize( r );
	}
	
	IOPtr FSTree_Unrsrc_File::Open( OpenFlags flags, mode_t mode ) const
	{
		const bool writing = flags + (1 - O_RDONLY) & 2;
		
		const bool creating = flags & O_CREAT;
		
		if ( !creating )
		{
			p7::throw_errno( ENOENT );
		}
		
		{
			RdWr_OpenResFile_Scope openResFile( itsFileSpec );
			
			(void) N::AddResource( N::NewHandle( 0 ), its_resinfo );
		}
		
		n::owned< N::Handle > h = N::NewHandle( 0 );
		
		IOHandle* result = writing ? new Rsrc_IOHandle  ( Self(), flags, h, itsFileSpec )
		                           : new Handle_IOHandle( Self(), flags, h );
		
		return result;
	}
	
	IOPtr FSTree_Rsrc_File::Open( OpenFlags flags, mode_t mode ) const
	{
		const bool writing = flags + (1 - O_RDONLY) & 2;
		
		const bool excluding = flags & O_EXCL;
		
		if ( excluding )
		{
			p7::throw_errno( EEXIST );
		}
		
		n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( itsFileSpec, Mac::fsRdPerm );
		
		const N::Handle r = N::Get1Resource( its_resinfo.type, its_resinfo.id );
		
		n::owned< N::Handle > h = N::DetachResource( r );
		
		IOHandle* result = writing ? new Rsrc_IOHandle  ( Self(), flags, h, itsFileSpec )
		                           : new Handle_IOHandle( Self(), flags, h );
		
		return result;
	}
	
	static IOPtr rsrc_file_open( const FSTree* node, int flags, mode_t mode )
	{
		const FSTree_Rsrc_File* file = static_cast< const FSTree_Rsrc_File* >( node );
		
		return file->Open( flags, mode );
	}
	
	static off_t rsrc_file_geteof( const FSTree* node )
	{
		const FSTree_Rsrc_File* file = static_cast< const FSTree_Rsrc_File* >( node );
		
		return file->GetEOF();
	}
	
	FSTreePtr Get_RsrcFile_FSTree( const FSTreePtr&     parent,
	                               const plus::string&  name,
	                               const FSSpec&        file )
	{
		const N::GetResInfo_Result resinfo = GetResInfo_from_name( name );
		
		const bool exists = has_resource( file, resinfo );
		
		typedef const FSTree* T;
		
		return exists ? T( new FSTree_Rsrc_File  ( parent, name, file ) )
		              : T( new FSTree_Unrsrc_File( parent, name, file ) );
	}
}

