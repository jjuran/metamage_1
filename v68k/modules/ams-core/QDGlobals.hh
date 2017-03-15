/*
	QDGlobals.hh
	------------
*/

#ifndef QDGLOBALS_HH
#define QDGLOBALS_HH

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

inline asm struct GrafPort** get_addrof_thePort() : __A0
{
	MOVEA.L  (A5),A0
}

static inline
QDGlobals& get_QDGlobals()
{
	GrafPtr* const thePort_addr = get_addrof_thePort();
	
	return *(QDGlobals*) ((char*) thePort_addr - offsetof(QDGlobals, thePort));
}

#endif
