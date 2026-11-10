/*
	throttle.hh
	-----------
*/

#ifndef THROTTLE_HH
#define THROTTLE_HH

// Mac OS
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif


QHdr EventQueue : 0x014A;

inline
void enable_SystemTask_throttle()
{
	EventQueue.qFlags |= 1;
}

inline
void disable_SystemTask_throttle()
{
	EventQueue.qFlags &= ~1;
}

inline
unsigned ticks_to_wait_in_SystemTask()
{
	return EventQueue.qFlags & 1;
}

#endif
