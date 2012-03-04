/*
	handler.hh
	----------
*/

#ifndef SYSCALL_HANDLER_HH
#define SYSCALL_HANDLER_HH

// Standard C
#include <stdint.h>


const int system_call_word_count = 7;

extern const uint16_t system_call[ system_call_word_count ];


#endif

