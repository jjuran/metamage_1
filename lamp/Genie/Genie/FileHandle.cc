/*	=============
 *	FileHandle.cc
 *	=============
 */

#include "Genie/FileHandle.hh"

// Nitrogen Nucleus
#include "Nucleus/OnlyOnce.h"

// Io
#include "Io/Exceptions.hh"

// Genie
#include "Genie/ResourceTable.hh"


namespace Genie
{
	
	typedef ResourceTable< FileHandle > FileTable;
	
	void RegisterFileRefMod();
	void RegisterFileRefMod()
	{
		RegisterIOType( kFileDescriptor,
		                FileTable::RefMod,
		                FileTable::Read,
		                FileTable::Write,
		                FileTable::Poll );
	}
	
	
	IORef OpenFile( NN::Owned< N::FSFileRefNum > refNum )
	{
		NN::OnlyOnce< RegisterFileRefMod >();
		
		FileHandle* handle = new FileHandle( refNum );
		
		std::size_t offset = FileTable::Add( std::auto_ptr< FileHandle >( handle ) );
		
		return IORef( kFileDescriptor, offset, handle );
	}
	
	IORef OpenFile( const FSSpec& file, N::FSIOPermissions perm )
	{
		NN::OnlyOnce< RegisterFileRefMod >();
		
		FileHandle* handle = new FileHandle( file, perm );
		
		std::size_t offset = FileTable::Add( std::auto_ptr< FileHandle >( handle ) );
		
		return IORef( kFileDescriptor, offset, handle );
	}
	
	
	FileHandle::FileHandle( const FSSpec& file, N::FSIOPermissions perm )
	:
		fRefNum( N::FSpOpenDF( file, perm ) )
	{
	}
	
	int FileHandle::SysRead( char* data, std::size_t byteCount )
	{
		try
		{
			return N::FSRead( fRefNum, byteCount, data );
		}
		catch ( const N::EOFErr& err )
		{
			ASSERT( err.Get() == eofErr );
			
			throw Io::EndOfInput();
		}
		
		// Not reached
		return 0;
	}
	
	int FileHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		return N::FSWrite( fRefNum, byteCount, data );
	}
	
}

