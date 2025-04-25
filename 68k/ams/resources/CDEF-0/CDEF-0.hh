/*
	CDEF-0.hh
	---------
*/

#ifndef CDEF0_HH
#define CDEF0_HH

struct ControlRecord;

typedef ControlRecord** ControlRef;

pascal
long CDEF_0( short varCode, ControlRef control, short message, long param );

#endif
