/*	============
 *	Directory.cc
 *	============
 */

#include "Genie/IO/Directory.hh"

// Standard C/C++
#include <cstring>

// POSIX
#include "sys/stat.h"

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
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
	
	off_t DirHandle::Seek( off_t offset, int whence )
	{
		off_t position = 0;
		
		switch ( whence )
		{
			case SEEK_SET:
				//position = 0;
				break;
			
			case SEEK_CUR:
				position = iterator->Tell();
				break;
			
			case SEEK_END:
			default:
				p7::throw_errno( EINVAL );
		}
		
		position += offset;
		
		iterator->Seek( position );
		
		return position;
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
	
}

