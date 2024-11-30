/*
	handler.cc
	----------
*/

#include "syscall/handler.hh"


namespace v68k    {
namespace syscall {

const uint16_t patch_handler[ patch_handler_word_count ] =
{
	0x41EF,  // LEA  (2,A7),A0
	0x0002,
	
	0x5590,  // SUBQ.L  #2,(A0)
	
	0x2050,  // MOVEA.L  (A0),A0
	
	0x30BC,  // MOVE.W  #0x484A,(A0)
	0x484A,
	
	0x4E73   // RTE
};

}  // namespace syscall
}  // namespace v68k
