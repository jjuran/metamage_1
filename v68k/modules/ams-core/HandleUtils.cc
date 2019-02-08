/*
	HandleUtils.cc
	--------------
*/

#include "HandleUtils.hh"

// Mac OS
#ifndef __TRAPS__
#include <Traps.h>
#endif


asm short HandToHand_patch( char** h : __A0 )
{
	_GetHandleSize
	TST.L    D0
	BMI.S    end
	
	MOVE.L   (A0),D2   // dereference the input handle and save the pointer
	MOVE.L   D0,D1     // save the handle size
	
	_NewHandle
	BMI.S    end
	
	MOVEA.L  (A0),A1   // dereference the new handle into A1
	EXG      D2,A0     // move the data pointer into A0, save the new handle
	MOVE.L   D1,D0     // restore the handle size
	
	_BlockMove         // copy the data
	
	MOVEA.L  D2,A0     // set the new handle as the result
	MOVEQ.L  #0,D0     // set noErr as the result code
	
end:
	RTS

}
