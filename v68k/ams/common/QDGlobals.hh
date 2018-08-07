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
QDGlobals& get_QDGlobals( GrafPort** a5_world )
{
	return *(QDGlobals*) ((char*) a5_world - offsetof(QDGlobals, thePort));
}

static inline
QDGlobals& get_QDGlobals()
{
	return get_QDGlobals( get_addrof_thePort() );
}

#endif
