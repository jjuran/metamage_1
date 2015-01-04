/*
	line_A_shim.hh
	--------------
*/

#ifndef LINEASHIM_HH
#define LINEASHIM_HH

// Standard C
#include <stdint.h>


namespace v68k {
namespace user {

const int line_A_shim_word_count = 3;

extern const uint16_t line_A_shim[ line_A_shim_word_count ];

}  // namespace user
}  // namespace v68k

#endif
