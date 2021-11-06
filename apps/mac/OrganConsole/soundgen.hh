/*
	soundgen.hh
	-----------
*/

#ifndef SOUNDGEN_HH
#define SOUNDGEN_HH

typedef unsigned FTRate;

void fourtone_start( FTRate rate );
void fourtone_amend_rate( int channel, FTRate rate );
void fourtone_amend_submit();

void full_stop();

#endif
