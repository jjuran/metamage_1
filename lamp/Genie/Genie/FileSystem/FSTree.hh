/*	=========
 *	FSTree.hh
 *	=========
 */

#ifndef GENIE_FILESYSTEM_FSTREE_HH
#define GENIE_FILESYSTEM_FSTREE_HH

// Standard C++
#include <string>

// boost
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

// Files.h
struct FSSpec;

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
	
	
	FSTreePtr FSRoot();
	
	FSTreePtr FSTreeFromFSSpec( const FSSpec& item );
	
	std::string GetFSTreePathname( FSTreePtr node );
	
	
	struct FSNode
	{
		std::string  name;
		FSTreePtr    tree;
		
		FSNode()  {}
		
		FSNode( const std::string& name, const FSTreePtr& tree ) : name( name ), tree( tree )  {}
	};
	
	class FSIterator
	{
		public:
			virtual ~FSIterator()  {}
			
			virtual FSNode Get() const = 0;
			
			virtual void Advance() = 0;
			
			virtual void Rewind() = 0;
			
			virtual void Seek( off_t index ) = 0;
			
			virtual off_t Tell() const = 0;
	};
	
	
	class FSTree : public boost::enable_shared_from_this< const FSTree >
	{
		public:
			virtual ~FSTree()  {}
			
			// General methods
			
			virtual bool Exists() const;
			virtual bool IsFile() const;
			virtual bool IsDirectory() const;
			virtual bool IsLink() const;
			
			virtual std::string Name() const;
			
			FSTreePtr Self() const  { return shared_from_this(); }
			
			virtual FSTreePtr Parent() const = 0;
			
			virtual FSSpec GetFSSpec() const;
			
			virtual void Stat( struct ::stat& sb ) const;
			
			virtual void ChangeMode( mode_t mode ) const;
			
			virtual void Delete() const;
			
			// File methods
			
			virtual std::string ReadLink() const;
			virtual FSTreePtr ResolveLink() const;
			
			virtual boost::shared_ptr< IOHandle > Open( OpenFlags flags, mode_t mode ) const;
			virtual boost::shared_ptr< IOHandle > Open( OpenFlags flags              ) const;
			
			virtual void Exec( const char* const argv[], const char* const envp[] ) const;
			
			// Directory methods
			
			virtual void CreateDirectory( mode_t mode ) const;
			
			virtual FSTreePtr Lookup( const std::string& name ) const;
			
			virtual FSIteratorPtr Iterate() const;
	};
	
}

#endif

