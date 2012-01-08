/*
	dir_contents.hh
	---------------
*/

#ifndef VFS_DIRCONTENTS_HH
#define VFS_DIRCONTENTS_HH

// plus
#include "plus/ref_count.hh"

// vfs
#include "vfs/dir_entry.hh"


namespace vfs
{
	
	class dir_contents : public plus::ref_count< dir_contents >
	{
		private:
			// non-copyable
			dir_contents           ( const dir_contents& );
			dir_contents& operator=( const dir_contents& );
		
		public:
			dir_contents()
			{
			}
			
			virtual ~dir_contents()
			{
			}
			
			typedef const dir_entry& const_reference;
			
			void push_back( const dir_entry& node );
	};
	
}

#endif

