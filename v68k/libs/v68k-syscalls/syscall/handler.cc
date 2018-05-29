/*
	handler.cc
	----------
*/

#include "syscall/handler.hh"


const uint16_t syscall_patch_handler[ syscall_patch_handler_word_count ] =
{
	0x41EF,  // LEA  (2,A7),A0
	0x0002,
	
	0x5590,  // SUBQ.L  #2,(A0)
	
	0x2050,  // MOVEA.L  (A0),A0
	
	0x30BC,  // MOVE.W  #0x484A,(A0)
	0x484A,
	
	0x4E73   // RTE
};
