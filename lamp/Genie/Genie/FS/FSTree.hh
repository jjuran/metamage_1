/*	=========
 *	FSTree.hh
 *	=========
 */

#ifndef GENIE_FILESYSTEM_FSTREE_HH
#define GENIE_FILESYSTEM_FSTREE_HH

// Standard C++
#include <string>

// Debug
#include "debug/boost_assert.hh"

// boost
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

// Genie
#include "Genie/Exec/MainEntry.hh"

// time.h
struct timespec;

// <sys/stat.h>
struct stat;


namespace Genie
{
	
	class IOHandle;
	
	typedef int OpenFlags;
	
	class FSTree;
	class FSIterator;
	
	typedef boost::shared_ptr< FSTree const > FSTreePtr;
	typedef boost::shared_ptr< FSIterator   > FSIteratorPtr;
	
	
	const FSTreePtr& FSRoot();
	
	
	struct FSNode
	{
		ino_t        inode;
		std::string  name;
		
		FSNode()
		{
		}
		
		FSNode( ino_t i, const std::string& n ) : inode( i ), name( n )
		{
		}
		
		~FSNode();
	};
	
	class FSIterator
	{
		public:
			virtual ~FSIterator();
			
			virtual FSNode Get() const = 0;
			
			virtual void Advance() = 0;
			
			virtual void Rewind() = 0;
			
			virtual void Seek( off_t index ) = 0;
			
			virtual off_t Tell() const = 0;
	};
	
	
	class FSTree : public boost::enable_shared_from_this< FSTree >
	{
		private:
			FSTreePtr    itsParent;
			std::string  itsName;
		
		public:
			FSTree();
			
			FSTree( const FSTreePtr&    parent,
			        const std::string&  name );
			
			virtual ~FSTree();
			
			// General methods
			
			virtual bool Exists() const;
			virtual bool IsFile() const;
			virtual bool IsDirectory() const;
			virtual bool IsLink() const;
			virtual bool IsPipe() const;
			virtual bool IsAnonymous() const;
			
			const FSTreePtr& ParentRef() const  { return itsParent; }
			
			const std::string& Name() const  { return itsName; }
			
			std::string Pathname() const;
			
			FSTreePtr Self() const  { return shared_from_this(); }
			
			virtual FSTreePtr Parent() const;
			
			virtual ino_t Inode() const;
			virtual ino_t ParentInode() const;
			
			virtual mode_t FileTypeMode() const;
			virtual mode_t FilePermMode() const;
			
			virtual void Stat( struct ::stat& sb ) const;
			
			virtual void ChangeMode( mode_t mode ) const;
			
			virtual void SetTimes() const;
			
			virtual void SetTimes( const struct timespec times[2] ) const;
			
			virtual void Delete() const;
			
			virtual void Rename( const FSTreePtr& destination ) const;
			
			virtual void HardLink( const FSTreePtr& destination ) const;
			
			// File methods
			
			virtual void CopyFile( const FSTreePtr& destination ) const;
			
			virtual off_t GetEOF() const;
			virtual void  SetEOF( off_t length ) const;
			
			virtual std::string ReadLink() const;
			virtual FSTreePtr ResolveLink() const;
			
			virtual void SymLink( const std::string& target ) const;
			
			virtual boost::shared_ptr< IOHandle > Open( OpenFlags flags, mode_t mode ) const;
			virtual boost::shared_ptr< IOHandle > Open( OpenFlags flags              ) const;
			
			virtual MainEntry GetMainEntry() const;
			
			// Directory methods
			
			virtual boost::shared_ptr< IOHandle > OpenDirectory() const;
			
			virtual boost::shared_ptr< IOHandle > ChangeToDirectory() const;
			
			virtual void CreateDirectory( mode_t mode ) const;
			
			virtual FSTreePtr Lookup( const std::string& name ) const;
			
			virtual FSIteratorPtr Iterate() const;
			
			// Internal methods
			
			virtual FSTreePtr ResolvePath( const char*& begin, const char* end ) const;
			
			virtual void Attach( const FSTreePtr& target ) const;
	};
	
}

#endif

