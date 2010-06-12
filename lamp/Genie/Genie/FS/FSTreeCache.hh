/*
	FSTreeCache.hh
	--------------
*/

#ifndef GENIE_FS_FSTREECACHE_HH
#define GENIE_FS_FSTREECACHE_HH

// Standard C++
#include <vector>

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	class FSTreeCache
	{
		private:
			typedef std::vector< FSNode > Vector;
			
			Vector nodes;
		
		public:
			typedef Vector::size_type        size_type;
			typedef Vector::const_reference  const_reference;
			
			size_type size() const  { return nodes.size(); }
			
			const_reference at( size_type i ) const  { return nodes.at( i ); }
			
			void push_back( const FSNode& node )
			{
				nodes.push_back( node );
			}
			
			friend void swap( FSTreeCache& a, FSTreeCache& b )
			{
				using std::swap;
				
				swap( a.nodes, b.nodes );
			}
	};
	
}

#endif

