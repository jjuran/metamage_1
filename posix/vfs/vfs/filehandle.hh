/*
	filehandle.hh
	-------------
*/

#ifndef VFS_FILEHANDLE_HH
#define VFS_FILEHANDLE_HH

// plus
#include "plus/ref_count.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	struct filehandle_method_set;
	struct bstore_method_set;
	struct socket_method_set;
	struct terminal_method_set;
	
	/*
		Each file descriptor refers to an I/O handle.
		Several file descriptors may share the same I/O handle.
		Each I/O handle has a type.
		I/O handle types are arranged in an inheritence tree, with the base type
		at the root node, and actual (non-abstract) types as leaf nodes.
		Each type (abstract as well as leaf) has a node in the tree.
	*/
	
	class filehandle;
	
	typedef void (*filehandle_destructor)( filehandle* );
	
	class filehandle : public plus::ref_count< filehandle >
	{
		private:
			off_t                         its_mark;
			node_ptr                      its_file;
			int                           its_flags;
			const filehandle_method_set*  its_methods;
			void*                         its_extra;
			filehandle_destructor         its_destructor;
			
			// non-copyable
			filehandle           ( const filehandle& );
			filehandle& operator=( const filehandle& );
		
		public:
			typedef bool (filehandle::*Test)() const;
			
			filehandle( int                           flags,
			            const filehandle_method_set*  methods = NULL,
			            std::size_t                   n_extra = 0,
			            filehandle_destructor         dtor    = NULL );
			
			filehandle( const node*                   file,
			            int                           flags,
			            const filehandle_method_set*  methods = NULL,
			            std::size_t                   n_extra = 0,
			            filehandle_destructor         dtor    = NULL );
			
			virtual ~filehandle();
			
			virtual bool IsStream     () const  { return false; }
			virtual bool IsDirectory  () const  { return false; }
			
			filehandle* GetBaseForCast( Test test );
			
			int get_flags() const  { return its_flags; }
			
			void set_flags( int flags )  { its_flags = flags; }
			
			off_t get_mark() const  { return its_mark; }
			
			off_t set_mark( off_t mark )  { return its_mark = mark; }
			
			ssize_t advance_mark( ssize_t delta )  { its_mark += delta;  return delta; }
			
			const filehandle_method_set* methods() const  { return its_methods; }
			
			const bstore_method_set&   bstore_methods  () const;
			const socket_method_set&   socket_methods  () const;
			const terminal_method_set& terminal_methods() const;
			
			void* extra() const  { return its_extra; }
			
			filehandle_destructor destructor() const  { return its_destructor; }
			
			virtual void Attach( filehandle* target );
			
			virtual node_ptr GetFile();
			
			void set_file( const node& file );
			
			virtual void IOCtl( unsigned long request, int* argp );
	};
	
}

#endif

