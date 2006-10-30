/*	============
 *	DirHandle.cc
 *	============
 */

#include "Genie/IO/Directory.hh"

#include <cstring>


namespace Genie
{
	
	
	static const dirent* SetDirEntry( dirent& dir, ino_t inode, const std::string& name )
	{
		dir.d_ino = inode;
		std::strcpy( dir.d_name, name.c_str() );  // FIXME:  Unsafe!
		
		return &dir;
	}
	
	DirHandle::DirHandle( const FSTreePtr tree ) : iterator( tree->Iterate() )
	{
	}
	
	const dirent* DirHandle::ReadDir()
	{
		FSNode node = iterator->Get();
		
		if ( node.tree == NULL )
		{
			return NULL;
		}
		
		iterator->Advance();
		
		return SetDirEntry( fLastEntry, node.tree->Inode(), node.name );
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

