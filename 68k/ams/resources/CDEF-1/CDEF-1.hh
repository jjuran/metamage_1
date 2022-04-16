/*
	CDEF-1.hh
	---------
*/

#ifndef CDEF1_HH
#define CDEF1_HH

struct ControlRecord;

typedef ControlRecord** ControlRef;

pascal
long CDEF_1( short varCode, ControlRef control, short message, long param );

#endif
