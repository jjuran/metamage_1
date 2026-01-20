/*
	QDGlobals.hh
	------------
*/

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


enum
{
	offsetof_randSeed   = 76,
	offsetof_screenBits = offsetof_randSeed   + sizeof (long),
	offsetof_arrow      = offsetof_screenBits + sizeof (BitMap),
	offsetof_dkGray     = offsetof_arrow      + sizeof (Cursor),
	offsetof_ltGray     = offsetof_dkGray     + sizeof (Pattern),
	offsetof_gray       = offsetof_ltGray     + sizeof (Pattern),
	offsetof_black      = offsetof_gray       + sizeof (Pattern),
	offsetof_white      = offsetof_black      + sizeof (Pattern),
	offsetof_thePort    = offsetof_white      + sizeof (Pattern),
	
	thePort_to_screenBits = offsetof_screenBits - offsetof_thePort,
};

inline
asm
BitMap* get_screenBits()
{
	MOVEA.L  (A5),A0  // &qd.thePort
	LEA      thePort_to_screenBits(A0),A0  // add a negative value
}
