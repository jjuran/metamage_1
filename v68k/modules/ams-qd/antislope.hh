/*
	antislope.hh
	------------
*/

#ifndef ANTISLOPE_HH
#define ANTISLOPE_HH

// Mac OS
#ifndef __FIXMATH__
#include <FixMath.h>
#endif


inline
long antislope( short a_h, short a_v, short b_h, short b_v )
{
	// Slope is rise/run.  This is the reciprocal of that.
	
	short rise = a_v - b_v;
	short run  = a_h - b_h;
	
	return FixRatio( run, rise );
}

// Sorry about the macro, but MWC68K doesn't inline properly.

#define antislope( a, b )  antislope( a.h, a.v, b.h, b.v )

#endif
