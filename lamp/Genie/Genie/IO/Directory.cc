/*	============
 *	DirHandle.cc
 *	============
 */

#include "Genie/IO/Directory.hh"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"


namespace Genie
{
	
	namespace NN = Nucleus;
	
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

