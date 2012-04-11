/*	=========
 *	FSTree.cc
 *	=========
 */

#include "Genie/FS/FSTree.hh"

// POSIX
#include "fcntl.h"
#include "sys/stat.h"

// plus
#include "plus/hexidecimal.hh"
#include "plus/string/concat.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/dir_method_set.hh"
#include "Genie/FS/file_method_set.hh"
#include "Genie/FS/link_method_set.hh"
#include "Genie/FS/misc_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/IO/VirtualDirectory.hh"


#ifndef UTIME_NOW
#define UTIME_NOW   ((1 << 30) - 1)
#endif


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	const FSTreePtr null_FSTreePtr = FSTreePtr();
	
	
	FSTree::FSTree()
	:
		itsParent(),
		itsName(),
		itsMode(),
		its_methods(),
		its_extra(),
		its_destructor()
	{
	}
	
	static inline plus::string NameFromPtr( const FSTree* ptr )
	{
		return plus::encode_32_bit_hex( (unsigned) ptr );
	}
	
	FSTree::FSTree( const FSTreePtr&        parent,
	                const plus::string&     name,
	                mode_t                  mode,
	                const node_method_set*  methods,
	                std::size_t             n_extra,
	                node_destructor         dtor )
	:
		itsParent( parent ),
		itsName  ( name[0] == '/' ? NameFromPtr( this )
		                          : name ),
		itsMode  ( mode ),
		its_methods( methods ),
		its_extra( n_extra ? ::operator new( n_extra ) : NULL ),
		its_destructor( dtor )
	{
	}
	
	FSTree::~FSTree()
	{
		if ( its_destructor )
		{
			its_destructor( this );
		}
		
		::operator delete( its_extra );
	}
	
	plus::string FSTree::Pathname() const
	{
		plus::string pathname = Name();
		
		FSTreePtr tree = Parent();
		
		if ( tree.get() == this  &&  !pathname.empty() )
		{
			// Anonymous files have null parents (but so does root)
			return pathname;
		}
		
		while ( true )
		{
			const plus::string& name = tree->Name();
			
			pathname = name + "/" + pathname;
			
			// Root dir has empty name
			
			if ( name.empty() )
			{
				break;
			}
			
			tree = tree->Parent();
		}
		
		return pathname;
	}
	
	
	FSTreePtr FSTree::Parent() const
	{
		if ( itsParent.get() )
		{
			return itsParent;
		}
		
		const misc_method_set* misc_methods;
		
		if ( its_methods  &&  (misc_methods = its_methods->misc_methods) )
		{
			if ( misc_methods->parent )
			{
				return misc_methods->parent( this );
			}
		}
		
		return Self();
	}
	
	ino_t FSTree::Inode() const
	{
		const misc_method_set* misc_methods;
		
		if ( its_methods  &&  (misc_methods = its_methods->misc_methods) )
		{
			if ( misc_methods->inode )
			{
				return misc_methods->inode( this );
			}
		}
		
		return 0;
	}
	
	ino_t FSTree::ParentInode() const
	{
		const misc_method_set* misc_methods;
		
		if ( its_methods  &&  (misc_methods = its_methods->misc_methods) )
		{
			if ( misc_methods->parent_inode )
			{
				return misc_methods->parent_inode( this );
			}
		}
		
		return Parent()->Inode();
	}
	
	void FSTree::Stat( struct ::stat& sb ) const
	{
		if ( its_methods  &&  its_methods->stat )
		{
			its_methods->stat( this, sb );
			
			return;
		}
		
		if ( itsMode == 0 )
		{
			p7::throw_errno( ENOENT );
		}
		
		const time_t now = time( NULL );
		
		sb.st_ino = Inode();
		
		sb.st_mode = itsMode;
		
		sb.st_nlink = 1;
		
		// fstatat() or fstat() will fill in sb.st_size
		
		sb.st_blksize = 4096;
		
		sb.st_atime = now;
		sb.st_mtime = now;
		sb.st_ctime = now;
	}
	
	void FSTree::ChangeMode( mode_t mode ) const
	{
		if ( its_methods  &&  its_methods->chmod )
		{
			its_methods->chmod( this, mode );
		}
		else
		{
			p7::throw_errno( EPERM );
		}
	}
	
	void FSTree::SetTimes() const
	{
		if ( its_methods  &&  its_methods->touch )
		{
			its_methods->touch( this );
		}
		else
		{
			const struct timespec times[2] = { { 0, UTIME_NOW }, { 0, UTIME_NOW } };
			
			SetTimes( times );
		}
	}
	
	void FSTree::SetTimes( const struct timespec times[2] ) const
	{
		if ( its_methods  &&  its_methods->utime )
		{
			its_methods->utime( this, times );
		}
		else
		{
			p7::throw_errno( EPERM );
		}
	}
	
	void FSTree::Delete() const
	{
		if ( its_methods  &&  its_methods->remove )
		{
			its_methods->remove( this );
		}
		else
		{
			p7::throw_errno( EPERM );
		}
	}
	
	void FSTree::Rename( const FSTreePtr& destination ) const
	{
		if ( its_methods  &&  its_methods->rename )
		{
			its_methods->rename( this, destination.get() );
		}
		else
		{
			p7::throw_errno( EINVAL );
		}
	}
	
	void FSTree::HardLink( const FSTreePtr& destination ) const
	{
		const file_method_set* file_methods;
		
		if ( its_methods  &&  (file_methods = its_methods->file_methods) )
		{
			if ( file_methods->hardlink )
			{
				file_methods->hardlink( this, destination.get() );
				
				return;
			}
		}
		
		{
			p7::throw_errno( EINVAL );
		}
	}
	
	void FSTree::CopyFile( const FSTreePtr& destination ) const
	{
		const file_method_set* file_methods;
		
		if ( its_methods  &&  (file_methods = its_methods->file_methods) )
		{
			if ( file_methods->copyfile )
			{
				file_methods->copyfile( this, destination.get() );
				
				return;
			}
		}
		
		{
			p7::throw_errno( EINVAL );
		}
	}
	
	off_t FSTree::GetEOF() const
	{
		const data_method_set* data_methods;
		
		if ( its_methods  &&  (data_methods = its_methods->data_methods) )
		{
			if ( data_methods->geteof )
			{
				return data_methods->geteof( this );
			}
		}
		
		// Errors are meaningless here since there's no POSIX call specifically
		// to get EOF (as there is truncate() for setting EOF).  We only care so
		// we can populate stat::st_size.  Just return zero.
		
		return 0;
	}
	
	void FSTree::SetEOF( off_t length ) const
	{
		// This confuses MWCPPC when optimizing:
		//p7::throw_errno( IsDirectory() ? EISDIR : EINVAL );
		// internal compiler error: File: 'PCodeUtilities.c' Line: 80
		
		const data_method_set* data_methods;
		
		if ( its_methods  &&  (data_methods = its_methods->data_methods) )
		{
			if ( data_methods->seteof )
			{
				data_methods->seteof( this, length );
				
				return;
			}
		}
		
		if ( S_ISDIR( itsMode ) )
		{
			p7::throw_errno( EISDIR );
		}
		else
		{
			p7::throw_errno( EINVAL );
		}
	}
	
	plus::string FSTree::ReadLink() const
	{
		const link_method_set* link_methods;
		
		if ( its_methods  &&  (link_methods = its_methods->link_methods) )
		{
			if ( link_methods->readlink )
			{
				return link_methods->readlink( this );
			}
			
			if ( link_methods->resolve )
			{
				return link_methods->resolve( this )->Pathname();
			}
		}
		
		throw p7::errno_t( EINVAL );
	}
	
	FSTreePtr FSTree::ResolveLink() const
	{
		const link_method_set* link_methods;
		
		if ( its_methods  &&  (link_methods = its_methods->link_methods) )
		{
			if ( link_methods->resolve )
			{
				return link_methods->resolve( this );
			}
			
			if ( link_methods->readlink )
			{
				return ResolvePathname( link_methods->readlink( this ), owner() );
			}
		}
		
		return Self();
	}
	
	void FSTree::SymLink( const plus::string& target ) const
	{
		const link_method_set* link_methods;
		
		if ( its_methods  &&  (link_methods = its_methods->link_methods) )
		{
			if ( link_methods->symlink )
			{
				link_methods->symlink( this, target );
				
				return;
			}
		}
		
		p7::throw_errno( EINVAL );
	}
	
	IOPtr FSTree::Open( OpenFlags flags, mode_t mode ) const
	{
		const data_method_set* data_methods;
		
		if ( its_methods  &&  (data_methods = its_methods->data_methods) )
		{
			if ( data_methods->open )
			{
				return data_methods->open( this, flags, mode );
			}
		}
		
		bool creating  = flags & O_CREAT;
		bool excluding = flags & O_EXCL;
		
		if ( creating && excluding )
		{
			p7::throw_errno( EPERM );
		}
		
		throw p7::errno_t( ENOENT );  // Assume read attempt if no mode
	}
	
	shared_exec_handle FSTree::GetExecutable() const
	{
		const file_method_set* file_methods;
		
		if ( its_methods  &&  (file_methods = its_methods->file_methods) )
		{
			if ( file_methods->loadexec )
			{
				return file_methods->loadexec( this );
			}
		}
		
		throw p7::errno_t( ENOEXEC );
	}
	
	
	IOPtr FSTree::OpenDirectory() const
	{
		const dir_method_set* dir_methods;
		
		if ( its_methods  &&  (dir_methods = its_methods->dir_methods) )
		{
			if ( dir_methods->opendir )
			{
				return dir_methods->opendir( this );
			}
		}
		
		return new VirtualDirHandle( Self() );
	}
	
	IOPtr FSTree::ChangeToDirectory() const
	{
		const dir_method_set* dir_methods;
		
		if ( its_methods  &&  (dir_methods = its_methods->dir_methods) )
		{
			if ( dir_methods->chdir )
			{
				return dir_methods->chdir( this );
			}
		}
		
		return OpenDirectory();
	}
	
	void FSTree::CreateDirectory( mode_t mode ) const
	{
		const dir_method_set* dir_methods;
		
		if ( its_methods  &&  (dir_methods = its_methods->dir_methods) )
		{
			if ( dir_methods->mkdir )
			{
				dir_methods->mkdir( this, mode );
				
				return;
			}
		}

		p7::throw_errno( EPERM );
	}
	
	FSTreePtr FSTree::Lookup( const plus::string& name, const FSTree* parent ) const
	{
		if ( name == "." )
		{
			return Self();
		}
		else if ( name == ".." )
		{
			return Parent();
		}
		
		if ( parent == NULL )
		{
			parent = this;
		}
		
		const dir_method_set* dir_methods;
		
		if ( its_methods  &&  (dir_methods = its_methods->dir_methods) )
		{
			if ( dir_methods->lookup )
			{
				return dir_methods->lookup( this, name, parent );
			}
		}
		
		throw p7::errno_t( ENOTDIR );
	}
	
	void FSTree::IterateIntoCache( FSTreeCache& cache ) const
	{
		const dir_method_set* dir_methods;
		
		if ( its_methods  &&  (dir_methods = its_methods->dir_methods) )
		{
			if ( dir_methods->listdir )
			{
				dir_methods->listdir( this, cache );
				
				return;
			}
		}
		
		throw p7::errno_t( ENOTDIR );
	}
	
	void FSTree::Attach( const FSTreePtr& target ) const
	{
		const file_method_set* file_methods;
		
		if ( its_methods  &&  (file_methods = its_methods->file_methods) )
		{
			if ( file_methods->attach )
			{
				file_methods->attach( this, target.get() );
				
				return;
			}
		}
		
		p7::throw_errno( EINVAL );
	}
	
}

