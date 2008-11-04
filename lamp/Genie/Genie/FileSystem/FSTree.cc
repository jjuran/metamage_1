/*	=========
 *	FSTree.cc
 *	=========
 */

#include "Genie/FileSystem/FSTree.hh"

// POSIX
#include "fcntl.h"
#include "sys/stat.h"

// Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen
#include "Nitrogen/OSUtils.h"

// POSeven
#include "POSeven/Errno.hh"

// TimeOff
#include "TimeOff.hh"

// Genie
#include "Genie/IO/Directory.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	FSNode::~FSNode()
	{
	}
	
	FSIterator::~FSIterator()
	{
	}
	
	FSTree::FSTree() : itsParent(), itsName()
	{
	}
	
	FSTree::FSTree( const FSTreePtr&    parent,
	                const std::string&  name ) : itsParent( parent ),
	                                             itsName  ( name   )
	{
	}
	
	FSTree::~FSTree()
	{
	}
	
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
		
		return parentPathname += name;
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
	
	bool FSTree::IsPipe() const
	{
		return false;
	}
	
	bool FSTree::IsAnonymous() const
	{
		return false;
	}
	
	std::string FSTree::Name() const
	{
		return itsName;
	}
	
	FSTreePtr FSTree::Parent() const
	{
		return itsParent;
	}
	
	FSSpec FSTree::GetFSSpec( bool forCreation ) const
	{
		p7::throw_errno( EXDEV );
		
		return FSSpec();  // Not reached
	}
	
	mode_t FSTree::FileTypeMode() const
	{
		mode_t type = IsDirectory() ? S_IFDIR
		            : IsLink()      ? S_IFLNK
		            : IsPipe()      ? S_IFIFO
		            :                 S_IFREG;
		
		return type;
	}
	
	mode_t FSTree::FilePermMode() const
	{
		mode_t perm = IsDirectory() ? S_IRUSR | S_IWUSR | S_IXUSR
		            : IsLink()      ? S_IRUSR | S_IWUSR | S_IXUSR
		            : IsPipe()      ? S_IRUSR | S_IWUSR
		            :                 S_IRUSR;
		
		return perm;
	}
	
	void FSTree::Stat( struct ::stat& sb ) const
	{
		if ( ! Exists() )
		{
			p7::throw_errno( ENOENT );
		}
		
		const unsigned long timeDiff = TimeOff::MacToUnixTimeDifference();
		
		time_t now = N::GetDateTime() - timeDiff;
		
		sb.st_mode = FileTypeMode() | FilePermMode();
		
		sb.st_nlink = 1;
		
		try
		{
			sb.st_size = IsDirectory() ? 0 : GetEOF();
		}
		catch ( ... ) {}
		
		sb.st_blksize = 4096;
		
		sb.st_atime = now;
		sb.st_mtime = now;
		sb.st_ctime = now;
	}
	
	void FSTree::ChangeMode( mode_t /*mode*/ ) const
	{
		p7::throw_errno( EPERM );
	}
	
	void FSTree::SetUTime( const struct utimbuf* utime_buf ) const
	{
		p7::throw_errno( EPERM );
	}
	
	void FSTree::Delete() const
	{
		p7::throw_errno( EPERM );
	}
	
	off_t FSTree::GetEOF() const
	{
		// This confuses MWCPPC when optimizing:
		//p7::throw_errno( IsDirectory() ? EISDIR : EINVAL );
		// internal compiler error: File: 'PCodeUtilities.c' Line: 80
		
		if ( IsDirectory() )
		{
			p7::throw_errno( EISDIR );
		}
		else
		{
			p7::throw_errno( EINVAL );
		}
		
		return 0;  // Not reached
	}
	
	void FSTree::SetEOF( off_t /*length*/ ) const
	{
		if ( IsDirectory() )
		{
			p7::throw_errno( EISDIR );
		}
		else
		{
			p7::throw_errno( EINVAL );
		}
	}
	
	std::string FSTree::ReadLink() const
	{
		p7::throw_errno( EINVAL );
		
		return std::string();
	}
	
	FSTreePtr FSTree::ResolveLink() const
	{
		p7::throw_errno( EINVAL );
		
		return FSTreePtr();
	}
	
	boost::shared_ptr< IOHandle > FSTree::Open( OpenFlags flags, mode_t /*mode*/ ) const
	{
		bool creating  = flags & O_CREAT;
		bool excluding = flags & O_EXCL;
		
		if ( creating && excluding )
		{
			p7::throw_errno( EPERM );
		}
		
		return Open( flags );
	}
	
	boost::shared_ptr< IOHandle > FSTree::Open( OpenFlags /*flags*/ ) const
	{
		p7::throw_errno( ENOENT );  // Assume read attempt if no mode
		
		// Not reached
		return boost::shared_ptr< IOHandle >();
	}
	
	MainEntry FSTree::GetMainEntry() const
	{
		p7::throw_errno( ENOEXEC );
		
		// Not reached
		return MainEntry();
	}
	
	
	boost::shared_ptr< IOHandle > FSTree::OpenDirectory() const
	{
		if ( !IsDirectory() )
		{
			p7::throw_errno( ENOTDIR );
		}
		
		return boost::shared_ptr< IOHandle >( new DirHandle( shared_from_this() ) );
	}
	
	void FSTree::CreateDirectory( mode_t /*mode*/ ) const
	{
		p7::throw_errno( EPERM );
	}
	
	FSTreePtr FSTree::Lookup( const std::string& /*name*/ ) const
	{
		p7::throw_errno( ENOENT );
		
		// Not reached
		return FSTreePtr();
	}
	
	FSIteratorPtr FSTree::Iterate() const
	{
		p7::throw_errno( ENOENT );
		
		// Not reached
		return FSIteratorPtr();
	}
	
}

