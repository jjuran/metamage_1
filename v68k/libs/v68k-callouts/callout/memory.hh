/*
	memory.hh
	---------
*/

#ifndef CALLOUTMEMORY_HH
#define CALLOUTMEMORY_HH

// v68k
#include "v68k/memory.hh"


namespace v68k    {
namespace callout {

class memory : public v68k::memory
{
	public:
		uint8_t* translate( uint32_t               addr,
		                    uint32_t               length,
		                    v68k::function_code_t  fc,
		                    v68k::memory_access_t  access ) const;
};

}  // namespace callout
}  // namespace v68k


#endif
