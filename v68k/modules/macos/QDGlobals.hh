/*
	QDGlobals.hh
	------------
*/

#ifndef QDGLOBALS_HH
#define QDGLOBALS_HH

inline asm struct GrafPort** get_addrof_thePort() : __A0
{
	MOVEA.L  (A5),A0
}

#endif
