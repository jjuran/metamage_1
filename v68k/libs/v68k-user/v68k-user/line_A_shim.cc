/*
	line_A_shim.cc
	--------------
*/

#include "v68k-user/line_A_shim.hh"


namespace v68k {
namespace user {

const uint16_t line_A_shim[ line_A_shim_word_count ] =
{
	0x54AF,  // ADDQ.L  #2,(2,A7)
	0x0002,
	
	0x4E73   // RTE
};

}  // namespace user
}  // namespace v68k
