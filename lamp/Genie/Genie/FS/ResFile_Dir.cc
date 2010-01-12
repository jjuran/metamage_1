/*	==============
 *	ResFile_Dir.cc
 *	==============
 */

#include "Genie/FS/ResFile_Dir.hh"

// Iota
#include "iota/decimal.hh"

// Nucleus
#include "Nucleus/Shared.h"

// Nitrogen
#include "Nitrogen/Resources.h"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/Name_OSType.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	class Handle_IOHandle : public VirtualFileHandle< RegularFileHandle >
	{
		private:
			NN::Shared< N::Handle > itsHandle;
		
		protected:
			const NN::Shared< N::Handle >& GetHandle() const  { return itsHandle; }
		
		public:
			Handle_IOHandle( const FSTreePtr&                file,
			                 OpenFlags                       flags,
			                 const NN::Shared< N::Handle >&  h )
			:
				VirtualFileHandle< RegularFileHandle >( file, flags ),
				itsHandle( h )
			{
			}
			
			~Handle_IOHandle();
			
			boost::shared_ptr< IOHandle > Clone();
			
			ssize_t Positioned_Read( char* buffer, size_t n_bytes, off_t offset );
			
			ssize_t Positioned_Write( const char* buffer, size_t n_bytes, off_t offset );
			
			off_t GetEOF()  { return N::GetHandleSize( itsHandle ); }
			
			void SetEOF( off_t length )  { N::SetHandleSize( itsHandle, length ); }
	};
	
	Handle_IOHandle::~Handle_IOHandle()
	{
	}
	
	boost::shared_ptr< IOHandle > Handle_IOHandle::Clone()
	{
		return boost::shared_ptr< IOHandle >( new Handle_IOHandle( GetFile(),
		                                                           GetFlags(),
		                                                           itsHandle ) );
	}
	
	ssize_t Handle_IOHandle::Positioned_Read( char* buffer, size_t n_bytes, off_t offset )
	{
		const size_t size = GetEOF();
		
		if ( offset >= size )
		{
			return 0;
		}
		
		n_bytes = std::min< size_t >( n_bytes, size - offset );
		
		memcpy( buffer, *itsHandle.get().Get() + offset, n_bytes );
		
		return n_bytes;
	}
	
	ssize_t Handle_IOHandle::Positioned_Write( const char* buffer, size_t n_bytes, off_t offset )
	{
		const bool writable = GetFlags() + (1 - O_RDONLY) & 2;
		
		if ( !writable )
		{
			p7::throw_errno( EPERM );
		}
		
		if ( n_bytes == 0 )
		{
			return 0;
		}
		
		const size_t required_size = offset + n_bytes;
		
		const size_t existing_size = GetEOF();
		
		if ( required_size > existing_size )
		{
			try
			{
				N::SetHandleSize( itsHandle, required_size );
			}
			catch ( ... )
			{
				if ( offset > existing_size )
				{
					throw;
				}
				
				n_bytes = std::min< size_t >( n_bytes, existing_size - offset );
			}
		}
		
		memcpy( *itsHandle.get().Get() + offset, buffer, n_bytes );
		
		return n_bytes;
	}
	
	
	class Resource_IOHandle : public Handle_IOHandle
	{
		private:
			FSSpec itsFileSpec;
		
		private:
			void FlushResource();
		
		private:
			// non-copyable
			Resource_IOHandle           ( const Resource_IOHandle& );
			Resource_IOHandle& operator=( const Resource_IOHandle& );
		
		public:
			Resource_IOHandle( const FSTreePtr&                file,
			                   OpenFlags                       flags,
			                   const NN::Shared< N::Handle >&  h,
			                   const FSSpec&                   resFile )
			:
				Handle_IOHandle( file, flags, h ),
				itsFileSpec( resFile )
			{
			}
			
			~Resource_IOHandle();
			
			boost::shared_ptr< IOHandle > Clone();
			
			void Synchronize( bool metadata );
	};
	
	Resource_IOHandle::~Resource_IOHandle()
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
	
	void Resource_IOHandle::FlushResource()
	{
		FSTreePtr file = GetFile();
		
		const ::OSType osType = OSType_KeyName_Traits::KeyFromName( file->ParentRef()->Name() );
		
		const N::ResType type = N::ResType( osType );
		
		const N::ResID id = N::ResID( atoi( file->Name().c_str() ) );
		
		NN::Owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( itsFileSpec, N::fsRdWrPerm );
		
		const N::Handle r = GetOrAddResource( type, id );
		
		const size_t size = GetEOF();
		
		N::SetHandleSize( r, size );
		
		Positioned_Read( *r.Get(), size, 0 );
		
		N::ChangedResource( r );
		N::WriteResource  ( r );
	}
	
	boost::shared_ptr< IOHandle > Resource_IOHandle::Clone()
	{
		return boost::shared_ptr< IOHandle >( new Resource_IOHandle( GetFile(),
		                                                             GetFlags(),
		                                                             GetHandle(),
		                                                             itsFileSpec ) );
	}
	
	void Resource_IOHandle::Synchronize( bool metadata )
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
	
	
	class FSTree_RsrcFile_Type_ID : public FSTree
	{
		private:
			FSSpec itsFileSpec;
		
		private:
			N::ResType  GetType() const;
			N::ResID    GetID  () const;
		
		public:
			FSTree_RsrcFile_Type_ID( const FSTreePtr&    parent,
			                         const std::string&  name,
			                         const FSSpec&       file )
			:
				FSTree( parent, name ),
				itsFileSpec( file )
			{
			}
			
			void Delete() const;
			
			off_t GetEOF() const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	N::ResType FSTree_RsrcFile_Type_ID::GetType() const
	{
		const ::OSType type = OSType_KeyName_Traits::KeyFromName( ParentRef()->Name() );
		
		return N::ResType( type );
	}
	
	N::ResID FSTree_RsrcFile_Type_ID::GetID() const
	{
		const int id = atoi( Name().c_str() );
		
		return N::ResID( id );
	}
	
	void FSTree_RsrcFile_Type_ID::Delete() const
	{
		const N::ResType  type = GetType();
		const N::ResID    id   = GetID  ();
		
		NN::Owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( itsFileSpec, N::fsRdWrPerm );
		
		const N::Handle r = N::Get1Resource( type, id );
		
		(void) N::RemoveResource( r );
	}
	
	off_t FSTree_RsrcFile_Type_ID::GetEOF() const
	{
		const N::ResType  type = GetType();
		const N::ResID    id   = GetID  ();
		
		NN::Owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( itsFileSpec, N::fsRdPerm );
		
		const N::Handle r = N::Get1Resource( type, id );
		
		return N::GetHandleSize( r );
	}
	
	boost::shared_ptr< IOHandle >
	//
	FSTree_RsrcFile_Type_ID::Open( OpenFlags flags ) const
	{
		const bool writing = flags + (1 - O_RDONLY) & 2;
		
		const bool creating  = flags & O_CREAT;
		const bool excluding = flags & O_EXCL;
		
		const N::ResType  type = GetType();
		const N::ResID    id   = GetID  ();
		
		NN::Owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( itsFileSpec, N::fsRdPerm );
		
		NN::Owned< N::Handle > h;
		
		try
		{
			const N::Handle r = N::Get1Resource( type, id );
			
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
		
		IOHandle* raw_pointer = writing ? new Resource_IOHandle( Self(), flags, h, itsFileSpec )
		                                : new Handle_IOHandle  ( Self(), flags, h );
		
		return boost::shared_ptr< IOHandle >( raw_pointer );
	}
	
	
	class FSTree_RsrcFile_Type : public FSTree_Directory
	{
		private:
			FSSpec itsFileSpec;
		
		private:
			N::ResType GetType() const;
		
		public:
			FSTree_RsrcFile_Type( const FSTreePtr&    parent,
			                      const std::string&  name,
			                      const FSSpec&       file )
			:
				FSTree_Directory( parent, name ),
				itsFileSpec( file )
			{
			}
			
			FSTreePtr Lookup_Child( const std::string& name ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
			
	};
	
	N::ResType FSTree_RsrcFile_Type::GetType() const
	{
		const ::OSType type = OSType_KeyName_Traits::KeyFromName( Name() );
		
		return N::ResType( type );
	}
	
	FSTreePtr FSTree_RsrcFile_Type::Lookup_Child( const std::string& name ) const
	{
		// FIXME:  verify name converts to an id
		
		return seize_ptr( new FSTree_RsrcFile_Type_ID( Self(), name, itsFileSpec ) );
	}
	
	void FSTree_RsrcFile_Type::IterateIntoCache( FSTreeCache& cache ) const
	{
		const N::ResType type = GetType();
		
		NN::Owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( itsFileSpec, N::fsRdPerm );
		
		const short n_rsrcs = N::Count1Resources( type );
		
		for ( short i = 1;  i <= n_rsrcs;  ++i )
		{
			const N::Handle r = N::Get1IndResource( type, i );
			
			const N::GetResInfo_Result info = N::GetResInfo( r );
			
			const std::string name = iota::inscribe_decimal( info.id );
			
			const FSNode node( i, name );
			
			cache.push_back( node );
		}
	}
	
	
	class FSTree_ResFileDir : public FSTree_Directory
	{
		private:
			FSSpec  itsFileSpec;
			bool    itIsOnServer;
		
		public:
			FSTree_ResFileDir( const FSSpec& file, bool onServer )
			:
				FSTree_Directory( FSTreeFromFSSpec( file, onServer ), "rsrc" ),
				itsFileSpec( file ),
				itIsOnServer( onServer )
			{
			}
			
			FSTreePtr Lookup_Child( const std::string& name ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
			
	};
	
	
	FSTreePtr FSTree_ResFileDir::Lookup_Child( const std::string& name ) const
	{
		// Throws if conversion to OSType fails.
		(void) OSType_KeyName_Traits::KeyFromName( name );
		
		return seize_ptr( new FSTree_RsrcFile_Type( Self(), name, itsFileSpec ) );
	}
	
	void FSTree_ResFileDir::IterateIntoCache( FSTreeCache& cache ) const
	{
		NN::Owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( itsFileSpec, N::fsRdPerm );
		
		const short n_types = N::Count1Types();
		
		for ( short i = 1;  i <= n_types;  ++i )
		{
			const ::ResType type = N::Get1IndType( i );
			
			std::string name = OSType_KeyName_Traits::NameFromKey( N::OSType( type ) );
			
			const FSNode node( i, name );
			
			cache.push_back( node );
		}
	}
	
	
	FSTreePtr Get_ResFileDir_FSTree( const FSSpec& file, bool onServer )
	{
		return seize_ptr( new FSTree_ResFileDir( file, onServer ) );
	}
	
}

