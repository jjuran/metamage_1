/*
	OSUtils.hh
	----------
*/

#ifndef OSUTILS_HH
#define OSUTILS_HH

pascal long TickCount_patch();

pascal long Delay_patch( long numTicks : __A0 ) : __D0;

pascal void SysBeep_patch( short duration );

#endif
