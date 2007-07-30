/*	============
 *	Directory.cc
 *	============
 */

#include "Genie/IO/Directory.hh"

// Standard C/C++
#include <cstring>

// POSIX
#include "sys/stat.h"


namespace Genie
{
	
	DirHandle::DirHandle( const FSTreePtr& tree ) : itsDir( tree ), iterator( tree->Iterate() )
	{
	}
	
	DirHandle::~DirHandle()
	{
	}
	
	
	static const dirent* SetDirEntry( dirent& dir, ino_t inode, const std::string& name )
	{
		dir.d_ino = inode;
		std::strcpy( dir.d_name, name.c_str() );  // FIXME:  Unsafe!
		
		return &dir;
	}
	
	const dirent* DirHandle::ReadDir()
	{
		FSNode node = iterator->Get();
		
		if ( node.tree == NULL )
		{
			return NULL;
		}
		
		iterator->Advance();
		
		struct ::stat sb;
		
		node.tree->Stat( sb );
		
		return SetDirEntry( fLastEntry, sb.st_ino, node.name );
	}
	
	void DirHandle::RewindDir()
	{
		iterator->Rewind();
	}
	
	void DirHandle::SeekDir( off_t index )
	{
		iterator->Seek( index );
	}
	
	off_t DirHandle::TellDir() const
	{
		return iterator->Tell();
	}
	
}

