/*
	FSTreeCache_Impl.hh
	-------------------
*/

#ifndef GENIE_FS_FSTREECACHEIMPL_HH
#define GENIE_FS_FSTREECACHEIMPL_HH

// Standard C++
#include <vector>

// Genie
#include "Genie/FS/FSTreeCache.hh"


namespace Genie
{
	
	class FSTreeCache_Impl : public FSTreeCache
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
			
			friend void swap( FSTreeCache_Impl& a, FSTreeCache_Impl& b )
			{
				using std::swap;
				
				swap( a.nodes, b.nodes );
			}
	};
	
}

#endif

