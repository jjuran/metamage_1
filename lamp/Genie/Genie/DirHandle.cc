/*	============
 *	DirHandle.cc
 *	============
 */

#include "Genie/DirHandle.hh"

// Nucleus
#include "Nucleus/OnlyOnce.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Genie
#include "Genie/pathnames.hh"
#include "Genie/ResourceTable.hh"


namespace Genie
{
	
	class VolumesDirHandle : public DirHandle
	{
		public:
			VolumesDirHandle() : DirHandle( N::FSDirSpec() )  {}
			
			const dirent* ReadDir();
	};
	
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
		
		N::FSDirSpec volumes = NN::Convert< N::FSDirSpec >( ResolveUnixPathname( "/Volumes" ) );
		
		DirHandle* handle = dir == volumes ? new VolumesDirHandle() : new DirHandle( dir );
		
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
	
	const dirent* VolumesDirHandle::ReadDir()
	{
		N::Volume_Container::const_iterator it = N::Volumes().begin();
		
		for ( int i = fIndex++; i > 0; --i )
		{
			++it;
			
			if ( it == N::Volumes().end() )
			{
				return NULL;
			}
		}
		
		N::FSVolumeRefNum vRefNum = *it;
		
		//N::FSVolumeRefNum vRefNum = *( N::Volumes().begin() + ++fIndex );
		
		FSSpec volSpec = NN::Convert< FSSpec >( N::RootDirectory( vRefNum ) );
		
		fLastEntry.d_ino = fsRtDirID;
		p2cstrcpy( fLastEntry.d_name, volSpec.name );
		
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

