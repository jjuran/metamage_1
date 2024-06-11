/*
	HandleUtils.cc
	--------------
*/

#include "HandleUtils.hh"

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

// mac-glue-utils
#include "mac_glue/Memory.hh"

// ams-common
#include "callouts.hh"


#ifndef __TRAPS__

enum
{
	_NewHandle     = 0xA122,
	_SetHandleSize = 0xA024,
	_GetHandleSize = 0xA025,
	_BlockMove     = 0xA02E,
};

#endif

pascal long Munger_patch( Handle h, long i, Ptr p1, long n1, Ptr p2, long n2 )
{
	if ( h == NULL  ||  *h == NULL )
	{
		return -1;
	}
	
	Size size = mac::glue::GetHandleSize_raw( h );
	
	if ( i > size )
	{
		return -1;
	}
	
	char* p = *h + i;
	char* q = *h + size;
	
	if ( p1 != NULL )
	{
		long len = size - i;
		
		while ( len >= n1 )
		{
			if ( fast_memcmp( p, p1, n1 ) == 0 )
			{
				goto found;
			}
			
			++p;
			--len;
		}
		
		/*
			We didn't find the pattern string in its entirety.
			Try finding a truncation of it at the end of the subject string.
		*/
		
		while ( --n1 > 0 )
		{
			if ( fast_memcmp( p, p1, n1 ) == 0 )
			{
				goto found;
			}
			
			++p;
		}
		
		return -1;  // not found
	}
	
found:
	
	i = p - *h;
	
	if ( p2 == NULL )
	{
		return i;
	}
	
	long delta = n2 - n1;
	
	if ( delta > 0 )
	{
		size += delta;
		
		SetHandleSize( h, size );
		
		p = *h + i;
		
		i += n2;
		
		BlockMoveData( p + n1, p + n2, size - i );
	}
	
	fast_memcpy( p, p2, n2 );
	
	if ( delta < 0 )
	{
		size += delta;  // decrease size
		
		i += n2;
		
		BlockMoveData( p + n1, p + n2, size - i );
		
		SetHandleSize( h, size );
	}
	
	return i;
}

asm short HandToHand_patch( Handle h : __A0 )
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

asm short PtrToHand_patch( Ptr p : __A0, long size : __D0 )
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

asm short PtrToXHand_patch( Ptr p : __A0, Handle h : __A1, long n : __D0 )
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

asm short HandAndHand_patch( Handle src : __A0, Handle dest : __A1 )
{
	_GetHandleSize
	TST.L    D0
	BMI.S    end      // bail on error
	
	EXG      D0,D1    // D1 = src size
	EXG      A0,A1    // place dest in A0 for _SetHandleSize, save src in A1
	
	_GetHandleSize
	TST.L    D0
	BMI.S    end      // bail on error
	
	MOVE.L   D0,D2    // copy dest size
	ADD.L    D1,D0    // D0 = total size
	BMI.S    end      // size exceeds 0x7FFFFFFF
	
	_SetHandleSize
	BNE.S    end      // fail if err != noErr
	
	MOVE.L   D1,D0    // D0 = src size
	MOVE.L   A0,D1    // save dest
	
	EXG      A0,A1    // place src in A0, dest in A1
	
	MOVEA.L  (A0),A0  // dereference src
	MOVEA.L  (A1),A1  // dereference dest
	ADDA.L   D2,A1    // dest += dest_size
	
	_BlockMove
	
	MOVEA.L  D1,A0    // restore h
	
end:
	RTS
}

asm short PtrAndHand_patch( Ptr p : __A0, Handle h : __A1, long n : __D0 )
{
	EXG      D0,D1    // D1 = src size
	EXG      A0,A1    // place dest in A0 for _GetHandleSize, save src in A1
	
	_GetHandleSize
	TST.L    D0
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
