/*
	map_anonymous.hh
	----------------
*/

#ifndef GENIE_MMAP_MAPANONYMOUS_HH
#define GENIE_MMAP_MAPANONYMOUS_HH

// Genie
#include "Genie/mmap/memory_mapping.hh"


namespace Genie
{
	
	boost::intrusive_ptr< memory_mapping > map_anonymous( std::size_t length );
	
}

#endif

