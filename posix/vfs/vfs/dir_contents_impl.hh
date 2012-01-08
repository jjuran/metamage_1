/*
	dir_contents_impl.hh
	--------------------
*/

#ifndef VFS_DIRCONTENTSIMPL_HH
#define VFS_DIRCONTENTSIMPL_HH

// Standard C++
#include <vector>

// vfs
#include "vfs/dir_contents.hh"


namespace vfs
{
	
	class dir_contents_impl : public dir_contents
	{
		private:
			typedef std::vector< dir_entry > Vector;
			
			Vector entries;
		
		public:
			typedef Vector::size_type        size_type;
			typedef Vector::const_reference  const_reference;
			
			size_type size() const  { return entries.size(); }
			
			const_reference at( size_type i ) const  { return entries.at( i ); }
			
			void push_back( const dir_entry& node )
			{
				entries.push_back( node );
			}
			
			friend void swap( dir_contents_impl& a, dir_contents_impl& b )
			{
				using std::swap;
				
				swap( a.entries, b.entries );
			}
	};
	
}

#endif

