/*
	handler.hh
	----------
*/

#ifndef SYSCALL_HANDLER_HH
#define SYSCALL_HANDLER_HH

// Standard C
#include <stdint.h>


namespace v68k    {
namespace syscall {

const int patch_handler_word_count = 7;

extern const uint16_t patch_handler[ patch_handler_word_count ];

}  // namespace syscall
}  // namespace v68k

#endif
