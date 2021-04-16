/*
	is_grayscale.cc
	---------------
*/

#include "mac_qd/is_grayscale.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


namespace mac {
namespace qd  {

bool is_grayscale( const PixMap& pixmap )
{
	if ( CTabHandle ctab = pixmap.pmTable )
	{
		const long ctSeed = ctab[0]->ctSeed;
		
		return ctSeed == 2  ||  ctSeed == 32 + 4  ||  ctSeed == 32 + 8;
	}
	
	return false;
}

}
}
