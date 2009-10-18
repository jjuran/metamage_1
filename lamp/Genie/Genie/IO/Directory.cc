/*	============
 *	Directory.cc
 *	============
 */

#include "Genie/IO/Directory.hh"

// Standard C/C++
#include <cstring>

// POSIX
#include "fcntl.h"
#include "sys/stat.h"

// poseven
#include "poseven/types/errno_t.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	DirHandle::DirHandle( const FSTreePtr& tree )
	:
		IOHandle( O_RDONLY ),
		itsDir( tree )
	{
	}
	
	DirHandle::~DirHandle()
	{
	}
	
	const FSIteratorPtr& DirHandle::Iterator()
	{
		if ( itsIterator == NULL )
		{
			itsIterator = itsDir->Iterate();
		}
		
		return itsIterator;
	}
	
	static void SetDirEntry( dirent& dir, ino_t inode, const std::string& name )
	{
		dir.d_ino = inode;
		
		std::strcpy( dir.d_name, name.c_str() );  // FIXME:  Unsafe!
	}
	
	off_t DirHandle::Seek( off_t offset, int whence )
	{
		FSIteratorPtr iterator = Iterator();
		
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
	
	int DirHandle::ReadDir( dirent& entry )
	{
		FSNode node = Iterator()->Get();
		
		if ( node.name.empty() )
		{
			return 0;
		}
		
		itsIterator->Advance();
		
		SetDirEntry( entry, node.inode, node.name );
		
		return sizeof (dirent);
	}
	
}

