/*
	resources.cc
	------------
*/

#include "Genie/FS/resources.hh"

// Standard C
#include <ctype.h>

// iota
#include "iota/hexidecimal.hh"

// plus
#include "plus/hexidecimal.hh"
#include "plus/var_string.hh"

// nucleus
#include "nucleus/shared.hh"

// Nitrogen
#include "Nitrogen/Resources.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/quad_name.hh"
#include "Genie/IO/Handle.hh"
#include "Genie/Utilities/RdWr_OpenResFile_Scope.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
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
				
				inscribe_quad_name( name, type );
				
				const FSNode node( info.id, name );
				
				cache.push_back( node );
			}
		}
	}
	
	static N::GetResInfo_Result GetResInfo_from_name( const plus::string& name )
	{
		const char* begin = name.data();
		const char* end   = name.size() + begin;
		
		// In the event of a short name, isxdigit( '\0' ) will return false
		
		const bool has_id =      isxdigit( begin[0] )
		                     &&  isxdigit( begin[1] )
		                     &&  isxdigit( begin[2] )
		                     &&  isxdigit( begin[3] );
		
		if ( !has_id )
		{
			p7::throw_errno( ENOENT );
		}
		
		const short id = iota::decode_16_bit_hex( begin );
		
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
			
			boost::shared_ptr< IOHandle > Clone();
			
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
	
	static N::Handle GetOrAddResource( N::ResType type, N::ResID id )
	{
		try
		{
			return N::Get1Resource( type, id );
		}
		catch ( const N::OSStatus& err )
		{
			if ( err != resNotFound )
			{
				throw;
			}
			
			return N::AddResource( N::NewHandle( 0 ), type, id, NULL );
		}
	}
	
	void Rsrc_IOHandle::FlushResource()
	{
		FSTreePtr file = GetFile();
		
		N::GetResInfo_Result resInfo = GetResInfo_from_name( file->Name() );
		
		RdWr_OpenResFile_Scope openResFile( itsFileSpec );
		
		const N::Handle r = GetOrAddResource( resInfo.type, resInfo.id );
		
		const size_t size = GetEOF();
		
		N::SetHandleSize( r, size );
		
		Positioned_Read( *r.Get(), size, 0 );
		
		N::ChangedResource( r );
		N::WriteResource  ( r );
	}
	
	boost::shared_ptr< IOHandle > Rsrc_IOHandle::Clone()
	{
		return seize_ptr( new Rsrc_IOHandle( GetFile(),
		                                     GetFlags(),
		                                     GetHandle(),
		                                     itsFileSpec ) );
	}
	
	void Rsrc_IOHandle::Synchronize( bool metadata )
	{
		FlushResource();
		
		metadata = true;  // until we implement data-only flush
		
		if ( metadata )
		{
			// Just flush the whole volume, since we can't be more specific.
			N::ThrowOSStatus( ::FlushVol( NULL, itsFileSpec.vRefNum ) );
		}
		else
		{
			// Call PBFlushFile(), or high-level wrapper
		}
	}
	
	
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
				FSTree( parent, name ),
				itsFileSpec( file ),
				its_resinfo( GetResInfo_from_name( name ) )
			{
			}
			
			void Delete() const;
			
			off_t GetEOF() const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags, mode_t mode ) const;
			boost::shared_ptr< IOHandle > Open( OpenFlags flags              ) const;
	};
	
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
	
	boost::shared_ptr< IOHandle >
	//
	FSTree_Rsrc_File::Open( OpenFlags flags, mode_t mode ) const
	{
		return Open( flags );
	}
	
	boost::shared_ptr< IOHandle >
	//
	FSTree_Rsrc_File::Open( OpenFlags flags ) const
	{
		const bool writing = flags + (1 - O_RDONLY) & 2;
		
		const bool creating  = flags & O_CREAT;
		const bool excluding = flags & O_EXCL;
		
		n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( itsFileSpec, Mac::fsRdPerm );
		
		n::owned< N::Handle > h;
		
		try
		{
			const N::Handle r = N::Get1Resource( its_resinfo.type, its_resinfo.id );
			
			if ( excluding )
			{
				p7::throw_errno( EEXIST );
			}
			
			h = N::DetachResource( r );
		}
		catch ( const N::OSStatus& err )
		{
			if ( !creating  ||  err != resNotFound )
			{
				throw;
			}
			
			h = N::NewHandle( 0 );
		}
		
		IOHandle* raw_pointer = writing ? new Rsrc_IOHandle  ( Self(), flags, h, itsFileSpec )
		                                : new Handle_IOHandle( Self(), flags, h );
		
		return boost::shared_ptr< IOHandle >( raw_pointer );
	}
	
	FSTreePtr Get_RsrcFile_FSTree( const FSTreePtr&     parent,
	                               const plus::string&  name,
	                               const FSSpec&        file )
	{
		return new FSTree_Rsrc_File( parent, name, file );
	}
}

