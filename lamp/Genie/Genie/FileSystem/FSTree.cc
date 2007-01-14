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
	
	
	std::string GetFSTreePathname( FSTreePtr node )
	{
		std::string name = node->Name();
		
		if ( name.empty() )
		{
			return "/";
		}
		
		std::string result = name;
		
		// Root dir has empty name
		
		while ( result[0] != '/' )
		{
			node = node->Parent();
			name = node->Name();
			
			result = name + "/" + result;
		}
		
		return result;
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
		
		mode_t mode = IsDirectory() ? S_IFDIR | S_IRUSR | S_IWUSR | S_IXUSR
		            : IsLink()      ? S_IFLNK | S_IRUSR | S_IWUSR | S_IXUSR
		            :                 S_IFREG | S_IRUSR;
		
		sb.st_mode = mode;
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

