/*	==============
 *	ResFile_Dir.cc
 *	==============
 */

#include "Genie/FS/ResFile_Dir.hh"

// Iota
#include "iota/decimal.hh"

// plus
#include "plus/var_string.hh"

// nucleus
#include "nucleus/shared.hh"

// Nitrogen
#include "Nitrogen/Resources.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/quad_name.hh"
#include "Genie/IO/Handle.hh"
#include "Genie/Utilities/AsyncIO.hh"
#include "Genie/Utilities/RdWr_OpenResFile_Scope.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
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
			Resource_IOHandle( const FSTreePtr&               file,
			                   OpenFlags                      flags,
			                   const n::shared< N::Handle >&  h,
			                   const FSSpec&                  resFile )
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
		
		const ::OSType osType = parse_quad_name( file->ParentRef()->Name() );
		
		const N::ResType type = N::ResType( osType );
		
		const N::ResID id = N::ResID( iota::parse_decimal( file->Name().c_str() ) );
		
		RdWr_OpenResFile_Scope openResFile( itsFileSpec );
		
		const N::Handle r = GetOrAddResource( type, id );
		
		const size_t size = GetEOF();
		
		N::SetHandleSize( r, size );
		
		Positioned_Read( *r.Get(), size, 0 );
		
		N::ChangedResource( r );
		N::WriteResource  ( r );
	}
	
	boost::shared_ptr< IOHandle > Resource_IOHandle::Clone()
	{
		return seize_ptr( new Resource_IOHandle( GetFile(),
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
			FSTree_RsrcFile_Type_ID( const FSTreePtr&     parent,
			                         const plus::string&  name,
			                         const FSSpec&        file )
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
		const ::OSType type = parse_quad_name( ParentRef()->Name() );
		
		return N::ResType( type );
	}
	
	N::ResID FSTree_RsrcFile_Type_ID::GetID() const
	{
		const int id = iota::parse_decimal( Name().c_str() );
		
		return N::ResID( id );
	}
	
	void FSTree_RsrcFile_Type_ID::Delete() const
	{
		const N::ResType  type = GetType();
		const N::ResID    id   = GetID  ();
		
		RdWr_OpenResFile_Scope openResFile( itsFileSpec );
		
		const N::Handle r = N::Get1Resource( type, id );
		
		(void) N::RemoveResource( r );
	}
	
	off_t FSTree_RsrcFile_Type_ID::GetEOF() const
	{
		const N::ResType  type = GetType();
		const N::ResID    id   = GetID  ();
		
		n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( itsFileSpec, Mac::fsRdPerm );
		
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
		
		n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( itsFileSpec, Mac::fsRdPerm );
		
		n::owned< N::Handle > h;
		
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
			FSTree_RsrcFile_Type( const FSTreePtr&     parent,
			                      const plus::string&  name,
			                      const FSSpec&        file )
			:
				FSTree_Directory( parent, name ),
				itsFileSpec( file )
			{
			}
			
			FSTreePtr Lookup_Child( const plus::string& name, const FSTree* parent ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
			
	};
	
	N::ResType FSTree_RsrcFile_Type::GetType() const
	{
		const ::OSType type = parse_quad_name( Name() );
		
		return N::ResType( type );
	}
	
	FSTreePtr FSTree_RsrcFile_Type::Lookup_Child( const plus::string& name, const FSTree* parent ) const
	{
		// FIXME:  verify name converts to an id
		
		return seize_ptr( new FSTree_RsrcFile_Type_ID( (parent ? parent : this)->Self(), name, itsFileSpec ) );
	}
	
	void FSTree_RsrcFile_Type::IterateIntoCache( FSTreeCache& cache ) const
	{
		const N::ResType type = GetType();
		
		n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( itsFileSpec, Mac::fsRdPerm );
		
		const short n_rsrcs = N::Count1Resources( type );
		
		for ( short i = 1;  i <= n_rsrcs;  ++i )
		{
			const N::Handle r = N::Get1IndResource( type, i );
			
			const N::GetResInfo_Result info = N::GetResInfo( r );
			
			const plus::string name = iota::inscribe_decimal( info.id );
			
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
			
			bool Exists() const;
			
			void CreateDirectory( mode_t mode ) const;
			
			FSTreePtr Lookup_Child( const plus::string& name, const FSTree* parent ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
			
	};
	
	
	bool FSTree_ResFileDir::Exists() const
	{
		::ResFileRefNum refNum = ::FSpOpenResFile( &itsFileSpec, fsRdPerm );
		
		const bool exists = refNum >= 0;
		
		if ( exists )
		{
			::CloseResFile( refNum );
		}
		else
		{
			const OSErr err = ::ResError();
			
			if ( err != resFNotFound )
			{
				N::ThrowOSStatus( err );
			}
		}
		
		return exists;
	}
	
	static inline bool contains( const char* s, size_t length, char c )
	{
		const char* end = s + length;
		
		return std::find( s, end, c ) != end;
	}
	
	static inline bool is_rsrc_file( const CInfoPBRec&  cInfo,
	                                 ConstStr255Param   name )
	{
		return cInfo.hFileInfo.ioFlLgLen == 0  &&  !contains( (char*) &name[ 1 ],
		                                                      1 + name[0],
		                                                      '.' );
	}
	
	void FSTree_ResFileDir::CreateDirectory( mode_t mode ) const
	{
		CInfoPBRec cInfo = { 0 };
		
		const bool exists = FSpGetCatInfo< FNF_Returns >( cInfo, false, itsFileSpec );
		
		::OSType creator;
		::OSType type;
		
		if ( !exists || is_rsrc_file( cInfo, itsFileSpec.name ) )
		{
			creator = 'RSED';
			type    = 'rsrc';
		}
		else
		{
			const FInfo& fInfo = cInfo.hFileInfo.ioFlFndrInfo;
			
			creator = fInfo.fdCreator;
			type    = fInfo.fdType;
		}
		
		N::FSpCreateResFile( itsFileSpec,
		                     Mac::FSCreator( creator ),
		                     Mac::FSType   ( type    ),
		                     Mac::smSystemScript );
	}
	
	FSTreePtr FSTree_ResFileDir::Lookup_Child( const plus::string& name, const FSTree* parent ) const
	{
		// Throws if conversion to OSType fails.
		(void) parse_quad_name( name );
		
		return seize_ptr( new FSTree_RsrcFile_Type( (parent ? parent : this)->Self(), name, itsFileSpec ) );
	}
	
	void FSTree_ResFileDir::IterateIntoCache( FSTreeCache& cache ) const
	{
		n::owned< N::ResFileRefNum > resFile = N::FSpOpenResFile( itsFileSpec, Mac::fsRdPerm );
		
		const short n_types = N::Count1Types();
		
		for ( short i = 1;  i <= n_types;  ++i )
		{
			const ::ResType type = N::Get1IndType( i );
			
			plus::var_string name;
			
			inscribe_quad_name( name, type );
			
			const FSNode node( i, name );
			
			cache.push_back( node );
		}
	}
	
	
	FSTreePtr Get_ResFileDir_FSTree( const FSSpec& file, bool onServer )
	{
		return seize_ptr( new FSTree_ResFileDir( file, onServer ) );
	}
	
}

