/*	=========
 *	FSTree.cc
 *	=========
 */

#include "Genie/FS/FSTree.hh"

// POSIX
#include "fcntl.h"
#include "sys/stat.h"

// Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen
#include "Nitrogen/OSUtils.h"

// POSeven
#include "POSeven/Errno.hh"

// BitsAndBytes
#include "HexStrings.hh"

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
	
	inline std::string NameFromPtr( const FSTree* ptr )
	{
		return BitsAndBytes::EncodeAsHex( ptr );
	}
	
	FSTree::FSTree( const FSTreePtr&    parent,
	                const std::string&  name ) : itsParent( parent ),
	                                             itsName  ( name[0] == '/' ? NameFromPtr( this )
	                                                                       : name )
	{
	}
	
	FSTree::~FSTree()
	{
	}
	
	std::string FSTree::Pathname() const
	{
		std::string pathname = Name();
		
		FSTreePtr tree = Parent();
		
		if ( tree.get() == NULL )
		{
			// Anonymous files have null parents.
			goto name_only;
		}
		
		while ( true )
		{
			const std::string& name = tree->Name();
			
			pathname = name + "/" + pathname;
			
			// Root dir has empty name
			
			if ( name.empty() )
			{
				break;
			}
			
			tree = tree->Parent();
		}
		
	name_only:
		
		return pathname;
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
	
	FSTreePtr FSTree::Parent() const
	{
		return itsParent;
	}
	
	ino_t FSTree::Inode() const
	{
		return 0;
	}
	
	ino_t FSTree::ParentInode() const
	{
		return Parent()->Inode();
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
		
		sb.st_ino = Inode();
		
		sb.st_mode = FileTypeMode() | FilePermMode();
		
		sb.st_nlink = 1;
		
		// fstatat() or fstat() will fill in size for regular files
		
		if ( !S_ISREG( sb.st_mode ) )
		{
			sb.st_size = 0;
		}
		
		sb.st_blksize = 4096;
		
		sb.st_atime = now;
		sb.st_mtime = now;
		sb.st_ctime = now;
	}
	
	void FSTree::ChangeMode( mode_t /*mode*/ ) const
	{
		p7::throw_errno( EPERM );
	}
	
	void FSTree::SetTimes() const
	{
		p7::throw_errno( EPERM );
	}
	
	void FSTree::SetTimes( const struct timespec times[2] ) const
	{
		p7::throw_errno( EPERM );
	}
	
	void FSTree::Delete() const
	{
		p7::throw_errno( EPERM );
	}
	
	void FSTree::Rename( const FSTreePtr& destination ) const
	{
		p7::throw_errno( EINVAL );
	}
	
	void FSTree::HardLink( const FSTreePtr& destination ) const
	{
		p7::throw_errno( EINVAL );
	}
	
	void FSTree::CopyFile( const FSTreePtr& destination ) const
	{
		p7::throw_errno( EINVAL );
	}
	
	off_t FSTree::GetEOF() const
	{
		// Errors are meaningless here since there's no POSIX call specifically
		// to get EOF (as there is truncate() for setting EOF).  We only care so
		// we can populate stat::st_size.  Just return zero.
		
		return 0;
	}
	
	void FSTree::SetEOF( off_t /*length*/ ) const
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
	
	void FSTree::SymLink( const std::string& target ) const
	{
		p7::throw_errno( EINVAL );
	}
	
	boost::shared_ptr< IOHandle > FSTree::Open( OpenFlags flags, mode_t mode ) const
	{
		const bool following = (flags & O_NOFOLLOW) == 0;
		
		if ( following  &&  IsLink() )
		{
			return ResolveLink()->Open( flags, mode );
		}
		
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
		if ( IsLink() )
		{
			return ResolveLink()->OpenDirectory();
		}
		
		if ( !IsDirectory() )
		{
			p7::throw_errno( Exists() ? ENOTDIR : ENOENT );
		}
		
		return boost::shared_ptr< IOHandle >( new DirHandle( Self() ) );
	}
	
	boost::shared_ptr< IOHandle > FSTree::ChangeToDirectory() const
	{
		return OpenDirectory();
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
	
	FSTreePtr FSTree::ResolvePath( const char*& begin, const char* end ) const
	{
		if ( begin == end )
		{
			return Self();
		}
		
		ASSERT( begin < end );
		
		const char* slash = std::find( begin, end, '/' );
		
		const bool done = slash == end;
		
		std::string name( begin, slash );
		
		begin = slash + !done;
		
		return Lookup( name );
	}
	
	void FSTree::Attach( const FSTreePtr& target ) const
	{
		p7::throw_errno( EINVAL );
	}
	
}

