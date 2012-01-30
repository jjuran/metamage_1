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
		
		public:
			FSTree();
			
			FSTree( const FSTreePtr&        parent,
			        const plus::string&     name,
			        mode_t                  mode,
			        const node_method_set*  methods = NULL );
			
			virtual ~FSTree();
			
			// General methods
			
			bool Exists() const;
			bool IsFile() const;
			bool IsDirectory() const;
			bool IsLink() const;
			
			const FSTreePtr& ParentRef() const  { return itsParent; }
			
			const plus::string& Name() const  { return itsName; }
			
			mode_t FileMode() const  { return itsMode; }
			
			plus::string Pathname() const;
			
			FSTreePtr Self() const  { return FSTreePtr( this ); }
			
			FSTreePtr Parent() const;
			
			virtual ino_t Inode() const;
			virtual ino_t ParentInode() const;
			
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
			
			virtual plus::string ReadLink() const;
			virtual FSTreePtr ResolveLink() const;
			
			void SymLink( const plus::string& target ) const;
			
			virtual IOPtr Open( OpenFlags flags, mode_t mode ) const;
			virtual IOPtr Open( OpenFlags flags              ) const;
			
			virtual shared_exec_handle GetExecutable() const;
			
			// Directory methods
			
			virtual IOPtr OpenDirectory() const;
			
			virtual IOPtr ChangeToDirectory() const;
			
			virtual void CreateDirectory( mode_t mode ) const;
			
			FSTreePtr Lookup( const plus::string& name, const FSTree* parent = NULL ) const;
			
			virtual FSTreePtr Lookup_Child( const plus::string& name, const FSTree* parent ) const;
			
			virtual void IterateIntoCache( FSTreeCache& cache ) const;
			
			// Internal methods
			
			virtual FSTreePtr ResolvePath( const char*& begin, const char* end ) const;
			
			virtual void Attach( const FSTreePtr& target ) const;
	};
	
}

#endif

