/*	=========
 *	FSTree.cc
 *	=========
 */

#include "Genie/FileSystem/FSTree.hh"

// POSIX
#include "fcntl.h"
#include "sys/stat.h"

// Nucleus
//#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace P7 = POSeven;
	
	
	std::string FSTree::Pathname() const
	{
		std::string name = Name();
		
		// Root dir has empty name
		
		if ( name.empty() )
		{
			return "/";
		}
		
		std::string parentPathname = Parent()->Pathname();
		
		if ( parentPathname != "/" )
		{
			parentPathname += "/";
		}
		
		return parentPathname + Name();
	}
	
	
	bool FSTree::Exists() const
	{
		// Reasonable default -- most items do or they wouldn't have FSTrees.
		return true;
	}
	
	bool FSTree::IsFile() const
	{
		// Reasonable default -- most virtual dirs are bottlenecked through a superclass
		return true;
	}
	
	bool FSTree::IsDirectory() const
	{
		return false;
	}
	
	bool FSTree::IsLink() const
	{
		return false;
	}
	
	std::string FSTree::Name() const
	{
		P7::ThrowErrno( ENOENT );
		
		return "";  // Not reached
	}
	
	FSSpec FSTree::GetFSSpec() const
	{
		P7::ThrowErrno( EXDEV );
		
		return FSSpec();  // Not reached
	}
	
	void FSTree::Stat( struct ::stat& sb ) const
	{
		if ( ! Exists() )
		{
			P7::ThrowErrno( ENOENT );
		}
		
		time_t now = time( NULL );
		
		mode_t mode = IsDirectory() ? S_IFDIR | S_IRUSR | S_IWUSR | S_IXUSR
		            : IsLink()      ? S_IFLNK | S_IRUSR | S_IWUSR | S_IXUSR
		            :                 S_IFREG | S_IRUSR;
		
		sb.st_mode = mode;
		
		sb.st_nlink = 1;
		
		sb.st_blksize = 4096;
		
		sb.st_atime = now;
		sb.st_mtime = now;
		sb.st_ctime = now;
	}
	
	void FSTree::ChangeMode( mode_t mode ) const
	{
		P7::ThrowErrno( EPERM );
	}
	
	void FSTree::Delete() const
	{
		P7::ThrowErrno( EPERM );
	}
	
	std::string FSTree::ReadLink() const
	{
		P7::ThrowErrno( EINVAL );
		
		return std::string();
	}
	
	FSTreePtr FSTree::ResolveLink() const
	{
		P7::ThrowErrno( EINVAL );
		
		return FSTreePtr();
	}
	
	boost::shared_ptr< IOHandle > FSTree::Open( OpenFlags flags, mode_t mode ) const
	{
		bool creating  = flags & O_CREAT;
		bool excluding = flags & O_EXCL;
		
		if ( creating && excluding )
		{
			P7::ThrowErrno( EPERM );
		}
		
		return Open( flags );
	}
	
	boost::shared_ptr< IOHandle > FSTree::Open( OpenFlags flags ) const
	{
		P7::ThrowErrno( ENOENT );  // Assume read attempt if no mode
		
		// Not reached
		return boost::shared_ptr< IOHandle >();
	}
	
	void FSTree::Exec( const char* const argv[], const char* const envp[] ) const
	{
		P7::ThrowErrno( ENOEXEC );
	}
	
	
	void FSTree::CreateDirectory( mode_t mode ) const
	{
		P7::ThrowErrno( EPERM );
	}
	
	FSTreePtr FSTree::Lookup( const std::string& name ) const
	{
		P7::ThrowErrno( ENOENT );
		
		// Not reached
		return FSTreePtr();
	}
	
	FSIteratorPtr FSTree::Iterate() const
	{
		P7::ThrowErrno( ENOENT );
		
		// Not reached
		return FSIteratorPtr();
	}
	
}

