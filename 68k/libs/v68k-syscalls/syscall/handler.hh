/*
	handler.hh
	----------
*/

#ifndef SYSCALL_HANDLER_HH
#define SYSCALL_HANDLER_HH

// Standard C
#include <stdint.h>


const int syscall_patch_handler_word_count = 7;

extern const uint16_t syscall_patch_handler[ syscall_patch_handler_word_count ];


#endif
