/*
	VBL.hh
	------
*/

#ifndef VBL_HH
#define VBL_HH

struct QElem;

void do_VBL();

short VInstall_patch( QElem* vbl : __A0 );
short VRemove_patch ( QElem* vbl : __A0 );

#endif
