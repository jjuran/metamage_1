/*
	load.hh
	-------
*/

#ifndef V68KUSER_LOAD_HH
#define V68KUSER_LOAD_HH

// Standard C
#include <stdint.h>


namespace v68k {
namespace user {

struct os_load_spec
{
	uint8_t*  mem_base;
	uint32_t  mem_size;
	
	uint32_t  mem_used;
};

bool install_exception_handler( os_load_spec&    os,
                                int              index,
                                const uint16_t*  handler,
                                uint32_t         handler_size );

}  // namespace user
}  // namespace v68k

#endif
