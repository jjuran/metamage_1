/*
	memory.hh
	---------
*/

#ifndef V68KMACMEMORY_HH
#define V68KMACMEMORY_HH

// v68k
#include "v68k/memory.hh"


namespace v68k {
namespace mac  {

class low_memory : public v68k::memory
{
	public:
		uint8_t* translate( uint32_t               addr,
		                    uint32_t               length,
		                    v68k::function_code_t  fc,
		                    v68k::memory_access_t  access ) const;
};

}  // namespace v68k
}  // namespace mac


#endif

