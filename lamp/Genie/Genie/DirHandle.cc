/*	============
 *	DirHandle.cc
 *	============
 */

#include "Genie/DirHandle.hh"

// Nitrogen Nucleus
#include "Nucleus/OnlyOnce.h"

// Genie
#include "Genie/ResourceTable.hh"


namespace Genie
{
	
	typedef ResourceTable< DirHandle > DirTable;
	
	void RegisterDirRefMod();
	void RegisterDirRefMod()
	{
		RegisterIOType( kDirectoryIterator,
		                DirTable::RefMod,
		                DirTable::Read,
		                DirTable::Write,
		                DirTable::Poll );
	}
	
	
	IORef OpenDir( const N::FSDirSpec& dir )
	{
		NN::OnlyOnce< RegisterDirRefMod >();
		
		DirHandle* handle = new DirHandle( dir );
		
		std::size_t offset = DirTable::Add( std::auto_ptr< DirHandle >( handle ) );
		
		return IORef( kDirectoryIterator, offset, handle );
	}
	
	const dirent* DirHandle::ReadDir()
	{
		CInfoPBRec pb;
		::Str255 name;
		
		N::FSpGetCatInfo( fDir, ++fIndex, pb, name );
		
		fLastEntry.d_ino = pb.hFileInfo.ioDirID;
		p2cstrcpy( fLastEntry.d_name, name );
		
		return &fLastEntry;
	}
	
	void DirHandle::RewindDir()
	{
		fIndex = 0;
	}
	
	void DirHandle::SeekDir( off_t index )
	{
		fIndex = index;
	}
	
	off_t DirHandle::TellDir() const
	{
		return fIndex;
	}
	
}

