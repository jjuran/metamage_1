/*	=============
 *	FileHandle.cc
 *	=============
 */

#include "Genie/FileHandle.hh"

// Nitrogen core
#include "Nitrogen/OnlyOnce.h"

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
		                FileTable::Write );
	}
	
	
	IORef OpenFile( N::Owned< N::FSFileRefNum > refNum )
	{
		N::OnlyOnce< RegisterFileRefMod >();
		
		std::auto_ptr< FileHandle > handle( new FileHandle( refNum ) );
		
		std::size_t offset = FileTable::Add( handle );
		
		return IORef( kFileDescriptor, offset );
	}
	
	IORef OpenFile( const FSSpec& file, N::FSIOPermissions perm )
	{
		N::OnlyOnce< RegisterFileRefMod >();
		
		std::auto_ptr< FileHandle > handle( new FileHandle( file, perm ) );
		
		std::size_t offset = FileTable::Add( handle );
		
		return IORef( kFileDescriptor, offset );
	}
	
	
	FileHandle::FileHandle( const FSSpec& file, N::FSIOPermissions perm )
	:
		fRefNum( N::FSpOpenDF( file, perm ) )
	{
	}
	
	int FileHandle::Read( char* data, std::size_t byteCount )
	{
		return N::FSRead( fRefNum, byteCount, data );
	}
	
	int FileHandle::Write( const char* data, std::size_t byteCount )
	{
		return N::FSWrite( fRefNum, byteCount, data );
	}
	
}

