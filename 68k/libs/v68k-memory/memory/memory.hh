/*
	memory.hh
	---------
*/

#ifndef MEMORY_MEMORY_HH
#define MEMORY_MEMORY_HH

// v68k
#include "v68k/memory.hh"


namespace v68k {

enum usermode_memory_access
{
	Usermode_memory_none,
	Usermode_memory_real,
	Usermode_memory_fake = -1,
};

extern usermode_memory_access major_system_vector_access;

inline
bool usermode_sees_real_system_vectors()
{
	return major_system_vector_access > 0;
}

inline
bool usermode_sees_fake_system_vectors()
{
	return major_system_vector_access < 0;
}

extern uint32_t alt_screen_addr;
extern uint32_t main_screen_addr;
extern uint32_t main_sound_addr;

class memory_manager : public v68k::memory
{
	public:
		memory_manager( uint8_t*  low_mem_base,
		                uint32_t  low_mem_size );
		
		static
		uint8_t* translate( uint32_t               addr,
		                    uint32_t               length,
		                    v68k::function_code_t  fc,
		                    v68k::memory_access_t  access );
};

}  // namespace v68k

#endif
