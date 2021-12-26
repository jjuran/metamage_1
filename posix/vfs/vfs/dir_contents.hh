/*
	dir_contents.hh
	---------------
*/

#ifndef VFS_DIRCONTENTS_HH
#define VFS_DIRCONTENTS_HH

// Standard C++
#include <vector>

// vxo
#include "vxo/ref_count.hh"

// vfs
#include "vfs/dir_entry.hh"


namespace vfs
{
	
	class dir_contents : public vxo::ref_count< dir_contents >
	{
		private:
			typedef std::vector< dir_entry > Vector;
			
			Vector its_entries;
		
		private:
			// non-copyable
			dir_contents           ( const dir_contents& );
			dir_contents& operator=( const dir_contents& );
		
		public:
			dir_contents()
			{
			}
			
			~dir_contents();
			
			typedef Vector::size_type        size_type;
			typedef Vector::const_reference  const_reference;
			
			size_type size() const  { return its_entries.size(); }
			
			const_reference at( size_type i ) const  { return its_entries.at( i ); }
			
			void push_back( const dir_entry& node )
			{
				its_entries.push_back( node );
			}
			
			friend void swap( dir_contents& a, dir_contents& b )
			{
				using std::swap;
				
				swap( a.its_entries, b.its_entries );
			}
	};
	
}

#endif
