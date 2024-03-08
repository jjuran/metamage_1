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

GrafPort*** CurrentA5 : 0x0904;

inline asm
struct GrafPort** get_addrof_thePort()
{
	MOVEA.L  (A5),A0
}

inline
QDGlobals& get_QDGlobals( GrafPort** a5_world )
{
	return *(QDGlobals*) ((char*) a5_world - offsetof(QDGlobals, thePort));
}

inline
QDGlobals& get_QDGlobals()
{
	return get_QDGlobals( get_addrof_thePort() );
}

inline
QDGlobals& get_QDGlobals_from_CurrentA5()
{
	return get_QDGlobals( *CurrentA5 );
}

#endif
