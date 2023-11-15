/*
	trap_dispatcher.hh
	------------------
*/

#ifndef V68KMAC_TRAPDISPATCHER_HH
#define V68KMAC_TRAPDISPATCHER_HH

// Standard C
#include <stdint.h>


namespace v68k {
namespace mac {

const int trap_dispatcher_word_count = 117;

extern const uint16_t trap_dispatcher[ trap_dispatcher_word_count ];

}  // namespace mac
}  // namespace v68k

#endif
