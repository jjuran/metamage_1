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
	
end:
	RTS

}

asm short PtrToHand_patch( char* p : __A0, long size : __D0 )
{
	MOVE.L   A0,D2     // save the input pointer
	MOVE.L   D0,D1     // save the input size
	
	_NewHandle
	BMI.S    end
	
	MOVEA.L  (A0),A1   // dereference the new handle into A1
	EXG      D2,A0     // move the data pointer into A0, save the new handle
	MOVE.L   D1,D0     // restore the handle size
	
	_BlockMove         // copy the data
	
	MOVEA.L  D2,A0     // set the new handle as the result
	
end:
	RTS
}

asm short PtrToXHand_patch( const void* p : __A0, char** h : __A1, long : __D0 )
{
	MOVE.L   A0,D2     // save the input pointer
	MOVE.L   D0,D1     // save the input size
	MOVEA.L  A1,A0     // place h in A0 for _SetHandleSize
	
	_SetHandleSize
	BNE.S    end       // fail if err != noErr
	
	MOVEA.L  (A0),A1   // dereference the handle into A1
	EXG      D2,A0     // move the data pointer into A0, save the handle
	MOVE.L   D1,D0     // restore the input size
	
	_BlockMove         // copy the data
	
	MOVEA.L  D2,A0     // set h as the result
	
end:
	RTS
}

asm short PtrAndHand_patch( const void* p : __A0, char** h : __A1, long : __D0 )
{
	EXG      D0,D1    // D1 = src size
	EXG      A0,A1    // place dest in A0 for _SetHandleSize, save src in A1
	
	_GetHandleSize
	BMI.S    end      // bail on error
	
	MOVE.L   D0,D2    // copy dest size
	ADD.L    D1,D0    // D0 = total size
	BMI.S    end      // size exceeds 0x7FFFFFFF
	
	_SetHandleSize
	BNE.S    end      // fail if MemErr != noErr
	
	MOVE.L   D1,D0    // D0 = src size
	MOVE.L   A0,D1    // save dest
	
	EXG      A0,A1    // place src in A0, dest in A1
	
	MOVEA.L  (A1),A1  // dereference dest
	ADDA.L   D2,A1    // dest += dest_size
	
	_BlockMove
	
	MOVEA.L  D1,A0    // restore h
	
end:
	RTS
}
