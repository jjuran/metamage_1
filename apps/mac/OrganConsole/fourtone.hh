/*
	fourtone.hh
	-----------
*/

#ifndef FOURTONE_HH
#define FOURTONE_HH


typedef unsigned FTRate;

void fourtone_acquire_key_note( unsigned char keycode, FTRate rate );
void fourtone_release_not_down();

#endif
