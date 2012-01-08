/*
	FSTreeCache.hh
	--------------
*/

#ifndef GENIE_FS_FSTREECACHE_HH
#define GENIE_FS_FSTREECACHE_HH

// plus
#include "plus/ref_count.hh"

// Genie
#include "Genie/FS/FSNode.hh"


namespace Genie
{
	
	class FSTreeCache : public plus::ref_count< FSTreeCache >
	{
		private:
			// non-copyable
			FSTreeCache           ( const FSTreeCache& );
			FSTreeCache& operator=( const FSTreeCache& );
		
		public:
			FSTreeCache()
			{
			}
			
			virtual ~FSTreeCache()
			{
			}
			
			typedef const FSNode& const_reference;
			
			void push_back( const FSNode& node );
	};
	
}

#endif

