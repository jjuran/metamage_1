/*	=======
 *	Base.hh
 *	=======
 */

#ifndef GENIE_IO_BASE_HH
#define GENIE_IO_BASE_HH

// Debug
#include "debug/boost_assert.hh"

// boost
#include <boost/intrusive_ptr.hpp>

// plus
#include "plus/ref_count.hh"

// vfs
#include "vfs/handle_ptr.hh"
#include "vfs/node_ptr.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"
#include "Genie/IO/IOPtr.hh"
#include "Genie/mmap/memory_mapping.hh"


namespace Genie
{
	
	enum
	{
		kPollRead   = 1,
		kPollWrite  = 2,
		kPollExcept = 4
	};
	
}

namespace vfs
{
	
	using Genie::memory_mapping;
	
	/*
		Each file descriptor refers to an I/O handle.
		Several file descriptors may share the same I/O handle.
		Each I/O handle has a type.
		I/O handle types are arranged in an inheritence tree, with the base type
		at the root node, and actual (non-abstract) types as leaf nodes.
		Each type (abstract as well as leaf) has a node in the tree.
		
	*/
	
	class handle : public plus::ref_count< handle >
	{
		private:
			int itsOpenFlags;
			
			virtual handle* Next() const  { return NULL; }
			
			// non-copyable
			handle           ( const handle& );
			handle& operator=( const handle& );
		
		public:
			typedef bool (handle::*Test)() const;
			
			handle( int flags );
			
			virtual ~handle();
			
			virtual bool IsStream     () const  { return false; }
			virtual bool IsRegularFile() const  { return false; }
			virtual bool IsSocket     () const  { return false; }
			virtual bool IsTerminal   () const  { return false; }
			virtual bool IsDirectory  () const  { return false; }
			
			handle* GetBaseForCast( Test test );
			
			int GetFlags() const  { return itsOpenFlags; }
			
			void SetFlags( int flags )  { itsOpenFlags = flags; }
			
			virtual handle_ptr Clone();
			
			virtual void Attach( const handle_ptr& target );
			
			virtual node_ptr GetFile();
			
			virtual void IOCtl( unsigned long request, int* argp );
			
			virtual boost::intrusive_ptr< memory_mapping > Map( size_t length, off_t offset );
			
	};
	
}

namespace Genie
{
	
	typedef vfs::handle IOHandle;
	
}

#endif

