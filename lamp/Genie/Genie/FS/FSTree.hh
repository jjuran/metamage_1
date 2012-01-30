/*	=========
 *	FSTree.hh
 *	=========
 */

#ifndef GENIE_FILESYSTEM_FSTREE_HH
#define GENIE_FILESYSTEM_FSTREE_HH

// POSIX
#include <sys/types.h>

// Debug
#include "debug/boost_assert.hh"

// plus
#include "plus/ref_count.hh"
#include "plus/string.hh"

// Genie
#include "Genie/code/shared_exec_handle.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/FSTreePtr.hh"
#include "Genie/IO/IOPtr.hh"
#include "Genie/IO/OpenFlags.hh"

// time.h
struct timespec;

// <sys/stat.h>
struct stat;


namespace Genie
{
	
	typedef void (*node_destructor)( const FSTree* );
	
	struct node_method_set;
	
	
	extern const FSTreePtr null_FSTreePtr;
	
	const FSTreePtr& FSRoot();
	
	
	class FSTree : public plus::ref_count< FSTree >
	{
		private:
			FSTreePtr     itsParent;
			plus::string  itsName;
			mode_t        itsMode;
			
			const node_method_set*  its_methods;
			
			void*  its_extra;
			
			node_destructor  its_destructor;
			
		private:
			// non-copyable
			FSTree           ( const FSTree& );
			FSTree& operator=( const FSTree& );
		
		public:
			FSTree();
			
			FSTree( const FSTreePtr&        parent,
			        const plus::string&     name,
			        mode_t                  mode,
			        const node_method_set*  methods = NULL,
			        std::size_t             n_extra = 0,
			        node_destructor         dtor    = NULL );
			
			virtual ~FSTree();
			
			// General methods
			
			const FSTree* owner() const  { return itsParent.get(); }
			
			const plus::string& name() const  { return itsName; }
			const plus::string& Name() const  { return itsName; }
			
			mode_t FileMode() const  { return itsMode; }
			
			void* extra() const  { return its_extra; }
			
			node_destructor destructor() const  { return its_destructor; }
			
			plus::string Pathname() const;
			
			FSTreePtr Self() const  { return FSTreePtr( this ); }
			
			FSTreePtr Parent() const;
			
			ino_t Inode() const;
			ino_t ParentInode() const;
			
			void Stat( struct ::stat& sb ) const;
			
			void ChangeMode( mode_t mode ) const;
			
			void SetTimes() const;
			
			void SetTimes( const struct timespec times[2] ) const;
			
			void Delete() const;
			
			void Rename( const FSTreePtr& destination ) const;
			
			void HardLink( const FSTreePtr& destination ) const;
			
			// File methods
			
			void CopyFile( const FSTreePtr& destination ) const;
			
			off_t GetEOF() const;
			void  SetEOF( off_t length ) const;
			
			plus::string ReadLink() const;
			FSTreePtr ResolveLink() const;
			
			void SymLink( const plus::string& target ) const;
			
			IOPtr Open( OpenFlags flags, mode_t mode ) const;
			
			shared_exec_handle GetExecutable() const;
			
			// Directory methods
			
			IOPtr OpenDirectory() const;
			
			IOPtr ChangeToDirectory() const;
			
			void CreateDirectory( mode_t mode ) const;
			
			FSTreePtr Lookup( const plus::string& name, const FSTree* parent = NULL ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
			
			// Internal methods
			
			FSTreePtr ResolvePath( const char*& begin, const char* end ) const;
			
			void Attach( const FSTreePtr& target ) const;
	};
	
}

#endif

