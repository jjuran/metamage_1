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
	
	static const dirent* SetDirEntry( dirent& dir, N::FSDirID inode, ConstStr255Param name )
	{
		dir.d_ino = inode;
		p2cstrcpy( dir.d_name, name );
		
		return &dir;
	}
	
	const dirent* DirHandle::ReadDir()
	{
		CInfoPBRec pb;
		UInt32 dirID = 0;
		::Str255 name_buffer;
		
		ConstStr255Param name = name_buffer;
		
		if ( ++fIndex <= 0 )
		{
			dirID = fIndex == -1 ? SInt32( fDir.dirID ) : NN::Convert< FSSpec >( fDir ).parID;
			name  = fIndex == -1 ? "\p."                : "\p..";
		}
		else
		{
			N::FSpGetCatInfo( fDir, fIndex, pb, name_buffer );
			dirID = pb.hFileInfo.ioDirID;
		}
		
		return SetDirEntry( fLastEntry, dirID, name );
	}
	
	const dirent* VolumesDirHandle::ReadDir()
	{
		UInt32 dirID = fsRtDirID;
		
		ConstStr255Param name = "\p";
		
		if ( ++fIndex <= 0 )
		{
			dirID = fIndex == -1 ? fsRtParID : fsRtDirID;
			name  = fIndex == -1 ? "\p."     : "\p..";
		}
		else
		{
			N::Volume_Container::const_iterator it = N::Volumes().begin();
			
			for ( int i = fIndex - 1;  i > 0;  --i )
			{
				++it;
				
				if ( it == N::Volumes().end() )
				{
					return NULL;
				}
			}
			
			N::FSVolumeRefNum vRefNum = *it;
			
			FSSpec volSpec = NN::Convert< FSSpec >( N::RootDirectory( vRefNum ) );
			name = volSpec.name;
		}
		
		return SetDirEntry( fLastEntry, dirID, name );
	}
	
	void DirHandle::RewindDir()
	{
		fIndex = kStartIndex;
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

