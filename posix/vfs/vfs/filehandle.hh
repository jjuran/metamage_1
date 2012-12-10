/*
	filehandle.hh
	-------------
*/

#ifndef VFS_FILEHANDLE_HH
#define VFS_FILEHANDLE_HH

// plus
#include "plus/ref_count.hh"

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/memory_mapping_ptr.hh"
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	/*
		Each file descriptor refers to an I/O handle.
		Several file descriptors may share the same I/O handle.
		Each I/O handle has a type.
		I/O handle types are arranged in an inheritence tree, with the base type
		at the root node, and actual (non-abstract) types as leaf nodes.
		Each type (abstract as well as leaf) has a node in the tree.
	*/
	
	class filehandle : public plus::ref_count< filehandle >
	{
		private:
			int itsOpenFlags;
			
			virtual filehandle* Next() const  { return NULL; }
			
			// non-copyable
			filehandle           ( const filehandle& );
			filehandle& operator=( const filehandle& );
		
		public:
			typedef bool (filehandle::*Test)() const;
			
			filehandle( int flags );
			
			virtual ~filehandle();
			
			virtual bool IsStream     () const  { return false; }
			virtual bool IsRegularFile() const  { return false; }
			virtual bool IsSocket     () const  { return false; }
			virtual bool IsTerminal   () const  { return false; }
			virtual bool IsDirectory  () const  { return false; }
			
			filehandle* GetBaseForCast( Test test );
			
			int GetFlags() const  { return itsOpenFlags; }
			
			void SetFlags( int flags )  { itsOpenFlags = flags; }
			
			virtual filehandle_ptr Clone();
			
			virtual void Attach( const filehandle_ptr& target );
			
			virtual node_ptr GetFile();
			
			virtual void IOCtl( unsigned long request, int* argp );
			
			virtual memory_mapping_ptr Map( size_t length, int prot, int flags, off_t offset );
	};
	
}

#endif

