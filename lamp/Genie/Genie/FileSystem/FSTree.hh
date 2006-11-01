/*	=========
 *	FSTree.hh
 *	=========
 */

#ifndef GENIE_FILESYSTEM_FSTREE_HH
#define GENIE_FILESYSTEM_FSTREE_HH

// Standard C++
#include <string>

// boost
#include <boost/shared_ptr.hpp>

// Files.h
struct FSSpec;

// <sys/stat.h>
struct stat;


namespace Genie
{
	
	class FSTree;
	class FSIterator;
	
	typedef boost::shared_ptr< FSTree const > FSTreePtr;
	typedef boost::shared_ptr< FSIterator   > FSIteratorPtr;
	
	
	FSTreePtr FSRoot();
	
	FSTreePtr FSTreeFromFSSpec( const FSSpec& item );
	
	
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
	
	
	class FSTree
	{
		public:
			virtual ~FSTree()  {}
			
			virtual void Stat( struct ::stat& sb ) const = 0;
			
			virtual FSTreePtr Lookup( const std::string& name ) const = 0;
			
			virtual FSIteratorPtr Iterate() const = 0;
	};
	
}

#endif

