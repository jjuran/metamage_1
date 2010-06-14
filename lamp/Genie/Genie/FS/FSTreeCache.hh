/*
	FSTreeCache.hh
	--------------
*/

#ifndef GENIE_FS_FSTREECACHE_HH
#define GENIE_FS_FSTREECACHE_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	class FSTreeCache
	{
		protected:
			// protected destructor prevents slicing
			~FSTreeCache()  {}
		
		public:
			typedef const FSNode& const_reference;
			
			void push_back( const FSNode& node );
	};
	
}

#endif

