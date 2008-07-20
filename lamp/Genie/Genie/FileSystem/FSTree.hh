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

// Genie
#include "Genie/Exec/MainEntry.hh"

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
	
	
	struct FSNode
	{
		std::string  name;
		FSTreePtr    tree;
		
		FSNode()  {}
		
		FSNode( const std::string& name, const FSTreePtr& tree ) : name( name ), tree( tree )  {}
		
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
			FSTree()
			{
			}
			
			FSTree( const FSTreePtr& parent ) : itsParent( parent )
			{
			}
			
			FSTree( const FSTreePtr&    parent,
			        const std::string&  name ) : itsParent( parent ),
			                                     itsName  ( name   )
			{
			}
			
			virtual ~FSTree();
			
			void Init()  {}  // never called virtually
			
			// General methods
			
			virtual bool Exists() const;
			virtual bool IsFile() const;
			virtual bool IsDirectory() const;
			virtual bool IsLink() const;
			virtual bool IsPipe() const;
			virtual bool IsAnonymous() const;
			
			virtual std::string Name() const;
			virtual std::string Pathname() const;
			
			FSTreePtr Self() const  { return shared_from_this(); }
			
			virtual FSTreePtr Parent() const;
			
			virtual FSSpec GetFSSpec( bool forCreation = false ) const;
			
			virtual mode_t FileTypeMode() const;
			virtual mode_t FilePermMode() const;
			
			virtual void Stat( struct ::stat& sb ) const;
			
			virtual void ChangeMode( mode_t mode ) const;
			
			virtual void Delete() const;
			
			// File methods
			
			virtual off_t GetEOF() const;
			virtual void  SetEOF( off_t length ) const;
			
			virtual std::string ReadLink() const;
			virtual FSTreePtr ResolveLink() const;
			
			virtual boost::shared_ptr< IOHandle > Open( OpenFlags flags, mode_t mode ) const;
			virtual boost::shared_ptr< IOHandle > Open( OpenFlags flags              ) const;
			
			virtual MainEntry GetMainEntry() const;
			
			// Directory methods
			
			boost::shared_ptr< IOHandle > OpenDirectory() const;
			
			virtual void CreateDirectory( mode_t mode ) const;
			
			virtual FSTreePtr Lookup( const std::string& name ) const;
			
			virtual FSIteratorPtr Iterate() const;
	};
	
	template < class Tree >
	inline FSTreePtr MakeFSTree( Tree* tree )
	{
		FSTreePtr result( tree );
		
		tree->Init();
		
		return result;
	}
	
}

#endif

