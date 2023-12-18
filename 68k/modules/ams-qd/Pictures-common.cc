/*
	Pictures-common.cc
	------------------
*/

#include "Pictures-common.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-common
#include "callouts.hh"


#pragma exceptions off


short read_word( const Byte*& p )
{
	short result;
	fast_memcpy( &result, p, sizeof result );
	
	p += sizeof result;
	
	return result;
}

Rect read_Rect( const Byte*& p )
{
	Rect result;
	fast_memcpy( &result, p, sizeof result );
	
	p += sizeof result;
	
	return result;
}
