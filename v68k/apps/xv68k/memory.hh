/*
	memory.hh
	---------
*/

#ifndef MEMORY_HH
#define MEMORY_HH

// v68k-callouts
#include "callout/memory.hh"

// v68k-mac
#include "v68k-mac/memory.hh"


class memory_manager : public v68k::memory
{
	private:
		uint32_t                 its_low_mem_size;
		v68k::low_memory_region  its_low_mem;
		
		v68k::mac::low_memory  its_mac_low_mem;
		
		v68k::callout::memory  its_callout_memory;
	
	public:
		memory_manager( uint8_t*  low_mem_base,
		                uint32_t  low_mem_size );
		
		uint8_t* translate( uint32_t               addr,
		                    uint32_t               length,
		                    v68k::function_code_t  fc,
		                    v68k::memory_access_t  access ) const;
};

#endif
